#!/usr/bin/env python3

import rclpy
import time
import json
import os
from rclpy.node import Node
from rclpy.qos import QoSDurabilityPolicy, QoSHistoryPolicy, QoSProfile, QoSReliabilityPolicy
from agent_msgs.msg import TagPosition

def dbg(*args, **kwargs):
    print(*args, **kwargs)

class UWBPublisher(Node):
    def __init__(self):
        super().__init__('uwb_position_publisher')
        
        # Set up QoS profile
        qos_profile = QoSProfile(
            reliability=QoSReliabilityPolicy.BEST_EFFORT,
            durability=QoSDurabilityPolicy.VOLATILE,
            history=QoSHistoryPolicy.KEEP_LAST,
            depth=10
        )
        
        # Create publisher
        self.tag_position_publisher = self.create_publisher(TagPosition, '/tag_position', qos_profile)
        
        # File path for position data from main.py
        self.position_file = '/tmp/uwb_positions.json'
        self.last_modified = 0
        
        # Timer to check for new position data every 100ms
        self.check_timer = self.create_timer(0.01, self.check_and_publish_positions)
        
        dbg("UWB ROS Publisher started, waiting for position data from main.py...")

    def check_and_publish_positions(self):
        """Check for new position data and publish it"""
        try:
            if os.path.exists(self.position_file):
                # Check if file has been modified
                current_modified = os.path.getmtime(self.position_file)
                
                if current_modified > self.last_modified:
                    self.last_modified = current_modified
                    
                    # Read and publish positions
                    with open(self.position_file, 'r') as f:
                        positions = json.load(f)
                        
                    for position_data in positions:
                        self.publish_position(position_data)
                        
        except Exception as e:
            # Silently ignore file read errors (file might be being written)
            pass

    def publish_position(self, position_data):
        """Publish position data to ROS topic"""
        try:
            eui = position_data['eui']
            coordinate = position_data['coordinate']
            
            if coordinate is not None:
                msg = TagPosition()
                msg.eui = eui
                msg.x, msg.y, msg.z = coordinate
                msg.timestamp = int(time.time_ns())
                self.tag_position_publisher.publish(msg)
                
                dbg(f"Published position for {eui}: ({coordinate[0]:.3f}, {coordinate[1]:.3f}, {coordinate[2]:.3f})")
            
        except Exception as e:
            dbg(f"Error publishing position: {e}")

def main(args=None):
    dbg("Starting UWB ROS Publisher...")
    
    rclpy.init(args=args)
    publisher = UWBPublisher()
    
    try:
        rclpy.spin(publisher)
    except KeyboardInterrupt:
        dbg("Keyboard interrupt received, stopping...")
    finally:
        publisher.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()