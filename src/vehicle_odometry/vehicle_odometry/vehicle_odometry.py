import time
import serial
import re
import threading

import rclpy
from rclpy.node import Node

from px4_msgs.msg import VehicleOdometry


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
        pattern = r"Estimated position:\s*\(\s*([-+]?\d*\.?\d+),\s*([-+]?\d*\.?\d+)\s*\)"
        match = re.search(pattern, data_string)
        
        if match:
            x = float(match.group(1))
            y = float(match.group(2))
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
        
        # 其他必要欄位設為 NaN 
        msg.q = [float('nan')] * 4  
        msg.velocity = [float('nan')] * 3  
        msg.angular_velocity = [float('nan')] * 3  
        
        msg.position_variance = [0.1, 0.1, 1.0]  # TODO 先用之前點對點的平均值, 需要再計算定位後的數值
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