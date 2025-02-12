import rclpy
from rclpy.node import Node
from geometry_msgs.msg import PointStamped
import serial
import serial.tools.list_ports
import re
import csv
import time
import os
import threading
import numpy as np
from scipy.optimize import minimize

class UWBPositionPublisher(Node):
    def __init__(self):
        super().__init__('uwb_position_publisher')
        self.publisher_ = self.create_publisher(PointStamped, 'uwb_positions', 10)
        self.lock = threading.Lock()
        self.anchor_list = self.initialize_anchors()
        self.data_pattern = re.compile(r'Range:\s([0-9.]+)\s*m\s+RX power:\s*(-?[0-9.]+)\s*dBm\s*distance between anchor/tag:\s*([0-9]+)\s*from Anchor\s*([0-9]+):([0-9]+)')
        self.start_serial_threads()
        self.processing_thread = threading.Thread(target=self.process_data, daemon=True)
        self.processing_thread.start()
    
    def initialize_anchors(self):
        return [
            UWBdata(0, 0, 0, "00:01", "AnchorA"),
            UWBdata(0, 0, 0, "00:02", "AnchorB"),
            UWBdata(0, 0, 0, "00:03", "AnchorC"),
            UWBdata(0, 0, 0, "00:04", "AnchorD"),
        ]
    
    def start_serial_threads(self):
        ports_list = [port.device for port in serial.tools.list_ports.comports()]
        self.threads = [threading.Thread(target=self.read_serial, args=(port,)) for port in ports_list]
        for thread in self.threads:
            thread.start()
    
    def read_serial(self, port):
        ser = serial.Serial(port, baudrate=9600, timeout=1)
        while rclpy.ok():
            line = ser.readline().decode('utf-8').strip()
            match = self.data_pattern.search(line)
            if match:
                range_m, from_address, anchor_key = float(match.group(1)), match.group(3), f"{match.group(4)}:{match.group(5)}"
                timestamp = time.time()
                with self.lock:
                    for anchor in self.anchor_list:
                        if anchor_key in anchor.EUI:
                            anchor.store_pooling_data(from_address, range_m, timestamp)
        ser.close()
    
    def process_data(self):
        while rclpy.ok():
            time.sleep(0.1)
            positions = self.compute_multilateration()
            for tag, pos in positions.items():
                msg = PointStamped()
                msg.header.stamp = self.get_clock().now().to_msg()
                msg.point.x, msg.point.y, msg.point.z = pos.x, pos.y, pos.z
                self.publisher_.publish(msg)
    
    def compute_multilateration(self):
        with self.lock:
            distances = {anchor.EUI: anchor.get_pooled_distances() for anchor in self.anchor_list}
            tag_positions = {}
            anchor_coords = np.array([[a.x, a.y, a.z] for a in self.anchor_list])
            for tag, dists in distances.items():
                tag_positions[tag] = Position(*gps_solve(list(dists.values()), anchor_coords))
        return tag_positions

class UWBdata:
    def __init__(self, x, y, z, EUI, name):
        self.x, self.y, self.z, self.EUI, self.name = x, y, z, EUI, name
        self.pooling_data = {}
    
    def store_pooling_data(self, tag_name, range_m, timestamp):
        if tag_name not in self.pooling_data:
            self.pooling_data[tag_name] = []
        self.pooling_data[tag_name].append((range_m, timestamp))
        self.pooling_data[tag_name] = [(r, t) for r, t in self.pooling_data[tag_name] if timestamp - t <= 0.5]
    
    def get_pooled_distances(self):
        return {tag: sum(r for r, _ in data) / len(data) for tag, data in self.pooling_data.items() if data}

class Position:
    def __init__(self, x, y, z):
        self.x, self.y, self.z = x, y, z

def gps_solve(distances, anchors):
    def error(x, c, r):
        return sum([(np.linalg.norm(x - c[i]) - r[i]) ** 2 for i in range(len(c))])
    x0 = sum([((len(anchors) - 1) * sum(distances)) / (sum(distances) - w) * anchors[i] for i, w in enumerate(distances)])
    return minimize(error, x0, args=(anchors, distances), method='Nelder-Mead').x

def main():
    rclpy.init()
    node = UWBPositionPublisher()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
