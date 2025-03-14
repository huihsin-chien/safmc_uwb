import serial
import serial.tools.list_ports
import re
import csv
import time
import os
import threading
import build_3D_coord
from scipy.optimize import minimize
import numpy as np
from datetime import datetime # for timestamp
import copy

# 全域變數
lock = threading.Lock()  # 確保多執行緒存取安全
distance_between_anchors_and_anchors = {
    "AB": list(),
    "AC": list(),
    "AD": list(),
    "BC": list(),
    "BD": list(),
    "CD": list(),
} # key: anchor1, anchor2, value: distance 

clean_distance_between_anchors_and_anchors =  {
    "AB": list(),
    "AC": list(),
    "AD": list(),
    "BC": list(),
    "BD": list(),
    "CD": list(),
} 
ports_list = list(serial.tools.list_ports.comports())
setupcomplete = False

# 清洗distance_between_anchors_and_anchors數據，刪掉離群值
# https://medium.com/@prateekchauhan923/how-to-identify-and-remove-outliers-a-step-by-step-tutorial-with-python-738a103ae666
# 四分位數 Z-score 標準差等 要選哪一種
def clean_distance_between_anchors_and_anchors_data(distance_between_anchors_and_anchors):
    for key in distance_between_anchors_and_anchors:
        clean_distance_between_anchors_and_anchors[key] = quartile_and_average(distance_between_anchors_and_anchors[key])
        
def  quartile_and_average(data): # 四分位數？
    # remove 0 in data
    data = [d for d in data if (d != 0 or d != 0.0)]
    if not data:
        print("No data for this anchor pair.")
        return 0
    q1 = np.percentile(data, 25)
    q3 = np.percentile(data, 75)
    filtered_data = [d for d in data if q1 <= d <= q3]
    if len(filtered_data) == 0:
        print("No data for this anchor pair.")
        return np.mean(data) # todo 要優化算法
    else:
        avg_distance = sum(filtered_data) / len(filtered_data)

    return avg_distance


class stateMachine:
    def __init__(self):
        self.status = "build_coord_1"
    # def update(self):
    #     if self.status == "built_coord_1":
    #         self.status = "built_coord_2"
    #     elif self.status == "built_coord_2":
    #         self.status = "built_coord_3"
    #     elif self.status == "built_coord_3":
    #         self.status = "self_calibration"
    #     elif self.status == "self_calibration":
    #         self.status = "flying"

state_machine = stateMachine()
class Position:
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z

    def __str__(self): 
        return f"({self.x}, {self.y}, {self.z})"

