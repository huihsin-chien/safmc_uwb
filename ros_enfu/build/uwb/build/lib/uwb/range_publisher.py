import rclpy
from rclpy.node import Node
from rclpy.qos import (QoSDurabilityPolicy, QoSHistoryPolicy, QoSProfile,
                       QoSReliabilityPolicy)
import serial
import re

from uwb_msgs.msg import Range


class PositionPublisher(Node):
    def __init__(self):

        super().__init__('position_publisher')

        port = None  # TODO 從 params.yaml 讀取

        self.ser = serial.Serial(port, baudrate=9600, timeout=1)

        # TODO
        qos_profile = QoSProfile(
            reliability=QoSReliabilityPolicy.BEST_EFFORT,
            durability=QoSDurabilityPolicy.TRANSIENT_LOCAL,
            history=QoSHistoryPolicy.KEEP_LAST,
            depth=1
        )

        self.timer = self.create_timer(0.05, self.update)

        self.range_data_pattern = re.compile(
            r'Range:\s(-?\d*\.\d*|-?\binf\b)\sm\t\sRX\spower:\s(-?\d*\.\d*|-?\binf\b)\sdBm\sdistance\sbetween\sanchor\/tag:(\d{2,4})\sfrom\sAnchor\s(\d{2}:\d{2})')
        self.sample_rate_data_pattern = re.compile(
            r'Sampling\srate\sof\s{2}Anchor[A-H]:\s(-?\d*\.\d*|-?\binf\b)Hz\s{4}Anchor:(\d{2}:\d{2})')

        self.publisher = self.create_publisher(
            Range,
            '/range',
            qos_profile
        )

    def update(self):
        # TODO self.ser.reset_input_buffer()
        line = self.ser.readline()
        line = self.ser.readline().decode('utf-8').strip()

        if not line:
            return
        range_data_match = self.range_data_pattern.search(line)
        sample_rate_data_match = self.range_data_pattern.search(line)

        if range_data_match:
            range = float(range_data_match.group(1))  # TODO inf?
            power = float(range_data_match.group(2))  # TODO inf?
            from_id: str = range_data_match.group(3)
            to_id: str = range_data_match.group(4)

            msg = Range()
            msg.range = range
            msg.power = power
            msg.from_id = from_id
            msg.to_id = to_id
            self.publisher.publish(msg)

        if sample_rate_data_match:
            rate: str = float(range_data_match.group(1))  # TODO inf?
            anchor_id: str = range_data_match.group(2)


def main(args=None):

    rclpy.init(args=args)
    position_publisher = PositionPublisher()

    try:
        rclpy.spin(position_publisher)
    finally:
        position_publisher.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
