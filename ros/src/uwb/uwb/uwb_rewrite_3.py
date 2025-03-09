# ROS2
import serial.tools
import serial.tools.list_ports
import rclpy
from rclpy.node import Node
from rclpy.qos import QoSDurabilityPolicy, QoSHistoryPolicy, QoSProfile, QoSReliabilityPolicy
from agent_msgs.msg import TagPosition # 自定義的 ROS2 Message

# 用於透過 USB 讀取 UWB Anchor，與截取資訊
import serial
import re

# 用於 Time Stamp
import time

# 用於儲存資料
import os
import csv

# 用於統計上排除極值、與 3D 定位
import numpy as np
from typing import Optional, Tuple
from .build_3D_coord import build_3D_coord

# 用於多點定位
from scipy.optimize import minimize

def dbg(*args, **kwargs):
    print(*args, **kwargs)

# 一些設定
SAVE_DATA = True # 是否儲存 UWB 設備的位置資訊用於 debug
DATA_FOLDER = os.path.join(os.getcwd(), "output") # 儲存資料的資料夾

# 為了幾乎沒有的效能差異，我們預先編譯正則表達式（Regular Expression）
range_data_regexp = re.compile(
    r'Range:\s(-?\d*\.\d*|-?\binf\b)\sm\t\sRX\spower:\s(-?\d*\.\d*|-?\binf\b)\sdBm\sdistance\sbetween\sanchor\/tag:(\d{2,4})\sfrom\sAnchor\s(\d{2}:\d{2})')
sample_rate_data_regexp = re.compile(
    r'Sampling\srate\sof\s{2}Anchor[A-H]:\s(-?\d*\.\d*|-?\binf\b)Hz\s{4}Anchor:(\d{2}:\d{2})')

# 多點定位算法
# ref: https://github.com/glucee/Multilateration/blob/master/Python/example.py
def gps_solve(distances_to_station: list[float], stations_coordinates: list[np.ndarray]) -> np.ndarray or None:
    """多邊定位算法 若有
    :param distances_to_station: list[float], 到各個 anchor 的距離
    :param stations_coordinates: list[np.ndarray], 各 anchor 的座標（每個 anchor 座標都是 len = 3 的 np.ndarray）
    :return: np.ndarray, 估計的位置, len = 3
    """
    def error(x, c, r):
        return sum([(np.linalg.norm(x - c[i]) - r[i]) ** 2 for i in range(min(len(c), len(r)))])

    l = len(stations_coordinates)
    S = sum(distances_to_station)

    # 為初始推測計算權重
    W = []
    if all(S - w != 0 for w in distances_to_station):
        W = [((l - 1) * S) / (S - w) for w in distances_to_station]
    else:
        print("Error: Only one distance provided")
        return None
        
    # get initial guess of point location
    Length = len(W)
    x0 = sum([W[i] * stations_coordinates[i] for i in range(Length)])
    
    # optimize distance from signal origin to border of spheres
    return minimize(error, x0, args=(stations_coordinates, distances_to_station), method='Nelder-Mead').x

# 用於記錄 UWB 的各 Anchor & Tag 之間的測得距離與時間
class UWBData:
    distance: float = 0
    timestamp: float = 0

    def __init__(self, distance: float):
        self.distance = distance
        self.timestamp = time.time()
        
# UWB 設備分為固定位置的 Anchor，和無人機上移動中的的 Tag
# 這幾個類別用來儲存他們的位置
class UWBDevice:
    eui: str = ""
    coordinate: Optional[list[float]] = None
    def __init__(self, eui: str):
        self.eui = eui
        self.coordinate = None
            # coordinate 是一個有三個元素的 Array，表示 x, y, z
            # 未設定時，coordinate 為 None

    # 用於設定座標
    def update_coordinate(self, coordinate) -> None:
        self.coordinate = coordinate