class UWBdata(Position):
    def __init__(self, x, y, z, EUI, name, output_folder):
        super().__init__(x, y, z)
        self.EUI = EUI
        self.name = name
        self.uwb_distance_data = {}  # 存放對應anchor to  tag 的距離數據 (tag_name -> list of (range_m, timestamp))
        self.previous_pooled_data = {}  # 存放上一次的距離數據 key: tag, value: [range_m, timestamp]
        timestamp = datetime.now().strftime("%Y-%m-%d %H-%M-%S.%f")
        self.output_file = os.path.join(output_folder, fr"{self.name}_{timestamp}.csv")#add timestamp

        # 初始化每個 anchor 的 CSV 檔案
        with open(self.output_file, mode='w', newline='') as file:
            csv_writer = csv.writer(file)
            csv_writer.writerow(["timestamp", "tag_name", "range_m", "sample_rate"])
        
    def setXYZ(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z

    def store_uwb_distance_data(self, tag_name, range_m, timestamp):
        """儲存 UWB anchor to tag 距離數據，並移除過期數據"""
        print("self name: ",self.name, "tag_name: ",tag_name, "range_m: ",range_m, "timestamp: ",timestamp)
        if tag_name not in self.uwb_distance_data:
            self.uwb_distance_data[tag_name] = []
            self.previous_pooled_data[tag_name] = [range_m, timestamp]
        
        self.uwb_distance_data[tag_name].append((range_m, timestamp))
        
        # 移除超過 0.5 秒的數據
        self.uwb_distance_data[tag_name] = [
            (r, t) for r, t in self.uwb_distance_data[tag_name] if timestamp - t <= 0.5
        ]
        print(f"Stored data for {tag_name}: {self.uwb_distance_data[tag_name]}")

        # 將數據寫入對應 anchor 的 CSV 檔案
        with open(self.output_file, mode='a', newline='') as file:
            csv_writer = csv.writer(file)
            csv_writer.writerow([time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(timestamp)), tag_name, range_m])

    def get_pooled_distances(self) -> dict:  # key: tag, value: avg_distance
        """回傳平均距離且濾除第一四分位數與第三四分位數以外的數據
        如果 2 秒內，該 tag 沒有新數據，則使用上一次的數據
        如果 2 秒後該 tag 還沒有新數據，則回傳 None
        """
        result = {}
        for tag, data in self.uwb_distance_data.items():
            if data:
                print(f"self: {self.name} Original data for {tag}: {data}, avg: {sum([r for r, _ in data]) / len(data)}")
                distances = [r for r, _ in data]
                q1 = np.percentile(distances, 25)
                q3 = np.percentile(distances, 75)
                filtered_distances = [r for r in distances if q1 <= r <= q3 and r != 0]
                if filtered_distances:
                    avg_distance = sum(filtered_distances) / len(filtered_distances)
                    result[tag] = avg_distance
                    print(f"Filtered data for {tag}, avg: {avg_distance}")
                    self.previous_pooled_data[tag][0] = avg_distance
                    self.previous_pooled_data[tag][1] = datetime.now().timestamp()            
                elif datetime.now().timestamp() - self.previous_pooled_data[tag][1] <= 10:  # 如果 2 秒內沒有新數據，則使用上一次的數據
                    print(f"No data for {tag}, using previous data.")
                    result[tag] = self.previous_pooled_data[tag][0] 
                else:
                    print(f"No data for {tag} for more than 2 seconds.")
                    result[tag] = None          
        return result
    
    def match_serial_data(self,serial_port,line):

        if state_machine.status == "build_coord_1":
            if self.EUI == "00:01":
                data_pattern =  re.compile(r'Range:\s([0-9.]+)\s*m\s+RX power:\s*(-?[0-9.]+)\s*dBm\s*distance between anchor\/tag:\s*([0-9]+)\s*from Anchor\s*([0-9]+):([0-9]+)')
                match = data_pattern.search(line)
                print(f"{serial_port}: {line}")
                if match:
                    range_m = float(match.group(1))
                    power = float(match.group(2))
                    from_address = match.group(3)
                    anchor_key = f"{match.group(4)}:{match.group(5)}"
                    timestamp = time.time()
                    # print("Matched!")
                    print(f"Range: {range_m} m, Power: {power} dBm, From: {from_address}, Anchor: {anchor_key}")
                
                    if from_address == "20":
                        distance_between_anchors_and_anchors["AB"].append(range_m)
                    elif from_address == "30":
                        distance_between_anchors_and_anchors["AC"].append(range_m)
                    elif from_address == "40":
                        distance_between_anchors_and_anchors["AD"].append(range_m)
            else:
                pass

        elif state_machine.status == "built_coord_2":
            if self.EUI == "00:01":
                data_pattern = re.compile(r'Range:\s([0-9.]+)\s*m\s+RX power:\s*(-?[0-9.]+)\s*dBm\s*distance between anchor\/tag:\s*([0-9]+)\s*from Anchor\s*([0-9]+):([0-9]+)')
                match = data_pattern.search(line)
                print(f"{serial_port}: {line}")
                if match:
                    range_m = float(match.group(1))
                    power = float(match.group(2))
                    from_address = match.group(3)
                    anchor_key = f"{match.group(4)}:{match.group(5)}"
                    timestamp = time.time()
                    # print("Matched!")
                    print(f"Range: {range_m} m, Power: {power} dBm, From: {from_address}, Anchor: {anchor_key}")

                    if from_address == "20":
                        distance_between_anchors_and_anchors["AB"].append(range_m)
                    elif from_address == "30":
                        distance_between_anchors_and_anchors["AC"].append(range_m)
                    elif from_address == "40":
                        distance_between_anchors_and_anchors["AD"].append(range_m)

            elif self.EUI == "00:02":
                # todo: store dBC, dBD
                data_pattern = re.compile(r'Range:\s([0-9.]+)\s*m\s+RX power:\s*(-?[0-9.]+)\s*dBm\s*distance between anchor\/tag:\s*([0-9]+)\s*from Anchor\s*([0-9]+):([0-9]+)')
                match = data_pattern.search(line)
                print(f"{serial_port}: {line}")
                if match:
                    range_m = float(match.group(1))
                    power = float(match.group(2))
                    from_address = match.group(3)
                    anchor_key = f"{match.group(4)}:{match.group(5)}"
                    timestamp = time.time()
                    # print("Matched!")
                    print(f"Range: {range_m} m, Power: {power} dBm, From: {from_address}, Anchor: {anchor_key}")

                    if from_address == "30":
                        distance_between_anchors_and_anchors["BC"].append(range_m)
                    elif from_address == "40":
                        distance_between_anchors_and_anchors["BD"].append(range_m)

        elif state_machine.status == "built_coord_3":
            if self.EUI == "00:01":
                data_pattern = re.compile(r'Range:\s([0-9.]+)\s*m\s+RX power:\s*(-?[0-9.]+)\s*dBm\s*distance between anchor\/tag:\s*([0-9]+)\s*from Anchor\s*([0-9]+):([0-9]+)')
                match = data_pattern.search(line)
                print(f"{serial_port}: {line}")
                if match:
                    range_m = float(match.group(1))
                    power = float(match.group(2))
                    from_address = match.group(3)
                    anchor_key = f"{match.group(4)}:{match.group(5)}"
                    timestamp = time.time()
                    # print("Matched!")
                    print(f"Range: {range_m} m, Power: {power} dBm, From: {from_address}, Anchor: {anchor_key}")

                    if from_address == "20":
                        distance_between_anchors_and_anchors["AB"].append(range_m)
                    elif from_address == "30":
                        distance_between_anchors_and_anchors["AC"].append(range_m)
                    elif from_address == "40":
                        distance_between_anchors_and_anchors["AD"].append(range_m)     

            elif self.EUI == "00:02":
                data_pattern = re.compile(r'Range:\s([0-9.]+)\s*m\s+RX power:\s*(-?[0-9.]+)\s*dBm\s*distance between anchor\/tag:\s*([0-9]+)\s*from Anchor\s*([0-9]+):([0-9]+)')
                match = data_pattern.search(line)
                print(f"{serial_port}: {line}")
                if match:
                    range_m = float(match.group(1))
                    power = float(match.group(2))
                    from_address = match.group(3)
                    anchor_key = f"{match.group(4)}:{match.group(5)}"
                    timestamp = time.time()
                    # print("Matched!")
                    print(f"Range: {range_m} m, Power: {power} dBm, From: {from_address}, Anchor: {anchor_key}")

                    if from_address == "30":
                        distance_between_anchors_and_anchors["BC"].append(range_m)
                    elif from_address == "40":
                        distance_between_anchors_and_anchors["BD"].append(range_m)

            elif self.EUI == "00:03":
                # todo: store dCD
                data_pattern = re.compile(r'Range:\s([0-9.]+)\s*m\s+RX power:\s*(-?[0-9.]+)\s*dBm\s*distance between anchor\/tag:\s*([0-9]+)\s*from Anchor\s*([0-9]+):([0-9]+)')
                match = data_pattern.search(line)
                print(f"{serial_port}: {line}")
                if match:
                    range_m = float(match.group(1))
                    power = float(match.group(2))
                    from_address = match.group(3)
                    anchor_key = f"{match.group(4)}:{match.group(5)}"
                    timestamp = time.time()
                    # print("Matched!")
                    print(f"Range: {range_m} m, Power: {power} dBm, From: {from_address}, Anchor: {anchor_key}")

                    if from_address == "40":
                        distance_between_anchors_and_anchors["CD"].append(range_m)

        elif state_machine.status == "self_calibration":
            
            #todo: store dAE, dAF, dAG, dAH, dBE, dBF, dBG, dBH, dCE, dCF, dCG, dCH, dDE, dDF, dDG, DDH

            data_pattern = re.compile(r'Range:\s([0-9.]+)\s*m\s+RX power:\s*(-?[0-9.]+)\s*dBm\s*distance between anchor\/tag:\s*([0-9]+)\s*from Anchor\s*([0-9]+):([0-9]+)')
            match = data_pattern.search(line)
            print(f"{serial_port}: {line}")
            if match:
                range_m = float(match.group(1))
                power = float(match.group(2))
                from_address = match.group(3)
                anchor_key = f"{match.group(4)}:{match.group(5)}"
                timestamp = time.time()
                # print("Matched!")
                print(f"Range: {range_m} m, Power: {power} dBm, From: {from_address}, Anchor: {anchor_key}")
                if self.EUI == "00:01":
                    self_anchor = "A"
                elif self.EUI == "00:02":
                    self_anchor = "B"
                elif self.EUI == "00:03":
                    self_anchor = "C"
                elif self.EUI == "00:04":
                    self_anchor = "D"

                if from_address == "50":
                    distance_between_anchors_and_anchors["AE"].append(range_m)
                elif from_address == "60":
                    distance_between_anchors_and_anchors["AF"].append(range_m)
                elif from_address == "70":
                    distance_between_anchors_and_anchors["AG"].append(range_m)
                elif from_address == "80":
                    distance_between_anchors_and_anchors["AH"].append(range_m)
            
