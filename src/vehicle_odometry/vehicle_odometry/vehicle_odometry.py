import time
import serial
import re
import threading
import pandas as pd
import numpy as np

import rclpy
from rclpy.node import Node

from px4_msgs.msg import VehicleOdometry

# 校正表 (Real Length 與 Std)
calib = pd.DataFrame({
    "Real Length": [0,3,5,6,9,10,12,15,18,20,21,24,25,27,30,33,35,36,39,40,42,45,48,50,51,54,55,57,60,63,65],
    "Std": [0.148242,0.043708,0.078889,0.030269,0.068376,0.075397,0.053864,0.198789,0.084648,0.065091,0.123263,
            0.044865,0.05268,0.040793,0.036058,0.284102,0.054037,0.816694,1.557951,0.070185,0.196966,0.040315,
            0.030639,0.661929,0.203289,1.444896,0.191301,0.032451,0.597958,0.049518,0.089583]
})

def var_calculation(p_hat, anchors, calib):
    # ========== 計算每個 anchor 幾何距離 ==========
    diff = p_hat - anchors  # N x 2
    d_geom = np.linalg.norm(diff, axis=1)  # N

    # ========== 插值標準差 ==========
    calib_d = calib["Real Length"].values
    calib_std = calib["Std"].values

    # 線性插值，超出範圍用邊界值
    std_interp = np.interp(d_geom, calib_d, calib_std,
                        left=calib_std[0], right=calib_std[-1])

    # 設定最小值，避免太小導致數值爆炸
    sigma_floor = 0.02
    std_interp = np.maximum(std_interp, sigma_floor)

    # ========== 幾何矩陣 H ==========
    H = (p_hat - anchors) / d_geom[:, None]  # N x 2

    # 權重矩陣 W
    W = np.diag(1.0 / (std_interp**2))

    # 協方差 Σ_p = inv(H^T W H)
    HTWH = H.T @ W @ H
    Sigma_p = np.linalg.inv(HTWH)


    # ========== 結果 ==========
    sigma_x = np.sqrt(Sigma_p[0, 0])
    sigma_y = np.sqrt(Sigma_p[1, 1])
    cov_xy  = Sigma_p[0, 1]
    var_x = Sigma_p[0, 0]
    var_y = Sigma_p[1, 1]

    return var_x, var_y
    # print("位置協方差矩陣 Σ_p =\n", Sigma_p)
    # print(f"σ_x = {sigma_x:.4f} m, σ_y = {sigma_y:.4f} m, Cov_xy = {cov_xy:.4f}")