# 用於管理 UWB 設備，包含
# - 儲存 UWB 設備資訊、及他們之間的距離關係
# - 清除過時距離資訊
# - 設定 Anchor 的座標
# - 多點定位計算 Tag 座標並儲存
class UWBDataMatrix:
    time_threshold: float = 0.5                     # 資料過時的時間門檻（秒）
    anchors: dict[str, UWBDevice] = {}              # Anchor 的 EUI 對應到 Anchor 物件
    tags: dict[str, UWBDevice] = {}                 # Tag 的 EUI 對應到 Tag 物件
    data: dict[str, dict[str, list[UWBData]]] = {}  # { TAG_EUI: { ANCHOR_EUI: [UWBData, UWBData, ...] } }
    anchor_sample_rate: dict[str, str] = {}

    def __init__(self, time_threshold: float, anchors: list[UWBDevice] = [], tags: list[UWBDevice] = []):
        self.time_threshold = time_threshold
        self.anchors = { anchor.eui: anchor for anchor in anchors }
        self.tags = { tag.eui: tag for tag in tags }
        self.data = { tag.eui: { anchor.eui: [] for anchor in anchors } for tag in tags }
        

        # 如果要為了 Debug 而儲存資料，則建立檔案
        if SAVE_DATA:
            self.timestamp_str = time.strftime('%Y%m%d_%H%M%S') # 記錄建立時間，用於儲存資料時的檔名

            # 為每個 Anchor 建立一個檔案，用以儲存測量資訊
            for anchor in anchors: 
                anchor_eui_encoded = anchor.eui.replace(":", "-")
                anchor_file_path = os.path.join(DATA_FOLDER, f"Anchor{anchor_eui_encoded}_{self.timestamp_str}.csv")
                with open(anchor_file_path, mode='w') as file:
                    csv_writer = csv.writer(file, escapechar='"')
                    csv_writer.writerow(["timestamp", "tag_eui", "distance", "sample_rate"]) # 標題
            
            # 建立檔案，用以儲存定位結果
            self.multilateration_file = os.path.join(DATA_FOLDER, f"multilateration_results_{self.timestamp_str}.csv")
            with open(self.multilateration_file, mode='w', newline='') as file:
                csv_writer = csv.writer(file, escapechar='"')
                csv_writer.writerow(["timestamp", "x", "y", "z"])

    # 將測量資訊加入資料庫
    def add_measurement(self, tag_eui: str, anchor_eui: str, distance: float) -> None:
        dbg(f"- - - adding measurement from {tag_eui} to {anchor_eui}: {distance}m")
        if tag_eui not in self.data:
            # dbg(f"- - - no tag_eui {tag_eui} in data")
            return
        if anchor_eui not in self.data[tag_eui]:
            dbg(f"- - - no anchor_eui {anchor_eui} in data")
            return

        self.data[tag_eui][anchor_eui].append(UWBData(distance))
        timestamp_str = time.strftime('%Y-%m-%d %H:%M:%S')
        if SAVE_DATA:
            anchor_eui_encoded = anchor_eui.replace(":", "-")
            anchor_file_path = os.path.join(DATA_FOLDER, f"Device_{anchor_eui_encoded}_{self.timestamp_str}.csv")
            with open(anchor_file_path, mode='a') as file:
                csv_writer = csv.writer(file, escapechar='"')
                csv_writer.writerow([timestamp_str, tag_eui, distance, self.anchor_sample_rate[anchor_eui] if anchor_eui in self.anchor_sample_rate else "N/A"])

    # 清除過時的測量資訊
    def clear_outdated_measurements(self, tag_eui: str, anchor_eui: str) -> None:
        measurements = self.data[tag_eui][anchor_eui]
        while len(measurements) > 0 and measurements[0].timestamp < time.time() - self.time_threshold:
            self.data[tag_eui][anchor_eui].pop(0)

    # 取得去極值後的距離
    def get_distance(self, tag_eui: str, anchor_eui: str) -> Optional[float]:
        # 清除過時資訊
        self.clear_outdated_measurements(tag_eui, anchor_eui)

        # 取得所測得距離陣列
        measurements: list[UWBData] = self.data[tag_eui][anchor_eui]
        distances: list[float] = [measurement.distance for measurement in measurements]

        # 去除極值
        q1 = np.percentile(distances, 25) if distances else float("-inf")
        q3 = np.percentile(distances, 75) if distances else float("inf")
        filtered_distances = [distance for distance in distances if q1 <= distance <= q3]

        # TODO: 也許這裡需要修正固定偏差值，來修正系統誤差、提高準確度

        # 計算並回傳平均值
        return np.mean(filtered_distances) if len(filtered_distances) > 0 else None

    # 計算多點定位（multilateration）的結果
    def locate_tag(self, tag_eui) -> Optional[Tuple[float, float, float]]:
        # 如果沒有這個 Tag 的資料，則回傳 None
        if tag_eui not in self.data:
            return None 

        # 蒐集該 tag 到各 anchor 的距離 & 各 anchor 座標
        distances_to_stations = []
        stations_coordinates = []
        for anchor_eui in self.anchors.keys():
            distance = self.get_distance(tag_eui, anchor_eui)
            if distance is not None:
                distances_to_stations.append(distance)
                stations_coordinates.append(self.anchors[anchor_eui].coordinate)

        # 如果資訊不足，致無法定位，則回傳 None
        if len(distances_to_stations) < 4:
            return None
        
        # 若出現奇怪的數學問題，回傳 None
        try:
            coordinate = gps_solve(distances_to_stations, stations_coordinates)
        except Exception as e:
            print(f"Error locating tag {tag_eui}: {e}")
            return None

        if coordinate is not None:
            self.tags[tag_eui].update_coordinate(coordinate)

        return coordinate

    # 更新 Anchor Sample Rate
    def update_anchor_sample_rate(self, anchor_eui: str, sample_rate: float) -> None:
        self.anchor_sample_rate[anchor_eui] = sample_rate

