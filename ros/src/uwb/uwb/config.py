import os

# 系統配置
SAVE_DATA = False  # 是否保存UWB設備的位置信息用於debug
DATA_FOLDER = os.path.join(os.getcwd(), "output")  # 保存數據的資料夾

# UWB設備配置
NUM_ANCHORS = 8 # Anchor數量
NUM_TAGS = 2   # Tag數量

# 數據處理配置
TIME_THRESHOLD = 0.2  # 數據過時的時間門檻（秒）
CALIBRATION_TIME_THRESHOLD = 180  # 校準時數據過時門檻（秒）
MIN_MEASUREMENTS_FOR_CALIBRATION = 20  # 校準所需最小測量次數
MULTILATERATION_TOLERANCE = 0.0009  # 多點定位容差 (3cm)^2
SELF_CALIBRATION_TOLERANCE = 1e-6  # 自校準時的容差

# 串口配置
SERIAL_BAUDRATE = 9600
SERIAL_TIMEOUT = 0.001
SERIAL_PORT_FILTER = ["COM3", "COM4"]  # 串口過濾器

# 狀態機配置
INITIAL_STATE = "built_coord_1"
INITIAL_TARGET_STATE = "11"

STATE_MAPPING = {
    "built_coord_1": "11",
    "built_coord_2": "22", 
    "built_coord_3": "33",
    "self_calibration": "44",
    "flying": "ff"
}

# Anchor狀態切換符號
TURN_TO_ANCHOR_SYMBOLS = {
    "00:05": "55",
    "00:06": "66",
    "00:07": "77", 
    "00:08": "88"
}

# ROS配置
ROS_TOPIC_NAME = '/tag_position'
ROS_QOS_DEPTH = 10

# 定時器配置
TIMER_UPDATE_SERIAL = 2.0      # 更新串口列表間隔（秒）
TIMER_READ_SERIAL = 0.05       # 讀取串口數據間隔（秒）
TIMER_BROADCAST_STATE = 2.0    # 廣播狀態間隔（秒）
TIMER_PUBLISH_DRONE = 0.0      # 發布無人機位置間隔（秒）
TIMER_PUBLISH_TARGET = 1.0     # 發布目標位置間隔（秒）
TIMER_SELF_CALIBRATION = 1.0   # 自校準間隔（秒）