def gps_solve(distances_to_station, stations_coordinates): #https://github.com/glucee/Multilateration/blob/master/Python/example.py
    """多邊定位算法 若有
    :param distances_to_station: dict, key: tag, value: {anchor_EUI: pooled_range}
    :param stations_coordinates: list, list of anchor coordinates
    
    """
    def error(x, c, r):
        return sum([(np.linalg.norm(x - c[i]) - r[i]) ** 2 for i in range(min(len(c), len(r)))])

    # print(f"distances_to_station: ")
    # print("stations_coordinates: ",stations_coordinates)
    l = len(stations_coordinates)
    # print("distances_to_station_type: ",type(distances_to_station))
    S = sum(distances_to_station)

    # compute weight vector for initial guess
    W = []
    if all(S - w != 0 for w in distances_to_station) :
        W = [((l - 1) * S) / (S - w ) for w in distances_to_station]
    else :
        print("Error: Only one distance provided")
    # print(W)
    # get initial guess of point location
    Length = len(W)
    x0 = sum([W[i] * stations_coordinates[i] for i in range(Length)])
    # optimize distance from signal origin to border of spheres
    return minimize(error, x0, args=(stations_coordinates, distances_to_station), method='Nelder-Mead').x

def multilateration(anchor_list, multilateration_file):
    """計算最新的 3D 位置"""
    # print("enter multilateration without lock")
    with lock:  # 確保多執行緒安全存取
        # 取得每個 tag 與每個anchor的距離。假設我想要tag1的位置，我需要anchor1, anchor2, anchor3的距離
        # print("enter multilateration")
        distances = {anchor.EUI: (anchor.get_pooled_distances()) for anchor in anchor_list} #key: anchor, value: {tag: avg_distance}，avg_distance 可能為 None
        tag_distances_to_anchor = {} #key: tag, value: {anchor_EUI: pooled_range}，pooled_range 可能為 None
        for anchor_EUI in distances:
            for tag, pooled_range in distances[anchor_EUI].items():
                if tag not in tag_distances_to_anchor:
                    tag_distances_to_anchor[tag] = {}
                tag_distances_to_anchor[tag][anchor_EUI] = pooled_range 
        print("distances: ",distances)
    
    tag_pos = {}
    copytag_distances_to_anchor = copy.deepcopy(tag_distances_to_anchor)
    print(tag_distances_to_anchor)
    for tag in tag_distances_to_anchor:
        # todo: we need more anchor locations such as anchorEFGH
        anchor_locations = list(np.array([[anchor_list[0].x,anchor_list[0].y,anchor_list[0].z],[anchor_list[1].x, anchor_list[1].y, anchor_list[1].z],[anchor_list[2].x, anchor_list[2].y, anchor_list[2].z],[anchor_list[3].x, anchor_list[3].y, anchor_list[3].z]]))
        for anchor_EUI in tag_distances_to_anchor[tag]:
            if tag_distances_to_anchor[tag][anchor_EUI] == None:
                # anchor_locations without the anchor with None distance
                anchor_locations.remove(anchor_locations[int(anchor_EUI[-1])-1])
                del copytag_distances_to_anchor[tag][anchor_EUI]

        print("tag_distances_to_anchor: ",tag_distances_to_anchor)
        print("list tag_distances_to_anchor: ",tag_distances_to_anchor[tag].values())
        if len(tag_distances_to_anchor[tag]) <= 3:
            print(f"Tag {tag} has less than 3 distances. Skipping multilateration.")
            print("Len(tag_distances_to_anchor[tag]): ",len(tag_distances_to_anchor[tag]))
        else :
            tag_pos[tag] = Position(*gps_solve(list(copytag_distances_to_anchor[tag].values()), anchor_locations))
            print(f"Tag {tag} position: {tag_pos[tag]}")

    # 將 multilateration 結果寫入 CSV 檔案
    with open(multilateration_file, mode='a', newline='') as file:
        csv_writer = csv.writer(file)
        # csv_writer.writerow([time.strftime('%Y-%m-%d %H:%M:%S'), pos.x, pos.y, pos.z])
        for tag, pos in tag_pos.items():
            timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')
            csv_writer.writerow([timestamp, time.localtime(time.time()), tag, pos.x, pos.y, pos.z])
    return tag_pos

