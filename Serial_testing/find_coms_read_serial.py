import serial
import serial.tools.list_ports

ports_list = list(serial.tools.list_ports.comports())
if len(ports_list) <= 0:
    print("No serial port device.")
else:
    print("Available serial port devices:")
    for comport in ports_list:
        print(list(comport)[0])
        print(list(comport)[1])

serial_port = input("please enter the serial port wanted to open:")
ser = serial.Serial(serial_port,baudrate= 9600)
if ser.is_open:
    print("COM opened successfully.")
else:
    print("COM open failed.")

while True:
    try:
        data = ser.readline()
        print(data)
    except KeyboardInterrupt:
        break
ser.close()