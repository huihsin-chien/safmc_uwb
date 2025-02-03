import serial  # 引用pySerial模組

COM_PORT1 = 'COM7'    # 指定通訊埠名稱
COM_PORT2 = 'COM9'    # 指定通訊埠名稱
COM_PORT3 = 'COM10'    # 指定通訊埠名稱
COM_PORT4 = 'COM11'    # 指定通訊埠名稱
COM_PORT5 = 'COM12'    # 指定通訊埠名稱
COM_PORT6 = 'COM13'    # 指定通訊埠名稱
COM_PORT7 = 'COM14'    # 指定通訊埠名稱
COM_PORT8 = 'COM15'    # 指定通訊埠名稱

BAUD_RATES = 9600    # 設定傳輸速率
ser1 = serial.Serial(COM_PORT1, BAUD_RATES)   # 初始化序列通訊埠
ser2 = serial.Serial(COM_PORT2, BAUD_RATES)   # 初始化序列通訊埠
ser3 = serial.Serial(COM_PORT3, BAUD_RATES)   # 初始化序列通訊埠
ser4 = serial.Serial(COM_PORT4, BAUD_RATES)   # 初始化序列通訊埠
ser5 = serial.Serial(COM_PORT5, BAUD_RATES)   # 初始化序列通訊埠
ser6 = serial.Serial(COM_PORT6, BAUD_RATES)   # 初始化序列通訊埠
ser7 = serial.Serial(COM_PORT7, BAUD_RATES)   # 初始化序列通訊埠
ser8 = serial.Serial(COM_PORT8, BAUD_RATES)   # 初始化序列通訊埠

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
        while ser3.in_waiting:          # 若收到序列資料…
            data_raw = ser3.readline()  # 讀取一行
            data = data_raw.decode()  # 用預設的UTF-8解碼
            print('從',COM_PORT3,'接收到的資料：', data)
        while ser4.in_waiting:          # 若收到序列資料…
            data_raw = ser4.readline()  # 讀取一行
            data = data_raw.decode()  # 用預設的UTF-8解碼
            print('從',COM_PORT4,'接收到的資料：', data)
        while ser5.in_waiting:          # 若收到序列資料…
            data_raw = ser5.readline()  # 讀取一行
            data = data_raw.decode()  # 用預設的UTF-8解碼
            print('從',COM_PORT5,'接收到的資料：', data)
        while ser6.in_waiting:          # 若收到序列資料…
            data_raw = ser6.readline()  # 讀取一行
            data = data_raw.decode()  # 用預設的UTF-8解碼
            print('從',COM_PORT6,'接收到的資料：', data)
        while ser7.in_waiting:          # 若收到序列資料…
            data_raw = ser7.readline()  # 讀取一行
            data = data_raw.decode()  # 用預設的UTF-8解碼
            print('從',COM_PORT7,'接收到的資料：', data)
        while ser8.in_waiting:          # 若收到序列資料…
            data_raw = ser8.readline()  # 讀取一行
            data = data_raw.decode()  # 用預設的UTF-8解碼
            print('從',COM_PORT8,'接收到的資料：', data)

except KeyboardInterrupt:
    ser1.close()    # 清除序列通訊物件
    ser2.close()    # 清除序列通訊物件
    ser3.close()    # 清除序列通訊物件
    ser4.close()    # 清除序列通訊物件
    ser5.close()    # 清除序列通訊物件
    ser6.close()    # 清除序列通訊物件
    ser7.close()    # 清除序列通訊物件
    ser8.close()    # 清除序列通訊物件
    print('再見！')