class VehicleVisualOdometry(Node):
    def __init__(self):
        super().__init__("vehicle_visual_odometry")

        # Publisher
        self.publisher_ = self.create_publisher(
            VehicleOdometry, f"/px4/fmu/in/vehicle_visual_odometry", 10
        )
        
        # Serial port configuration 
        self.serial_port = "/dev/ttyACM0"  # TODO 需要再調整
        self.baud_rate = 9600  
        self.serial_conn = None
        
        # 與各定位點的距離，用於計算 position variance
        self.range101 = 4
        self.range202 = 4        
        self.range303 = 4
        self.anchors_pos = np.array([[0.0, 0.0],
                    [8.0, 0.0],
                    [4.0, 4.0]])

        # 初始化 serial 連接
        self.init_serial()
        
        # 啟動 serial 讀取執行緒
        self.serial_thread = threading.Thread(target=self.serial_read_loop, daemon=True)
        self.serial_thread.start()
        
        self.timestamp = int(time.time() * 1e6)
        
        self.get_logger().info("Vehicle Visual Odometry Node initialized")

    def init_serial(self):
        try:
            self.serial_conn = serial.Serial(
                port=self.serial_port,
                baudrate=self.baud_rate,
                timeout=1,
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE
            )
            self.get_logger().info(f"Serial port {self.serial_port} opened successfully")
        except serial.SerialException as e:
            self.get_logger().error(f"Failed to open serial port {self.serial_port}: {e}")
            self.serial_conn = None

    def serial_read_loop(self):
 
        while rclpy.ok():
            try:
                if self.serial_conn and self.serial_conn.is_open:
                   
                    line = self.serial_conn.readline().decode('utf-8').strip()
                    if line:
                        self.get_logger().debug(f"received: {line}")
                        self.parse_and_publish_position(line)
                else:
                    self.get_logger().warn("reconnecting")
                    self.init_serial()
                    time.sleep(1)
            except Exception as e:
                self.get_logger().error(f"Serial read error: {e}")
                time.sleep(0.1)

    def parse_and_publish_position(self, data_line):
        
        try:
            # 解析格式: "x: 1.2, y: 4.2"
            position = self.parse_position_string(data_line)
            if position:
                self.__set_vehicle_odometry(position['x'], position['y'], float('nan')) # z will be fuse to px4 through another interface of optical flow
        except Exception as e:
            self.get_logger().error(f"Error parsing position data: {e}")

    def parse_position_string(self, data_string):
        """解析位置字串"""
        # regx
        position_pattern = r"Estimated position:\s*\(\s*([-+]?\d*\.?\d+),\s*([-+]?\d*\.?\d+)\s*\)"
        position_match = re.search(position_pattern, data_string)

        avg_distance_pattern = r'Average range for tag (\d+): ([0-9.]+) m'
        avg_distance_match = re.search(avg_distance_pattern, data_string)
        
        if avg_distance_match:
            # tag_id = int(avg_distance_match.group(1))
            # avg_distance = float(avg_distance_match.group(2))
            # self.get_logger().info(f"Tag {tag_id} average distance: {avg_distance} m")
            if avg_distance_match.group(1) == '101':
                self.range101 = float(avg_distance_match.group(2))
            elif avg_distance_match.group(1) == '202':
                self.range102 = float(avg_distance_match.group(2))
            elif avg_distance_match.group(1) == '303':
                self.range103 = float(avg_distance_match.group(2))

        if position_match:
            x = float(position_match.group(1))
            y = float(position_match.group(2))
            return {'x': x, 'y': y}
    
        
        self.get_logger().warn(f"Failed to parse position from: {data_string}")
        return None


    def __set_vehicle_odometry(self, x, y, z):
       
        global_position = [x, y, z]
        self.publish_odometry(global_position)

    def publish_odometry(self, global_position):
        
        msg = VehicleOdometry()
        msg.timestamp = int(time.time() * 1e6)
        msg.timestamp_sample = msg.timestamp
        
        
        msg.pose_frame = VehicleOdometry.POSE_FRAME_NED
        msg.velocity_frame = VehicleOdometry.VELOCITY_FRAME_NED
        
        
        msg.position = [
            global_position[1],  # North (Y)
            global_position[0],  # East (X) 
            global_position[2]   # Down (Z)
        ]

        phat = np.array([global_position[0], global_position[1]])
        var_x, var_y = var_calculation(phat, self.anchors_pos, calib) # TODO var_calculation是100% vibe coding, 需要確認真實性

        # 其他必要欄位設為 NaN
        msg.q = [float('nan')] * 4
        msg.velocity = [float('nan')] * 3  
        msg.angular_velocity = [float('nan')] * 3  
        
        # msg.position_variance = [0.1, 0.1, 1.0]  # TODO 先用之前點對點的平均值, 需要再計算定位後的數值
        msg.position_variance = [var_y, var_x, float('nan')] # TODO 確認是 xyz or yxz
        msg.orientation_variance = [float('nan')] * 3
        msg.velocity_variance = [float('nan')] * 3
        
        # 發布消息
        self.publisher_.publish(msg)
        self.get_logger().info(
            f"Published visual odometry: x={msg.position[0]:.3f}, y={msg.position[1]:.3f}, z={msg.position[2]}"
        )

    def __del__(self):
        
        if self.serial_conn and self.serial_conn.is_open:
            self.serial_conn.close()


def main(args=None):
    rclpy.init(args=args)
    node = VehicleVisualOdometry()
    
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()