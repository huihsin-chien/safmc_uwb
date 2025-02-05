import serial
import serial.tools.list_ports
import re
import csv
import time
import os
import threading
import queue

# 全域變數
data_queue = queue.Queue()  # 儲存所有 thread 接收的 UWB 資料
lock = threading.Lock()  # 確保多執行緒存取安全

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
        self.output_file = os.path.join(output_folder, fr"{self.name}_{timestamp}.csv")#add timestamp

        # 初始化每個 anchor 的 CSV 檔案
        with open(self.output_file, mode='w', newline='') as file:
            csv_writer = csv.writer(file)
            csv_writer.writerow(["timestamp", "tag_name", "range_m"])

    def store_pooling_data(self, tag_name, range_m, timestamp):
        """儲存 UWB 距離數據，並移除過期數據"""
        if tag_name not in self.pooling_data:
            self.pooling_data[tag_name] = []
        
        self.pooling_data[tag_name].append((range_m, timestamp))
        
        # 移除超過 0.5 秒的數據
        self.pooling_data[tag_name] = [
            (r, t) for r, t in self.pooling_data[tag_name] if timestamp - t <= 0.5
        ]

        # 將數據寫入對應 anchor 的 CSV 檔案
        with open(self.output_file, mode='a', newline='') as file:
            csv_writer = csv.writer(file)
            csv_writer.writerow([time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(timestamp)), tag_name, range_m])

    def get_pooled_distances(self):
        """回傳平均距離"""
        return {tag: sum(r for r, _ in data) / len(data) for tag, data in self.pooling_data.items() if data}

def multilateration(anchor_list, multilateration_file):
    """計算最新的 3D 位置"""
    with lock:  # 確保多執行緒安全存取
        distances = {anchor.name: anchor.get_pooled_distances() for anchor in anchor_list}

    print("計算位置：", distances)
    
    # 假設只是輸出當前數據，實際應該實作 multilateration 演算法
    pos = Position(0, 0, 0)  # 這裡應該實作真正的 multilateration 計算

    # 將 multilateration 結果寫入 CSV 檔案
    with open(multilateration_file, mode='a', newline='') as file:
        csv_writer = csv.writer(file)
        csv_writer.writerow([time.strftime('%Y-%m-%d %H:%M:%S'), pos.x, pos.y, pos.z])

    return pos

def handle_serial_data(serial_port, data_pattern, anchor_list):
    """處理每個 COM 連接，讀取並解析 UWB 數據"""
    ser = serial.Serial(serial_port, baudrate=9600, timeout=1)
    
    while True:
        try:
            line = ser.readline().decode('utf-8').strip()
            if line:
                match = data_pattern.search(line)
                print(f"{serial_port}: {line}")
                if match:
                    range_m = float(match.group(1))
                    power = float(match.group(2))
                    from_address = match.group(3)
                    anchor_key = f"{match.group(4)}:{match.group(5)}"
                    timestamp = time.time()
                    print(f"Range: {range_m} m, Power: {power} dBm, From: {from_address}, Anchor: {anchor_key}")
                
                    with lock:
                        for anchor in anchor_list:
                            if anchor.EUI == anchor_key:
                                anchor.store_pooling_data(from_address, range_m, timestamp)

                    # 將數據加入 queue
                    data_queue.put((anchor_key, from_address, range_m, timestamp))

        except KeyboardInterrupt:
            print(f"Stopped {serial_port}")
            break

    ser.close()

def processing_thread(anchor_list, multilateration_file):
    """每 0.1 秒處理一次數據並計算位置"""
    while True:
        time.sleep(0.1)  # 讓處理頻率穩定
        multilateration(anchor_list, multilateration_file)

def main():
    output_folder = r"C:\Users\jianh\OneDrive\Desktop\safmc\UWB_test\Serial_testing\readingSerialUwbData\output"
    multilateration_file = os.path.join(output_folder, "multilateration_results.csv")

    # 建立資料夾
    os.makedirs(output_folder, exist_ok=True)
    
    # 初始化 multilateration CSV 檔案
    with open(multilateration_file, mode='w', newline='') as file:
        csv_writer = csv.writer(file)
        csv_writer.writerow(["timestamp", "x", "y", "z"])


    data_pattern = re.compile(r'Range: \s([0-9.]+)\s* m\s+RX power: \s*(-?[0-9.]+)\s* dBm distance between anchor/tag:([0-9]+)\s* from Anchor ([0-9]+):([0-9]+)')
    # Range: 0.00 m      RX power: -59.80 dBm distance between anchor/tag:30 from Anchor 00:01

    ports_list = list(serial.tools.list_ports.comports())
    for port in ports_list:
        print(port[0])
    anchor_list = [
        UWBdata(0, 0, 0, "01:01", "AnchorA", output_folder),
        UWBdata(0, 0, 0, "02:02", "AnchorB", output_folder),
        UWBdata(0, 0, 0, "03:03", "AnchorC", output_folder),
        UWBdata(0, 0, 0, "04:04", "AnchorD", output_folder),
    ]

    if not ports_list:
        print("No serial ports found.")
        return
    
    selected_ports = input("Enter COM ports (e.g., COM3,COM4) or 'a' for All: ").split(',')
    if 'a' in selected_ports:
        selected_ports = [comport[0] for comport in ports_list]

    # 啟動 serial 讀取執行緒
    threads = []
    for port in selected_ports:
        thread = threading.Thread(target=handle_serial_data, args=(port, data_pattern, anchor_list))
        threads.append(thread)
        thread.start()

    # 啟動處理執行緒
    processing = threading.Thread(target=processing_thread, args=(anchor_list, multilateration_file), daemon=True)
    processing.start()

    for thread in threads:
        thread.join() # join 是等待執行緒結束
        print("Thread joined.")

if __name__ == "__main__":
    main()
