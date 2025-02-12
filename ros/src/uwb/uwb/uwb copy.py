import csv
import os
import threading
import time

import numpy as np
import rclpy
from rclpy.node import Node

from std_msgs.msg import String
from geometry_msgs.msg import PointStamped

import serial
import serial.tools.list_ports
import re
from scipy.optimize import minimize
from msg_folder.msg import UwbMsg


class UWBNodePublisher(Node):

    def __init__(self):
        super().__init__('uwb_publisher')
        self.publisher_ = self.create_publisher(UwbMsg, 'uwb_topic', 10)
        
        #設定時間
        self.timer = self.create_timer(1.0, self.timer_callback)
        
    
        self.range_value = 0.0
        self.power_value = 0.0
        self.from_address_value = ""
        self.anchor_key_x_value = 0
        self.anchor_key_y_value = 0

    def set_message_values(self, range_value, power_value, from_address_value, anchor_key_x_value, anchor_key_y_value):
     
        

    def timer_callback(self):
  
        
        

def main(args=None):
    rclpy.init(args=args)

    uwbNode_publisher = UWBNodePublisher()
    
    
    rclpy.spin(uwbNode_publisher)
    uwbNode_publisher.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
