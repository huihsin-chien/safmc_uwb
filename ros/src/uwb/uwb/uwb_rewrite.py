# ROS2
import rclpy
from rclpy.node import Node
from rclpy.qos import QoSDurabilityPolicy, QoSHistoryPolicy, QoSProfile, QoSReliabilityPolicy
from uwb_msgs.msg import TagPosition # 自定義的 ROS2 Message

# 用於透過 USB 讀取 UWB Anchor，與截取資訊
import serial
import re

# 用於 Time Stamp
import time

# 用於儲存資料
import os
import csv

# 用於統計上排除極值
import numpy as np

# 用於多點定位
from scipy import minimize

# 一些設定
SAVE_DATA = False # 是否儲存 UWB 設備的位置資訊用於 debug
DATA_FOLDER = os.path.join(os.getcwd(), "output") # 儲存資料的資料夾

# 為了幾乎沒有的效能差異，我們預先編譯正則表達式（Regular Expression）
range_data_regexp = re.compile(
    r'Range:\s(-?\d*\.\d*|-?\binf\b)\sm\t\sRX\spower:\s(-?\d*\.\d*|-?\binf\b)\sdBm\sdistance\sbetween\sanchor\/tag:(\d{2,4})\sfrom\sAnchor\s(\d{2}:\d{2})')
sample_rate_data_regexp = re.compile(
    r'Sampling\srate\sof\s{2}Anchor[A-H]:\s(-?\d*\.\d*|-?\binf\b)Hz\s{4}Anchor:(\d{2}:\d{2})')


# 由於……所以我們需要一個狀態機
class stateMachine:
    def __init__(self):
        self.status = "built_coord_1"
    def switch_to_next_step(self): # 這個函數不會被呼叫，只是用來展示狀態的切換
        states = ["built_coord_1", "built_coord_2", "self_calibration", "flying"]
        self.status = states[states.index(self.status) + 1] if self.status != "flying" else "flying"

# UWB 設備分為固定位置的 Anchor，和無人機上移動中的的 Tag
# 這幾個類別用來儲存他們的位置
class UWBDevice:
    def __init__(self, name: str, eui: str):
        self.name = f"{name}"
        self.eui = eui
        self.coordinate = None
            # coordinate 是一個有三個元素的 Array，表示 x, y, z
            # 未設定時，coordinate 為 None
    
class Anchor(UWBDevice):
    idx: int = 0
    eui: str = ""
    def __init__(self, idx: int, eui: str):
        self.idx = idx
        super().__init__(f"Anchor_{idx}", eui)
    
    # 用於設定 Anchor 的位置。一經設定，就不可更改
    def set_coordinate(self, coordinate) -> None:
        if self.coordinate is not None:
            raise Exception("coordinate is already set.")
        self.coordinate = coordinate

class Tag(UWBDevice):
    idx: int = 0
    eui: str = ""
    def __init__(self, idx: int, eui: str):
        self.idx = idx
        super().__init__(f"Tag_{idx}", eui)
    
    # 用於更新 Tag 的位置
    def update_coordinate(self, coordinate) -> None:
        self.coordinate = coordinate

# 用於記錄 UWB 的各 Anchor & Tag 之間的測得距離與時間
class UWBData:
    distance: float = 0
    timestamp: float = 0

    def __init__(self, distance: float):
        self.distance = distance
        self.timestamp = time.time()

