import serial
import serial.tools.list_ports
import time

current_time = time.time()
ser_list = []

ports_list = list(serial.tools.list_ports.comports())
if len(ports_list) <= 0:
    print("No serial port device.")
else:
    print("Available serial port devices:")
    current_time = time.time()
    for comport in ports_list:
        print(list(comport)[0])
        # print(list(comport)[1])

ser_list = [serial.Serial(list(comport)[0], baudrate=9600) for comport in ports_list]
for ser in ser_list:
    if ser.is_open:
        print("COM opened successfully.")
    else:
        print("COM open failed.")

while True:
    try:
        for ser in ser_list:
            data = ser.readline()
            print(ser.port, ":", data.decode().strip())
    except KeyboardInterrupt:
        print("KeyboardInterrupt")
        for ser in ser_list:
            ser.close()
        break
    
# 不能重複開啟同一個COM，因為會讓 ESP32 程式重啟
# while True:
#     if  time.time() - current_time >= 20: # search for ports every 1 secon
#         for ser in ser_list:        
#             ser.close()
#         ports_list = list(serial.tools.list_ports.comports())
#         if len(ports_list) <= 0:
#             print("No serial port device.")
#             continue
#         else:
#             print("Available serial port devices:")
#             current_time = time.time()
#             for comport in ports_list:
#                 print(list(comport)[0])
#                 # print(list(comport)[1])

#         ser_list = [serial.Serial(list(comport)[0], baudrate=9600) for comport in ports_list]
#         for ser in ser_list:
#             if ser.is_open:
#                 print("COM opened successfully.")
#             else:
#                 print("COM open failed.")
#     else:
#         try:
#             for ser in ser_list:
#                 data = ser.readline()
#                 print(ser.port, ":", data.decode().strip())
#         except KeyboardInterrupt:
#             print("KeyboardInterrupt")
#             for ser in ser_list:
#                 ser.close()
#             break