def handle_serial_data(serial_port, data_pattern, anchor_list, ser):
    """處理每個 COM 連接，讀取並解析 UWB 數據"""
    
    this_anchor = None
    anchor_find = False
    while True:
        try:
            line = ser.readline().decode('utf-8').strip()
            while not line:
                # print(f"no line {serial_port}")
                line = ser.readline().decode('utf-8').strip()
            # print(f"line {serial_port}: ", line)
            if line:
                global setupcomplete
                setupcomplete = True
                if not anchor_find:
                    if "00:01" in line:
                        anchor_find = True
                        this_anchor = anchor_list[0]
                    elif "00:02" in line:
                        anchor_find = True
                        this_anchor = anchor_list[1]
                    elif "00:03" in line:
                        anchor_find = True
                        this_anchor = anchor_list[2]
                    elif "00:04" in line:
                        anchor_find = True
                        this_anchor = anchor_list[3]



                if anchor_find and state_machine.status != "flying":
                    this_anchor.match_serial_data(serial_port, line)
                elif state_machine.status == "flying" and anchor_find:
                    match = data_pattern.search(line)
                    match_sample = re.search(r'Sampling rate\s*([0-9]+):\s*([0-9.]+)\s*Hz', line) #Sampling rate 1: 4.48 Hz
                    if line != "let's go~":
                        print(f"{serial_port}: {line}")
                    
                    if match:
                        range_m = float(match.group(1))
                        power = float(match.group(2))
                        from_address = match.group(3)
                        anchor_key = f"{match.group(4)}:{match.group(5)}"
                        timestamp = time.time()
                        # print("Matched!")
                        print(f"Range: {range_m} m, Power: {power} dBm, From: {from_address}, Anchor: {anchor_key}")
                    
                        with lock: 
                            # print("Lock acquired.")
                            if not anchor_find:
                                for anchor in anchor_list:
                                    if anchor_key in anchor.EUI:
                                        this_anchor = anchor
                                        print(f"Storing data to {anchor.name}")
                                        anchor.store_uwb_distance_data(from_address, range_m, timestamp)
                            else:
                                print("anchor_find: ",anchor_find)
                                this_anchor.store_uwb_distance_data(from_address, range_m, timestamp)

                        
                    elif match_sample:
                        # print("Matched sample rate!")
                        target_tag = match_sample.group(1)
                        sample_rate = float(match_sample.group(2))

                        with open(this_anchor.output_file, mode='a', newline='') as file:
                            csv_writer = csv.writer(file)
                            timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')  # 獲取包含微秒的時間戳
                            csv_writer.writerow([timestamp, f"Sample rate {target_tag}", None, None, sample_rate])



                #todo: update state according only to anchorA's status? 
                # No, according to the first anchor who changed state
                if "built_coord_2" in line:
                    state_machine.status = "built_coord_2"

                if "built_coord_3" in line:
                    state_machine.status = "built_coord_3"  

                if "self_calibration" in line and this_anchor == anchor_list[0]:
                    state_machine.status = "self_calibration"
                    print(distance_between_anchors_and_anchors)
                    clean_distance_between_anchors_and_anchors_data(distance_between_anchors_and_anchors)
                    print("clean distance: ",clean_distance_between_anchors_and_anchors)
                    X = build_3D_coord.build_3D_coord(clean_distance_between_anchors_and_anchors)
                    anchor_list[0].setXYZ(X[0][0], X[0][1], X[0][2])
                    anchor_list[1].setXYZ(X[1][0], X[1][1], X[1][2])
                    anchor_list[2].setXYZ(X[2][0], X[2][1], X[2][2])
                    anchor_list[3].setXYZ(X[3][0], X[3][1], X[3][2])
                    print((anchor.x, anchor.y, anchor.z) for anchor in anchor_list) 
                    # set anchor's x, y, z

                if "flying" in line:
                    state_machine.status = "flying"

        except KeyboardInterrupt:
            # print(f"Stopped {serial_port}")
            break

    ser.close()