# 用於管理 UWB 設備，包含
# - 儲存 UWB 設備資訊、及他們之間的距離關係
# - 清除過時距離資訊
# - 設定 Anchor 的座標
# - 多點定位計算 Tag 座標並儲存
class UWBDataMatrix:
    time_threshold: float = 0.5     # 資料過時的時間門檻（秒）
    anchors: dict[str, Anchor] = {} # Anchor 的 EUI 對應到 Anchor 物件
    tags: dict[str, Tag] = {}       # Tag 的 EUI 對應到 Tag 物件
    data: dict[str, dict[str, list[UWBData]]] = {}
    anchor_sample_rate: dict[str, str] = {}

    def __init__(self, time_threshold: float, anchors: list[Anchor] = [], tags: list[Tag] = []):
        self.time_threshold = time_threshold
        self.anchors = { anchor.eui: anchor for anchor in anchors }
        self.tags = { tag.eui: tag for tag in tags }
        self.data = { tag.eui: { anchor.eui: [] for anchor in anchors } for tag in tags }

        # 如果要為了 Debug 而儲存資料，則建立檔案
        if SAVE_DATA:
            self.timestamp_str = time.strftime('%Y%m%d_%H%M%S', time.time()) # 記錄建立時間，用於儲存資料時的檔名

            # 為每個 Anchor 建立一個檔案，用以儲存測量資訊
            for anchor in anchors: 
                anchor_eui_encoded = anchor.eui.replace(":", "-")
                anchor_file_path = os.path.join(DATA_FOLDER, f"Anchor{anchor_eui_encoded}_{self.timestamp_str}.csv")
                with open(anchor_file_path, mode='w') as file:
                    csv_writer = csv.writer(file)
                    csv_writer.writerow(["timestamp", "tag_eui", "distance", "sample_rate"]) # 標題
            
            # 建立檔案，用以儲存定位結果
            self.multilateration_file = os.path.join(output_folder, f"multilateration_results_{self.timestamp_str}.csv")
            with open(self.multilateration_file, mode='w', newline='') as file:
                csv_writer = csv.writer(file)
                csv_writer.writerow(["timestamp", "x", "y", "z"])

    # 將測量資訊加入資料庫
    def add_measurement(self, tag_eui: str, anchor_eui: str, distance: float) -> None:
        self.data[tag_eui][anchor_eui].append(UWBData(distance))
        if SAVE_DATA:
            anchor_eui_encoded = anchor_eui.replace(":", "-")
            anchor_file_path = os.path.join(DATA_FOLDER, f"Anchor{anchor_eui_encoded}_{self.timestamp}.csv")
            with open(anchor_file_path, mode='a') as file:
                csv_writer = csv.writer(file)
                timestamp_str = time.strftime('%Y-%m-%d %H:%M:%S', time.time())
                csv_writer.writerow([timestamp_str, tag_eui, distance, sample_rate[anchor_eui]])

    # 清除過時的測量資訊
    def clear_outdated_measurements(self, tag_eui: str, anchor_eui: str) -> None:
        measurements = self.data[tag_eui][anchor_eui]
        while len(x) > 0 and measurements[0].timestamp < time.time() - self.time_threshold:
            self.data[tag_eui][anchor_eui].pop(0)

    # 取得去除極值的測量資訊
    def get_distance(self, tag_eui: str, anchor_eui: str) -> float or None:
        # 清除過時資訊
        clear_outdated_measurements(tag_eui, anchor_eui)

        # 去除極值
        measurements = self.data[tag_eui][anchor_eui]
        distances = [measurement.distance for measurement in measurements]
        q1 = np.percentile(distances, 25)
        q3 = np.percentile(distances, 75)
        filtered_distances = [distance for distance in distances if q1 <= distance <= q3]

        # 計算並回傳平均值
        return np.mean(filtered_distances) if len(filtered_distances) > 0 else None

    # 設定 Anchor 的座標
    def set_anchor_coordinates(self) -> None:
        pass # TODO

    # 計算多點定位（multilateration）的結果
    def locate_tag(self, tag_eui) -> Tuple[float, float, float]:
        pass 
        # TODO:
        # - 完善定位算法
        # - `self.tags[tag_eui].coordinate = [x, y, z]`
        # - 如果 SAVE_DATA，則儲存結果到 CSV 檔案

    # 更新 Anchor Sample Rate
    def update_anchor_sample_rate(self, anchor_eui: str, sample_rate: float) -> None:
        self.anchor_sample_rate[anchor_eui] = sample_rate

