'''
serial communication輸入格式:
Anchor1: d1 d2 d3 d4 d5 d6 d7 d8
Anchor2: d1 d2 d3 d4 d5 d6 d7 d8  ...

使用threading模組，每0.1秒計算一次所有tag的位置，若難以debug
，可以改回接收到一定數量的anchor就計算

'''
import serial  # 引用pySerial模組
import numpy as np
import time
import scipy.optimize
import threading

# 設定通訊埠名稱
COM_PORTS = ['COM7', 'COM9', 'COM10', 'COM11', 'COM12', 'COM13', 'COM14', 'COM15']
BAUD_RATES = 9600  # 設定傳輸速率

# 初始化序列通訊埠
serial_ports = [serial.Serial(port, BAUD_RATES) for port in COM_PORTS]

# 設定錨點位置 (x, y)
anchor_positions = [
    (0, 0),  # Anchor1
    (10, 0),  # Anchor2
    (10, 10),  # Anchor3
    (0, 10),  # Anchor4
    (5, 0),  # Anchor5
    (10, 5),  # Anchor6
    (5, 10),  # Anchor7
    (0, 5)   # Anchor8
]

# 初始化二維陣列來儲存距離
distances = np.zeros((8, 8))

def process_serial_data(data):
    """
    Process the serial data and store it in a 2D array.
    
    :param data: String in the format "AnchorX: d1 d2 d3 d4 d5 d6 d7 d8"
    :return: 2D numpy array with distances
    """
    global distances
    # Split the input data by ": " to separate the anchor and distances
    anchor, distances_str = data.split(': ')
    # Extract the anchor number (e.g., "Anchor1" -> 1)
    anchor_index = int(anchor.replace('Anchor', '')) - 1
    # Split the distances string into individual distance values
    distance_values = list(map(float, distances_str.split()))
    # Store the distances in the 2D array
    distances[anchor_index, :] = distance_values

def calculate_position(anchor_positions, ranges):
    """
    Calculate the position using trilateration with least squares method.
    
    :param anchor_positions: List of tuples [(x, y), ...] of the anchor positions
    :param ranges: List of distances from each anchor to the target
    :return: Tuple (x, y) of the calculated position
    """
    # Convert positions and ranges to numpy arrays
    positions = np.array(anchor_positions)
    ranges = np.array(ranges)

    # Initial guess for the target position
    initial_guess = np.mean(positions, axis=0)

    def residuals(target_position):
        return np.sqrt(np.sum((positions - target_position)**2, axis=1)) - ranges

    # Use least squares optimization to find the best target position
    result = scipy.optimize.least_squares(residuals, initial_guess)

    return tuple(result.x)

def calculate_positions():
    """
    Calculate positions of all tags every 0.1 seconds.
    """
    global distances
    for tag_index in range(8):
        tag_ranges = distances[:, tag_index]
        tag_position = calculate_position(anchor_positions, tag_ranges)
        print(f'Tag{tag_index + 1} 的位置：', tag_position)
    
    # 設定下一次計算的位置
    threading.Timer(0.1, calculate_positions).start()

try:
    print('開始接收序列資料…')
    # 開始計算位置的定時器
    calculate_positions()
    
    while True:
        for i, ser in enumerate(serial_ports):
            if ser.in_waiting:  # 若收到序列資料…
                data_raw = ser.readline()  # 讀取一行
                data = data_raw.decode()  # 用預設的UTF-8解碼
                print(f'從 {COM_PORTS[i]} 接收到的資料：', data)
                process_serial_data(data)

except KeyboardInterrupt:
    for ser in serial_ports:
        ser.close()  # 清除序列通訊物件