import serial  # 引用pySerial模組

COM_PORT1 = 'COM7'    # 指定通訊埠名稱
COM_PORT2 = 'COM9'    # 指定通訊埠名稱
BAUD_RATES = 9600    # 設定傳輸速率
ser1 = serial.Serial(COM_PORT1, BAUD_RATES)   # 初始化序列通訊埠
ser2 = serial.Serial(COM_PORT2, BAUD_RATES)   # 初始化序列通訊埠

try:
    print('開始接收序列資料…')
    while True:
        while ser1.in_waiting:          # 若收到序列資料…
            data_raw = ser1.readline()  # 讀取一行
            data = data_raw.decode()   # 用預設的UTF-8解碼
            print('從',COM_PORT1,'接收到的資料：', data)
        while ser2.in_waiting:          # 若收到序列資料…
            data_raw = ser2.readline()  # 讀取一行
            data = data_raw.decode()  # 用預設的UTF-8解碼
            print('從',COM_PORT2,'接收到的資料：', data)

except KeyboardInterrupt:
    ser1.close()    # 清除序列通訊物件
    ser2.close()    # 清除序列通訊物件
    print('再見！')