# 主要的 Node，用於讀取 UWB 設備的資訊、計算 3D 座標後，發布到 ROS Topic
class UWBPublisher(Node):
    serials: list[serial.Serial] = []
    uwb_data_matrix: UWBDataMatrix = None
    state: str = "built_coord_1" 
        # 狀態機的狀態。其他的狀態：get_calib_data_2, get_calib_data_3, self_calibration, flying
    target_state: str = "1" 
        # 狀態機的目標狀態，將透過 Serial 發給 UWB Devices。其他是 "2", "3", "s", "f"，與 state 對應
    anchors: list[UWBDevice] = []
    tags: list[UWBDevice] = []
   
    
    def __init__(self):
        super().__init__('position_publisher')

        dbg("Starting UWB Publisher")

        # 如果要為了 Debug 而儲存資料，則建立資料夾以及檔案
        if SAVE_DATA:
            os.makedirs(DATA_FOLDER, exist_ok=True)

            # 儲存多點定位（multilateration）的結果
            self.multilateration_file = os.path.join(DATA_FOLDER, f"multilateration_results_{time.strftime('%Y%m%d_%H%M%S')}.csv")
            with open(self.multilateration_file, mode='w', newline='') as file:
                csv_writer = csv.writer(file, escapechar='"')
                csv_writer.writerow(["timestamp", "x", "y", "z"]) # 標題

            # 儲存 Serial Read 的結果
            self.serial_read_file = os.path.join(DATA_FOLDER, f"serial_read_{time.strftime('%Y%m%d_%H%M%S')}.csv")
            # dbg("Serial Read file is at", self.serial_read_file)
            with open(self.serial_read_file, mode='w', newline='') as file:
                csv_writer = csv.writer(file, escapechar='"')
                csv_writer.writerow(["timestamp", "portstr", "line"]) # 標題

        dbg("Opening Serial Ports")

        # 開啟 Ports 用以讀取 UWB 設備
        ports = [port.device for port in serial.tools.list_ports.comports() if "ttyACM" in port.device]
        for port in ports:
            serial_connection = serial.Serial(port, baudrate=9600, timeout=0.001)
            self.serials.append(serial_connection)

        dbg("ports are: ", ports)
        
        # 指定 ROS Topic 的傳輸行為與品質（QoS, Quality of Service)
        qos_profile = QoSProfile(
            reliability=QoSReliabilityPolicy.BEST_EFFORT,   # 不保證傳輸成功
            durability=QoSDurabilityPolicy.VOLATILE,        # 不為 Subscriber 保留資料
            history=QoSHistoryPolicy.KEEP_LAST, depth=10    # 只保留最新的 depth (= 10) 筆資料
        )

        # 定義 ROS Topic
        self.tag_position_publisher = self.create_publisher(
            TagPosition,
            '/tag_position',
            qos_profile
        )

        dbg("Generating UWB Devices")

        # 產生 UWB 設備的代表物件
        self.anchors = [UWBDevice(f"00:0{i + 1}") for i in range(8)]    # Anchor "00:0i"，其中 i = 1, 2, ..., 8
        self.tags = [UWBDevice(f"0{i + 1}:0{i + 1}") for i in range(8)]  # Tag "0i:0i"，其中 i = 1, 2, ..., 8
        
        dbg("Starting Self Calibration")

        # 進行 Self Calibration
        self.build_coord()

        dbg("Starting Loops")

        # 用於儲存估計各 tag 位置
        self.uwb_data_matrix = UWBDataMatrix(time_threshold=1.5, anchors=self.anchors, tags=self.tags)

        # 定期透過 USB Serial 讀取 UWB 裝置距離 & 發佈 Tag 的位置
        self.read_serial_loop = self.create_timer(0.05, lambda: self.read_serial(self.uwb_data_matrix))
        self.publish_tag_position_loop = self.create_timer(0.05, self.publish_tag_position)
    
    # 進行 Self Calibration：取得 Calibration Data，建立 Coordinate 並設定 Anchors 座標
    def build_coord(self):
        uwb_calibration_data_matrix = UWBDataMatrix(
            time_threshold=15, 
            anchors=self.anchors[0:4], 
            tags=self.anchors[1:4]     
                # 在 Self Calibration 階段，Anchor 00:02~00:04 都可能暫時作為 Tag
        )

        # 用於判斷 tag-like anchors 到 anchors 之間的資料量已足夠
        def have_enough_data_between(tag_euis: list[str], anchor_euis: list[str]) -> bool:
            dbg("- -", "\n- - ".join(f"from {tag_eui} to {anchor_eui}: {len(uwb_calibration_data_matrix.data[tag_eui][anchor_eui])}" for tag_eui in tag_euis for anchor_eui in anchor_euis))
            return all(
                len(uwb_calibration_data_matrix.data[tag_eui][anchor_eui]) >= 60
                for tag_eui in tag_euis
                for anchor_eui in anchor_euis
            )

        # 等一下用來建立 coordinate 的距離 matrix
        # [i][j] 表示 i & j 之間的距離
        distance_matrix = np.zeros((4, 4))

        # note: 設定 self.target_state 並 broadcast_target_state() 之後，如果成功
        #       read_serial() 會讀到回覆，並且更改 self.state，進而進入下一步

        dbg("- built_coord_1")

        ## get_calib_data_1 階段
        self.state = "built_coord_1"
        self.target_state = "1"
        while self.state == "built_coord_1":
            # dbg("- - broadcasting target state")
            self.broadcast_target_state()
            # dbg("- - reading serial")
            self.read_serial(uwb_calibration_data_matrix)
            if have_enough_data_between(["00:02", "00:03", "00:04"], ["00:01"]):
                self.target_state = "2"
                ## 假定 UWB Anchors 此時不會移動，故預先把距離資料儲存，以免稍後遭清除
                for i in range(1, 4):
                    distance_matrix[0][i] \
                    = distance_matrix[i][0] \
                    = uwb_calibration_data_matrix.get_distance(f"00:0{i + 1}", "00:01")
                break

        while self.state == "built_coord_1":
            self.broadcast_target_state()
            self.read_serial(uwb_calibration_data_matrix)
        
        dbg("- built_coord_2")

        ## get_calib_data_2 階段
        while self.state == "built_coord_2":
            self.broadcast_target_state()
            self.read_serial(uwb_calibration_data_matrix)
            if have_enough_data_between(["00:03", "00:04"], ["00:02"]):
                self.target_state = "3"
                for i in range(2, 4):
                    distance_matrix[1][i] \
                    = distance_matrix[i][1] \
                    = uwb_calibration_data_matrix.get_distance(f"00:0{i + 1}", "00:02")
                break
        
        while self.state == "built_coord_2":
            self.broadcast_target_state()
            self.read_serial(uwb_calibration_data_matrix)
            
        dbg("- built_coord_3")

        ## get_calib_data_3 階段 
        while self.state == "built_coord_3":
            self.broadcast_target_state()
            self.read_serial(uwb_calibration_data_matrix)
            if have_enough_data_between(["00:04"], ["00:03"]):
                # self.target_state = "s"
                for i in range(3, 4):
                    distance_matrix[2][i] \
                    = distance_matrix[i][2] \
                    = uwb_calibration_data_matrix.get_distance(f"00:0{i + 1}", "00:03")
                break
        
        # while self.state == "built_coord_3":
        #     self.broadcast_target_state()
        #     self.read_serial(uwb_calibration_data_matrix)

        dbg("- self_calibration")

        ## self_calibration 階段
        while True: # 重試到 build_3D_coord 成功
            ### 設定 Anchor 00:01~00:04 座標
            dbg("distance_matrix is", distance_matrix)
            anchor_coords = build_3D_coord(distance_matrix)
            if not all(
                all(
                    str(num) not in ["inf", "-inf", "nan"] 
                    for num in anchor_coord
                ) for anchor_coord in anchor_coords
            ):
                # 如果 build_3D_coord 失敗（含有非實數），重試
                dbg("build_3D_coord failed, retrying", anchor_coords)
                self.read_serial(uwb_calibration_data_matrix)
                for i in range(3, 4):
                    distance_matrix[2][i] \
                    = distance_matrix[i][2] \
                    = uwb_calibration_data_matrix.get_distance(f"00:0{i + 1}", "00:03")
                continue
            print("anchor_coords are", anchor_coords)
            for i in range(4):
                self.anchors[i].update_coordinate(anchor_coords[i])
            break
        
        self.target_state = "f"

        while self.state != "flying":
            self.broadcast_target_state()
            self.read_serial(uwb_calibration_data_matrix) 
                # uwb_calibration_data_matrix 不重要，這裡只是等待 self_calibration 結束

        # for i in range(10):
        #     time.sleep(0.1)
        #     self.broadcast_target_state()
        #     self.read_serial(uwb_calibration_data_matrix) 

        ### TODO: 設定 Anchor 00:05~00:08 座標

    def get_eui_from_id(self, id) -> Optional[str]:
        id_to_eui = {
            "10": "00:01",
            "20": "00:02",
            "30": "00:03",
            "40": "00:04",
            "50": "00:05",
            "60": "00:06",
            "70": "00:07",
            "80": "00:08",
            "00": "01:01",
            "11": "01:01",
            "22": "02:02",
            "33": "03:03",
            "44": "04:04",
            "55": "05:05",
            "66": "06:06",
            "77": "07:07",
            "88": "08:08",
        }
        return id_to_eui[id] if id in id_to_eui else None
    
    # 透過 USB Serial 廣播 target_state
    def broadcast_target_state(self) -> None:
        for serial_connection in self.serials:
            try: 
                while serial_connection.write(f"{self.target_state * 10}".encode('utf-8')) <= 0:
                    time.sleep(0.01)
            except Exception as e:
                serial_connection.close()
                print(f"Error sending message to {serial_connection.portstr}: {e}")
                try: 
                    serial_connection.open() 
                except Exception as a:
                    print(f"Failed to reopen the serial port {serial_connection.portstr}") 
        time.sleep(2) # 以免淹沒 Serial Port

    # 透過 USB Serial，讀取並儲存 UWB 裝置距離、採樣率、新狀態遷移
    def read_serial(self, uwb_data_matrix: UWBDataMatrix) -> None:
        for serial_connection in self.serials: # 逐一讀取每個 Serial Port
            # dbg(f"- - - Reading from {serial_connection.portstr}")
            try: 
                lines = [line.decode("utf-8").strip() for line in serial_connection.readlines()]
            except Exception as e:
                print(f"- - - Error reading from {serial_connection.portstr}: {e}")
                continue
            # dbg("- - - Lines are: ", lines)

            for line in lines: # 逐一讀取每一行資料
                if not line:
                    continue
                
                # 儲存資料
                if SAVE_DATA:
                    with open(self.serial_read_file, mode='a', newline='') as file:
                        csv_writer = csv.writer(file, escapechar='"')
                        csv_writer.writerow([time.strftime('%Y%m%d_%H%M%S'), serial_connection.portstr, line]) # 標題

                # 每一行有多種可能的資料：距離資料、採樣率資料、狀態遷移資料
                # - 距離資料 e.g. `Range: 1.23 m     RX power: -45.67 dBm distance between anchor/tag:01:01 from Anchor 00:01`
                # - 採樣率資料 e.g. `Sampling rate of  AnchorA: 10.0Hz     Anchor:00:01`
                # - 狀態遷移資料：太複雜了，請看 `anchor output.txt`
                range_data_match = range_data_regexp.search(line)
                sample_rate_data_match = sample_rate_data_regexp.search(line)
                
                if range_data_match: # 對於距離資料，新增測距結果到 UWB Data Matrix
                    distance, power, from_id, to_eui = range_data_match.groups()
                    
                    from_eui: str = self.get_eui_from_id(from_id) # somehow get the anchor_eui from from_id
                    if from_eui is None:
                        dbg(f"- - - from_id {from_id} is an unknown id")
                        continue

                    uwb_data_matrix.add_measurement(from_eui, to_eui, float(distance))

                elif sample_rate_data_match: # 對於採樣率資料，更新 Anchor 的採樣率到 UWB Data Matrix
                    sample_rate, anchor_eui = sample_rate_data_match.groups()

                    uwb_data_matrix.update_anchor_sample_rate(anchor_eui, float(sample_rate))

                else: # 對於狀態遷移資料，更新狀態機的狀態
                    for state in ["built_coord_1", "built_coord_2", "built_coord_3", "self_calibration", "flying"]:
                        if state in line:
                            self.state = state
                    
                    # dbg(f"- - - Read Serial: {line}")
    
    # 定期發佈 Tag 的位置
    def publish_tag_position(self) -> None:
        for tag_eui in self.uwb_data_matrix.tags.keys():
            coordinate = self.uwb_data_matrix.locate_tag(tag_eui)

            if coordinate is None:
                # dbg(f"- no coordinate for {tag_eui}, skipping")
                # dbg(f"- no coordinate for {tag_eui}, skipping")
                # TODO: 試著用 (速度 * 時間 + 舊位置) 或其他 Sensor 估算
                continue

            msg = TagPosition()
            msg.eui = tag_eui
            msg.x, msg.y, msg.z = coordinate
            msg.timestamp = time.time_ns()
            self.tag_position_publisher.publish(msg)

            dbg(f"- coordinate for {tag_eui} is {coordinate}")

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
