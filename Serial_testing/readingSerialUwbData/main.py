# for every 0.1 sec, pool the UWB data and calculate the position
# maybe have to change Cartesian coordinate to polar coordinate or sth NED coordinate
# and publish the position to the ROS2 topic
import serial
import serial.tools.list_ports
import re
import csv
import time
import os
import threading

# C++ struct for Position
# typrdef struct Position{
#     double x;
#     double y;
#     double z;
# }Position;

# Python class for Position
class Position:
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z

    def __str__(self): 
        return f"({self.x}, {self.y}, {self.z})"

    def __setitem__(self, index, value):
        if index == 0:
            self.x = value
        elif index == 1:
            self.y = value
        elif index == 2:
            self.z = value
        else:
            raise IndexError("Index out of range")
    def __getitem__(self, index):
        if index == 0:
            return self.x
        elif index == 1:
            return self.y
        elif index == 2:
            return self.z
        else:
            raise IndexError("Index out of range")

    def distance(self, other):
        return ((self.x - other.x) ** 2 + (self.y - other.y) ** 2 + (self.z - other.z) ** 2) ** 0.5

class UWBdata(Position):
    def __init__(self, x: float, y:float, z:float, EUI: str, name:str, pooling_data = {"01:01":[], "02:02":[], "03:03":[]}): #pooling_data is a dictionary with key as the tag name and value as the list of distance and timestamp 
        super().__init__(x, y, z)
        self.EUI = EUI
        self.name = name
        self.pooling_data = pooling_data

    def __getitem__(self, index):
        pass
        # if index == 3:
        #     return self.EUI
        # elif index == 4:
        #     return self.name
        # else:
        #     return super().__getitem__(index)

    def __setitem__(self, index, value):
        pass
        # if index == 3:
        #     self.range_m = value
        # elif index == 4:
        #     self.rx_power_dBm = value
        # elif index == 5:
        #     self.from_address = value
        # elif index == 6:
        #     self.sampling_rate = value
        # elif index == 7:
        #     self.anchor = value
        # else:
        #     super().__setitem__(index, value)
    
    def pool_distance(self): 
        pooled_data = {}
        for tag_name in self.pooling_data:
            pooled_data[tag_name] = 0
            for tagData in self.pooling_data[tag_name]:
                pooled_data[tag_name] += tagData[0]
            pooled_data[tag_name] /= len(self.pooling_data[tag_name])
        return pooled_data
        
    def store_pooling_data(self, tag_name, range_m, timeStamp):
        self.pooling_data[tag_name].append([range_m, timeStamp])
        for tagData in self.pooling_data[tag_name]:
            if tagData[1] - timeStamp > 10: # i want to set 0.5 sec as the threshold
                #remove the tag
                self.pooling_data[tag_name].remove(tagData)
            else:
                return


def create_UWBdata(x:float, y:float, z:float, EUI:str, name:str, pooling_data:dict):
    return UWBdata(x, y, z, EUI, name, pooling_data)

def create_Position(x, y, z):
    return Position(x, y, z)

def multilateration():
    pass

def handle_serial_data(serial_port, output_folder, data_pattern, anchor_list):
    ser = serial.Serial(serial_port, baudrate=9600, timeout=1)
    
    # Define the filename and path (with timestamp)
    filename = os.path.join(output_folder, f"uwb_filtered_data_{serial_port}_{time.strftime('%Y%m%d_%H%M%S')}.csv") # f-string is used to format the string
    
    with open(filename, mode='w', newline='') as file:
        csv_writer = csv.writer(file)
        # Write header row for the CSV file
        csv_writer.writerow(["timestamp", "range_m", "rx_power_dBm", "from", "sampling_Hz", "Anchor"])
        
        while True:
            try:
                # Read serial data line-by-line
                line = ser.readline().decode('utf-8').strip()
                if line:
                    # Search for the pattern using the regular expression
                    match = data_pattern.search(line)
                    print(f"[{serial_port}] {line}")
                    if match:
                        # Extract the matched groups
                        range_m = match.group(1)
                        rx_power_dBm = match.group(2)
                        from_address = match.group(3)
                        sampling_rate = match.group(4)
                        anchor_1 = match.group(5)
                        anchor_2 = match.group(6)
                        # Add a timestamp to the data for tracking
                        timestamp = time.strftime('%Y-%m-%d %H:%M:%S')

                        ##################################################################
                            # get data
                            # store data
                            # calculate position(multilateration)

                            # get  data of this specific anchor and put data into the pooling data
                        for anchor in anchor_list:
                            if str(f"{anchor_1}:{anchor_2}") in anchor.EUI:
                                anchor.store_pooling_data(from_address, range_m, timestamp)
                            else:
                                continue
                            pooled_data = anchor.pool_distance()
                            break
                        # calculate position
                        multilateration()
                        
                        ##################################################################
                        # Append the parsed data to the CSV file
                        csv_writer.writerow([timestamp, range_m, rx_power_dBm, from_address, sampling_rate, f"{anchor_1}:{anchor_2}"])

                        # Optionally print the extracted data
                        print(f"[{serial_port}] {timestamp}: Range = {range_m} m, RX Power = {rx_power_dBm} dBm, From = {from_address}, Sampling = {sampling_rate} Hz, Anchor = {anchor_1}:{anchor_2}")
            except KeyboardInterrupt:
                print(f"Stopped by user on {serial_port}")
                break

    ser.close()
    print(f"Data saved to {filename}")


def main():
    output_folder = r"C:\Users\jianh\OneDrive\Desktop\safmc\UWB_test\Serial_testing"  # r for raw string literal
    # Create the folder if it doesn't exist
    os.makedirs(output_folder, exist_ok=True)
    data_pattern = re.compile(r'Range:\s([0-9.]+)\s*m\s*RX power:\s*(-?[0-9.]+)\s*dBm\s*distance from anchor/tag:([0-9]+)\s*Sampling:\s*([0-9.]+)\s*Hz\s*Anchor:([0-9]+)\s*:([0-9]+)')
    
    ports_list = list(serial.tools.list_ports.comports())
    # create a list of UWBdata
    anchor_list = [create_UWBdata(0,0,0,"AA:BB:CC:DD:EE:FF:00:01","AnchorA"), create_UWBdata(0,0,0,"AA:BB:CC:DD:EE:FF:00:02", "AnchorB"),
                    create_UWBdata(0, 0, 0, "AA:BB:CC:DD:EE:FF:00:03", "AnchorC"), create_UWBdata(0, 0, 0, "AA:BB:CC:DD:EE:FF:00:04", "AnchorD")] 

    if len(ports_list) <= 0:
        print("No serial port devices found.")
    else:
        print("Available serial port devices:" )
        for comport in ports_list:
            print(f"{list(comport)[0]} - {list(comport)[1]}")

    selected_ports = input("Please enter the serial ports you want to open (e.g., COM3,COM4), or a for All: ").split(',')
    if 'a' in selected_ports:
        selected_ports = [comport[0] for comport in ports_list]

    # Start a separate thread for each selected COM port
    threads = []
    for port in selected_ports:
        thread = threading.Thread(target=handle_serial_data, args=(port, output_folder, data_pattern, anchor_list))
        threads.append(thread)
        thread.start()

    # Wait for all threads to finish
    for thread in threads:
        thread.join()

    print("All COM port data collection finished.")


    # handle_serial_data
    # get data
    # store data
    # calculate position(multilateration)

if __name__ == "__main__":
    main()
