# for every 0.1 sec, pool the UWB data and calculate the position
# maybe have to change Cartesian coordinate to polar coordinate or sth NED coordinate
# and publish the position to the ROS2 topic
import serial
import serial.tools.list_ports
import re
import csv
import time
import os
import threading

# C++ struct for Position
# typrdef struct Position{
#     double x;
#     double y;
#     double z;
# }Position;

# Python class for Position
class Position:
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z

    def __str__(self):
        return f"({self.x}, {self.y}, {self.z})"

    def __repr__(self):
        return f"Position({self.x}, {self.y}, {self.z})"

    def __add__(self, other):
        return Position(self.x + other.x, self.y + other.y, self.z + other.z)

    def __sub__(self, other):
        return Position(self.x - other.x, self.y - other.y, self.z - other.z)

    def __mul__(self, scalar):
        return Position(self.x * scalar, self.y * scalar, self.z * scalar)

    def __truediv__(self, scalar):
        return Position(self.x / scalar, self.y / scalar, self.z / scalar)

    def __eq__(self, other):
        return self.x == other.x and self.y == other.y and self.z == other.z

    def __ne__(self, other):
        return not self.__eq__(other)

    def __getitem__(self, index):
        if index == 0:
            return self.x
        elif index == 1:
            return self.y
        elif index == 2:
            return self.z
        else:
            raise IndexError("Index out of range")

    def __setitem__(self, index, value):
        if index == 0:
            self.x = value
        elif index == 1:
            self.y = value
        elif index == 2:
            self.z = value
        else:
            raise IndexError("Index out of range")

    def distance(self, other):
        return ((self.x - other.x) ** 2 + (self.y - other.y) ** 2 + (self.z - other.z) ** 2) ** 0.5

class UWBdata(Position):
    def __init__(self, x, y, z, range_m, rx_power_dBm, from_address, sampling_rate, anchor):
        super().__init__(x, y, z)
        self.range_m = range_m
        self.rx_power_dBm = rx_power_dBm
        self.from_address = from_address
        self.sampling_rate = sampling_rate
        self.anchor = anchor

    def __str__(self):
        return f"({self.x}, {self.y}, {self.z}) - Range: {self.range_m} m, RX Power: {self.rx_power_dBm} dBm, From: {self.from_address}, Sampling: {self.sampling_rate} Hz, Anchor: {self.anchor}"

    def __repr__(self):
        return f"UWBdata({self.x}, {self.y}, {self.z}, {self.range_m}, {self.rx_power_dBm}, {self.from_address}, {self.sampling_rate}, {self.anchor})"

    def __eq__(self, other):
        return super().__eq__(other) and self.range_m == other.range_m and self.rx_power_dBm == other.rx_power_dBm and self.from_address == other.from_address and self.sampling_rate == other.sampling_rate and self.anchor == other.anchor

    def __ne__(self, other):
        return not self.__eq__(other)

    def __getitem__(self, index):
        if index == 3:
            return self.range_m
        elif index == 4:
            return self.rx_power_dBm
        elif index == 5:
            return self.from_address
        elif index == 6:
            return self.sampling_rate
        elif index == 7:
            return self.anchor
        else:
            return super().__getitem__(index)

    def __setitem__(self, index, value):
        if index == 3:
            self.range_m = value
        elif index == 4:
            self.rx_power_dBm = value
        elif index == 5:
            self.from_address = value
        elif index == 6:
            self.sampling_rate = value
        elif index == 7:
            self.anchor = value
        else:
            super().__setitem__(index, value)

# create Anchor UWBdata object 
AnchorA = UWBdata(0, 0, 0, 0, 0, 0, 0, 0)

# create Tag UWBdata object
Tag1 = UWBdata(0, 0, 0, 0, 0, 0, 0, 0)

def create_UWBdata(x, y, z, range_m, rx_power_dBm, from_address, sampling_rate, anchor):
    return UWBdata(x, y, z, range_m, rx_power_dBm, from_address, sampling_rate, anchor)

def create_Position(x, y, z):
    return Position(x, y, z)
def handle_output_folder():
    pass

def multilateration():
    pass

def handle_serial_data(serial_port):
    pass