def output_to_serial_ports(selected_ports, message, opened_serial_port):
    """在所有選中的 serial ports 上輸出字串"""
    # print(f"Outputting message to {selected_ports}")
    # print(f"Message: {message}")

    # print(f"Opened serial ports: {[ser.portstr for ser in opened_serial_ports]}")
    if opened_serial_port.portstr in selected_ports:  # 確認端口在選中的列表中
        try:
            # 發送訊息
            opened_serial_port.write(message.encode('utf-8'))
            # print(f"Message s/ent to {opened_serial_ports.portstr}")
        except Exception as e:
            print(f"Error sending message to {opened_serial_port.portstr}: {e}")


def processing_thread(anchor_list, multilateration_file, ser, selected_ports):
    """每 0.1 秒處理一次數據並計算位置"""
    targetstate = '1'
    enterflying = False
    while True:
        print(f"Current state: {state_machine.status}")  # 調試輸出
        global setupcomplete
        if setupcomplete:
            output_to_serial_ports(selected_ports, targetstate, ser)
        print("len(distance_between_anchors_and_anchors[AB]): ",len(distance_between_anchors_and_anchors["AB"]))
        print(f"target state: {targetstate}")
        time.sleep(1)
        if state_machine.status == "flying":
            # time.sleep(0.1) 
            # print("multilateration")
            targetstate = 'f'
            multilateration(anchor_list, multilateration_file)
        elif state_machine.status == "self_calibration" and not enterflying:
            # time.sleep(0.1)
            enterflying = True
            targetstate = 'f'
        elif len(distance_between_anchors_and_anchors["AD"]) >20 and len(distance_between_anchors_and_anchors["BD"]) >20 and len(distance_between_anchors_and_anchors["CD"]) >20 and not enterflying:
            targetstate = 'f'       
        #TODO self_calibration to flying
        elif len(distance_between_anchors_and_anchors["AC"]) >20 and len(distance_between_anchors_and_anchors["BC"]) >20 and not enterflying:
            targetstate = '3'
        elif len(distance_between_anchors_and_anchors["AB"]) >20 and not enterflying:
            targetstate = '2'




 

