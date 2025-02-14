import csv
from datetime import datetime
import os
import time
import rclpy
from rclpy.node import Node
from rclpy.qos import (QoSDurabilityPolicy, QoSHistoryPolicy, QoSProfile,
                       QoSReliabilityPolicy)
import serial
import re

from uwb_msgs.msg import Range, Pose
import numpy as np
from scipy.optimize import minimize
class stateMachine:
    def __init__(self):
        self.status = "build_coord_1" #這裡是不是要改成 built_coord_1
    def update(self):
        if self.status == "built_coord_1":
            self.status = "built_coord_2"
        elif self.status == "built_coord_2":
            self.status = "self_calibration"
        elif self.status == "self_calibration":
            self.status = "flying"
            
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
        self.pooling_data = {}  # 存放對應 tag 的距離數據 (tag_name -> list of (range_m, timestamp))
        timestamp = time.strftime('%Y%m%d_%H%M%S')
        self.output_file = os.path.join(output_folder, f"{self.name}_{timestamp}.csv")

        # 初始化每個 anchor 的 CSV 檔案
        with open(self.output_file, mode='w', newline='') as file:
            csv_writer = csv.writer(file)
            csv_writer.writerow(["timestamp", "tag_name", "range_m", "sample_rate"])

    def setXYZ(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z

    def store_pooling_data(self, from_id, range, timestamp):
        """儲存 UWB 距離數據，並移除過期數據"""
        if from_id not in self.pooling_data:
            self.pooling_data[from_id] = []
        
        self.pooling_data[from_id].append((range, timestamp))
        
        # 移除超過 0.5 秒的數據
        self.pooling_data[from_id] = [
            (r, t) for r, t in self.pooling_data[from_id] if timestamp - t <= 0.5
        ]

        # 將數據寫入對應 anchor 的 CSV 檔案
        with open(self.output_file, mode='a', newline='') as file:
            csv_writer = csv.writer(file)
            csv_writer.writerow([time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(timestamp)), from_id, range])

    def get_pooled_distances(self) -> dict:
        """回傳平均距離且濾除第一四分位數與第三四分位數以外的數據"""
        result = {}
        for tag, data in self.pooling_data.items():
            if data:
                print(f"Original data for {tag}: {data}, avg: {sum([r for r, _ in data]) / len(data)}")
                distances = [r for r, _ in data]
                q1 = np.percentile(distances, 25)
                q3 = np.percentile(distances, 75)
                filtered_distances = [r for r in distances if q1 <= r <= q3]
                if filtered_distances:
                    avg_distance = sum(filtered_distances) / len(filtered_distances)
                    result[tag] = avg_distance
                print(f"Filtered data for {tag}: {filtered_distances}, avg: {avg_distance}")
        return result