# 主要的 Node，用於讀取 UWB 設備的資訊、計算 3D 座標後，發布到 ROS Topic
class UWBPublisher(Node):
    serials: list[serial.Serial] = []
    uwb_data_matrix: UWBDataMatrix = None
    state_machine: stateMachine = None
    def __init__(self):
        super().__init__('position_publisher')

        # 如果要為了 Debug 而儲存資料，則建立資料夾以及檔案
        if SAVE_DATA:
            os.makedirs(DATA_FOLDER, exist_ok=True)

            # 儲存多點定位（multilateration）的結果
            self.multilateration_file = os.path.join(output_folder, f"multilateration_results_{time.strftime('%Y%m%d_%H%M%S')}.csv")
            with open(self.multilateration_file, mode='w', newline='') as file:
                csv_writer = csv.writer(file)
                csv_writer.writerow(["timestamp", "x", "y", "z"]) # 標題

        # 開啟 Ports 用以讀取 UWB 設備
        ports = [] # TODO 從 params.yaml 讀取 list(serial.tools.list_ports.comports())
        for port in ports:
            serial_connection = serial.Serial(port, baudrate=9600, timeout=1)
            self.serials.append(serial_connection)
        
        # 指定 ROS Topic 的傳輸行為與品質（QoS, Quality of Service)
        qos_profile = QoSProfile(
            reliability=QoSReliabilityPolicy.BEST_EFFORT,   # 不保證傳輸成功
            durability=QoSDurabilityPolicy.TRANSIENT_LOCAL, # 為後期加入的 Subscriber 保留資料
            history=QoSHistoryPolicy.KEEP_LAST, depth=1     # 只保留最新的 depth (= 1) 筆資料
        )

        # 定義 ROS Topic
        self.tag_position_publisher = self.create_publisher(
            TagPosition,
            '/tag_position',
            qos_profile
        )

        # 設定 UWB Data Matrix
        anchors = [Anchor(1, "00:01"), Anchor(2, "00:02"), Anchor(3, "00:03"), Anchor(4, "00:04"), \
            Anchor(5, "00:05"), Anchor(6, "00:06"), Anchor(7, "00:07"), Anchor(8, "00:08")]
        tags = [Tag(1, "01:01"), Tag(2, "02:02"), Tag(3, "03:03"), Tag(4, "04:04"), \
            Tag(5, "05:05"), Tag(6, "06:06"), Tag(7, "07:07"), Tag(8, "08:08")]
        self.uwb_data_matrix = UWBDataMatrix(time_threshold=0.5, anchors=anchors, tags=tags)
        
        # 設定狀態機
        self.state_machine = stateMachine()

        # 定期透過 USB Serial 讀取 UWB 裝置距離
        # 與定期發佈 Tag 的位置
        self.timer1 = self.create_timer(0.05, self.read_serial)
        self.timer2 = self.create_timer(0.05, self.publish_tag_position)
    
    # 透過 USB Serial 讀取 UWB 裝置距離
    def read_serial(self) -> None:
        for serial_connection: serial.Serial in self.serials: # 逐一讀取每個 Serial Port
            lines = [line.decode("utf-8").strip() for serial_connection.readlines()]
            for line in lines: # 逐一讀取每一行資料
                if not line:
                    continue
                
                # 每一行有多種可能的資料：距離資料、採樣率資料、狀態遷移資料
                # - 距離資料 e.g. `Range: 1.23 m     RX power: -45.67 dBm distance between anchor/tag:01:01 from Anchor 00:01`
                # - 採樣率資料 e.g. `Sampling rate of  AnchorA: 10.0Hz     Anchor:00:01`
                # - 狀態遷移資料：太複雜了，請看 `anchor output.txt`
                range_data_match = range_data_regexp.search(line)
                sample_rate_data_match = sample_rate_data_regexp.search(line)

                if range_data_match: # 對於距離資料，新增測距結果到 UWB Data Matrix
                    distance, power, from_id, anchor_eui = range_data_match.groups()
                    anchor_eui: str = "" # somehow get the anchor_eui from from_id

                    self.uwb_data_matrix.add_measurement(tag_eui, anchor_eui, float(distance))

                elif sample_rate_data_match: # 對於採樣率資料，更新 Anchor 的採樣率到 UWB Data Matrix
                    sample_rate, anchor_eui = sample_rate_data_match.groups()

                    self.uwb_data_matrix.update_anchor_sample_rate(anchor_eui, float(sample_rate))

                else: # 對於狀態遷移資料，更新狀態機的狀態
                    for state in ["built_coord_1", "built_coord_2", "self_calibration", "flying"]:
                        if state in line:
                            self.state_machine.status = state
    
    # 定期發佈 Tag 的位置
    def publish_tag_position(self) -> None:
        for tag_eui in self.uwb_data_matrix.tags.keys():
            x, y, z = self.uwb_data_matrix.locate_tag(tag_eui)
            if tag_coordinate is not None:
                msg = TagPosition()
                msg.EUI = tag.eui
                msg.x = x
                msg.y = y
                msg.z = z
                self.tag_position_publisher.publish(msg)

# main 函數，僅在直接執行這個檔案時才執行
def main(args=None):
    rclpy.init(args=args)
    position_publisher = UWBPublisher()

    try: # 試圖保持程式運行。如果程式被強制終止，以 finally 正確結束程式
        rclpy.spin(position_publisher)
    finally:
        position_publisher.destroy_node()  
        rclpy.shutdown()
    
if __name__ == '__main__':
    main()