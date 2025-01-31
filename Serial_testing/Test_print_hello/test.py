import serial
import serial.tools.list_ports

ports_list = list(serial.tools.list_ports.comports())
if len(ports_list) <= 0:
    print("无串口设备。")
else:
    print("可用的串口设备如下：")
    for comport in ports_list:
        print(list(comport)[0])
        print(list(comport)[1])

serial_port = input("请输入串口号：")
ser = serial.Serial(serial_port,baudrate= 9600)
if ser.is_open:
    print("串口已打开。")
else:
    print("串口未打开。")

while True:
    try:
        data = ser.readline()
        print(data)
    except KeyboardInterrupt:
        break
ser.close()