class UWBPublisher(Node):
    def __init__(self):

        super().__init__('position_publisher')

        ports =[] # TODO 從 params.yaml 讀取 list(serial.tools.list_ports.comports())
        self.sers: list[serial.Serial] = []

        for port in ports:
        
            serial_connection=(serial.Serial(port, baudrate=9600, timeout=1))
            self.sers.append(serial_connection)
            
        # TODO
        qos_profile = QoSProfile(
            reliability=QoSReliabilityPolicy.BEST_EFFORT,
            durability=QoSDurabilityPolicy.TRANSIENT_LOCAL,
            history=QoSHistoryPolicy.KEEP_LAST,
            depth=1
        )
        output_folder = os.path.join(os.getcwd(), "output") 
        os.makedirs(output_folder, exist_ok=True)
        self.multilateration_file = os.path.join(output_folder, f"multilateration_results_{time.strftime('%Y%m%d_%H%M%S')}.csv")
        with open(self.multilateration_file, mode='w', newline='') as file:
            csv_writer = csv.writer(file)
            csv_writer.writerow(["timestamp", "x", "y", "z"])
        
        self.range_data_pattern = re.compile(
            r'Range:\s(-?\d*\.\d*|-?\binf\b)\sm\t\sRX\spower:\s(-?\d*\.\d*|-?\binf\b)\sdBm\sdistance\sbetween\sanchor\/tag:(\d{2,4})\sfrom\sAnchor\s(\d{2}:\d{2})')
        self.sample_rate_data_pattern = re.compile(
            r'Sampling\srate\sof\s{2}Anchor[A-H]:\s(-?\d*\.\d*|-?\binf\b)Hz\s{4}Anchor:(\d{2}:\d{2})')

        self.range_pub = self.create_publisher(
            Range,
            '/range',
            qos_profile
        )
        self.pose_pub = self.create_publisher(
            Pose,
            '/pose',
            qos_profile
        )
            
        self.anchor_list = [
            UWBdata(0, 0, 0, "00:01", "AnchorA", output_folder),
            UWBdata(0, 0, 0, "00:02", "AnchorB", output_folder),
            UWBdata(0, 0, 0, "00:03", "AnchorC", output_folder),
            UWBdata(0, 0, 0, "00:04", "AnchorD", output_folder),
        ]
        
        self.state_machine = stateMachine()
        self.timer1 = self.create_timer(0.05, self.update1)
        self.timer2 = self.create_timer(0.05, self.update2)
        
    def gps_solve(distances_to_station, stations_coordinates): #https://github.com/glucee/Multilateration/blob/master/Python/example.py
        def error(x, c, r):
            return sum([(np.linalg.norm(x - c[i]) - r[i]) ** 2 for i in range(len(c))])

        l = len(stations_coordinates)
        S = sum(distances_to_station)
        # compute weight vector for initial guess
        W = [((l - 1) * S) / (S - w) for w in distances_to_station]
        # get initial guess of point location
        x0 = sum([W[i] * stations_coordinates[i] for i in range(l)])
        # optimize distance from signal origin to border of spheres
        return minimize(error, x0, args=(stations_coordinates, distances_to_station), method='Nelder-Mead').x

    def update1(self):
        # TODO self.ser.reset_input_buffer()
        for ser in self.sers:

            line = ser.readline().decode('utf-8').strip()

            if not line:
                return
            
            range_data_match = self.range_data_pattern.search(line)
            sample_rate_data_match = self.sample_rate_data_pattern.search(line)

            if range_data_match:
                range = float(range_data_match.group(1))  # TODO inf?
                power = float(range_data_match.group(2))  # TODO inf?
                from_id: str = range_data_match.group(3)
                to_id: str = range_data_match.group(4)
                timestamp = time.time()

                msg = Range()
                msg.range = range
                msg.power = power
                msg.from_id = from_id
                msg.to_id = to_id
                self.range_pub.publish(msg)
                
                for anchor in self.anchor_list:
                    if to_id in anchor.EUI:
                        this_anchor = anchor
                        anchor.store_pooling_data(from_id, range, timestamp)

            if sample_rate_data_match:
                rate: str = float(sample_rate_data_match.group(1))  # TODO inf?
                anchor_id: str = sample_rate_data_match.group(2)
                
                
                #this anchor 是 anchor id嗎
                for anchor in self.anchor_list:
                    if anchor_id in anchor.name:
                        this_anchor=anchor 
                if this_anchor:
                    with open(this_anchor.output_file, mode='a', newline='') as file:
                        csv_writer = csv.writer(file)
                        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')  # 獲取包含微秒的時間戳
                        csv_writer.writerow([timestamp, f"Sample rate {anchor_id}", None, None, rate])
                    
            if "built_coord_2" in line:
                self.state_machine.status = "built_coord_2"
                    
            if "self_calibration" in line:
                self.state_machine.status = "self_calibration"

            if "flying" in line:
                self.state_machine.status = "flying"

            # ser.close()
    def update2(self):
        if self.state_machine.status == "flying":
            distances = {anchor.EUI: anchor.get_pooled_distances() for anchor in self.anchor_list} #key: anchor, value: {tag: avg_distance}
            tag_distances_to_anchor = {} #key: tag, value: {anchor: pooled_range}
            for anchor_EUI in distances:
                for tag, pooled_range in distances[anchor_EUI].items():
                    if tag not in tag_distances_to_anchor:
                        tag_distances_to_anchor[tag] = {}
                    tag_distances_to_anchor[tag][anchor_EUI] = pooled_range
                    
                    
        anchor_locations = [[anchor.x, anchor.y, anchor.z] for anchor in self.anchor_list]
        tag_pos = {}
        print(tag_distances_to_anchor)
        for tag in tag_distances_to_anchor:
            tag_pos[tag] = Position(*self.gps_solve(list(tag_distances_to_anchor[tag].values()), anchor_locations))
            print(f"Tag {tag} position: {tag_pos[tag]}")

        # 將 multilateration 結果寫入 CSV 檔案
        with open(self.multilateration_file, mode='a', newline='') as file:
            csv_writer = csv.writer(file)
            # csv_writer.writerow([time.strftime('%Y-%m-%d %H:%M:%S'), pos.x, pos.y, pos.z])
            for tag, pos in tag_pos.items():
                csv_writer.writerow([time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time())), tag, pos.x, pos.y, pos.z])
                msg=Pose()
                msg.pos=pos
                msg.tag=tag
                self.pose_pub.publish(msg)
      

            
   

def main(args=None):

    rclpy.init(args=args)
    position_publisher = UWBPublisher()

    try:
        rclpy.spin(position_publisher)
    finally:
        position_publisher.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
