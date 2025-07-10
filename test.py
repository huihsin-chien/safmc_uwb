import serial
import time

# 請根據實際情況修改 serial port
SERIAL_PORT = 'COM7'       # Windows 上通常是 COMx；Linux/macOS 上可能是 /dev/ttyUSB0
BAUD_RATE = 9600           # 修正：與 UWB 設備匹配

# 循環指令
commands = ["11", "22", "33", "44", "55", "66", "77", "88", "99", "00"]
command_index = 0

try:
    with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as ser:
        print(f"Connected to {SERIAL_PORT} at {BAUD_RATE} baud.")
        
        # 等待 serial 連接穩定
        time.sleep(2)
        
        while True:
            command = commands[command_index]
            
            # 發送指令
            ser.write(command.encode())
            ser.flush()  # 確保資料立即發送
            
            print(f"Sent: {command}")
            
            # 讀取 mediator 的回應（如果有的話）
            time.sleep(0.1)  # 給 mediator 一點時間回應
            if ser.in_waiting > 0:
                response = ser.readline().decode().strip()
                print(f"Response: {response}")
            
            command_index = (command_index + 1) % len(commands)
            time.sleep(10)  # 等待 10 秒
            
except serial.SerialException as e:
    print(f"Serial error: {e}")