def main():
    output_folder = r".\output"
    start_main_timestamp = datetime.now().strftime("%Y-%m-%d %H-%M-%S.%f")
    multilateration_file = os.path.join(output_folder, f"multilateration_results_{start_main_timestamp}.csv")

    os.makedirs(output_folder, exist_ok=True)
    
    # 初始化 multilateration CSV 檔案
    with open(multilateration_file, mode='w', newline='') as file:
        csv_writer = csv.writer(file)
        csv_writer.writerow(["timestamp", "Tag name", "x", "y", "z"])

    data_pattern = re.compile(r'Range:\s([0-9.]+)\s*m\s+RX power:\s*(-?[0-9.]+)\s*dBm\s*distance between anchor\/tag:\s*([0-9]+)\s*from Anchor\s*([0-9]+):([0-9]+)')
    # Range: 0.00 m      RX power: -59.80 dBm distance between anchor/tag:30 from Anchor 00:01

    for port in ports_list:
        print(port[0])
    anchor_list = [
        UWBdata(0, 0, 0, "00:01", "AnchorA", output_folder),
        UWBdata(0, 0, 0, "00:02", "AnchorB", output_folder),
        UWBdata(0, 0, 0, "00:03", "AnchorC", output_folder),
        UWBdata(0, 0, 0, "00:04", "AnchorD", output_folder),
    ]

    if not ports_list:
        print("No serial ports found.")
        return
    
    selected_ports = input("Enter COM ports (e.g., COM3,COM4) or 'a' for All: ").split(',')
    if 'a' in selected_ports:
        selected_ports = [comport[0] for comport in ports_list]

    
    # 啟動 serial 讀取執行緒
    threads = []
    print(f"Selected ports: {selected_ports}")
    for port in selected_ports:
        ser = serial.Serial(port, baudrate=9600, timeout=1)
        thread = threading.Thread(target=handle_serial_data, args=(port, data_pattern, anchor_list, ser))
        threads.append(thread)
        thread.start()
        # 啟動處理執行緒
        processing = threading.Thread(target=processing_thread, args=(anchor_list, multilateration_file, ser, selected_ports), daemon=True)
        processing.start()

    for thread in threads:
        thread.join() # join 是等待執行緒結束
        # print("Thread joined.")
    # 將 distance_between_anchors_and_anchors 寫入 CSV 檔案
    with open(os.path.join(output_folder, f"distance_between_anchors_and_anchors_{start_main_timestamp}.csv"), mode='w', newline='') as file:
        csv_writer = csv.writer(file)
        csv_writer.writerow(["AB"])
        for i in distance_between_anchors_and_anchors["AB"]:
            csv_writer.writerow([i])
        csv_writer.writerow(["AC"])
        for i in distance_between_anchors_and_anchors["AC"]:
            csv_writer.writerow([i])
        csv_writer.writerow(["AD"])
        for i in distance_between_anchors_and_anchors["AD"]:
            csv_writer.writerow([i])
        csv_writer.writerow(["BC"])
        for i in distance_between_anchors_and_anchors["BC"]:
            csv_writer.writerow([i])
        csv_writer.writerow(["BD"])
        for i in distance_between_anchors_and_anchors["BD"]:
            csv_writer.writerow([i])
        csv_writer.writerow(["CD"])
        for i in distance_between_anchors_and_anchors["CD"]:
            csv_writer.writerow([i])
    print("clean distance:",clean_distance_between_anchors_and_anchors)
    print("position")
    print((anchor.x, anchor.y, anchor.z) for anchor in anchor_list) 

if __name__ == "__main__":
    main()
