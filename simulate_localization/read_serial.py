import serial
import re
import csv
from datetime import datetime

# 1. 設定 Serial Port 參數
PORT = 'COM15'          # 修改成你的 Serial port，例如：'/dev/ttyUSB0'（Linux/macOS）
BAUDRATE = 9600
TIMEOUT = 1            # Timeout in seconds

# 2. 正則表達式
pattern = re.compile(r'anchor_range,([0-9.]+),([0-9a-fA-F:]+),([0-9a-fA-F:]+)')
pattern_avg = re.compile(r'Average range for tag (\d+): ([0-9.]+) m')
pattern_est = re.compile(r'Estimated position: \(([-0-9.]+),\s*([-0-9.]+)\)')

# 3. 檔案命名（由使用者輸入前綴，後面接時間）
user_prefix = input("請輸入檔案名稱前綴(x, y)：")
timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
filename = f"serial_data_{user_prefix}_{timestamp}.csv"

# 4. 開啟 Serial Port
with serial.Serial(PORT, BAUDRATE, timeout=TIMEOUT) as ser, open(filename, 'w', newline='') as csvfile:
    csv_writer = csv.writer(csvfile)
    csv_writer.writerow(['Timestamp', 'Distance (m)', 'Tag EUI', 'Anchor EUI', 'Est_X', 'Est_Y'])  # 新增 Est_X, Est_Y

    print(f"Recording started. Saving to {filename} ...\nPress Ctrl+C to stop.\n")

    try:
        while True:
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            match = pattern.search(line)
            match_avg = pattern_avg.search(line)
            match_est = pattern_est.search(line)
            if match:
                distance = float(match.group(1))
                tag_eui = match.group(2)
                anchor_eui = match.group(3)
                time_str = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                csv_writer.writerow([time_str, distance, tag_eui, anchor_eui, '', ''])
                print(f"[{time_str}] {distance} m, Tag {tag_eui}, Anchor {anchor_eui}")
            elif match_avg:
                tag_id = match_avg.group(1)
                distance = float(match_avg.group(2))
                time_str = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                csv_writer.writerow([time_str, distance, tag_id, '', '', ''])
                print(f"[{time_str}] {distance} m, Tag {tag_id}, Anchor N/A")
            elif match_est:
                est_x = float(match_est.group(1))
                est_y = float(match_est.group(2))
                time_str = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                csv_writer.writerow([time_str, '', '', '', est_x, est_y])
                print(f"[{time_str}] Estimated position: ({est_x}, {est_y})")
            else:
                print(f"Unmatched line: {line}")
    except KeyboardInterrupt:
        print("\nRecording stopped.")