import serial
import serial.tools.list_ports
import re
import csv
import time
import os
import threading

# Define the folder where you want to store the data
output_folder = r"C:\Users\jianh\OneDrive\Desktop\safmc\UWB_test\Serial_testing"  # r for raw string literal
# Create the folder if it doesn't exist
os.makedirs(output_folder, exist_ok=True)

# Regular expression to match the pattern in the data
data_pattern = re.compile(r'Range:\s([0-9.]+)\s*m\s*RX power:\s*(-?[0-9.]+)\s*dBm\s*distance from anchor/tag:([0-9]+)\s*Sampling:\s*([0-9.]+)\s*Hz\s*Anchor:([0-9]+)\s*:([0-9]+)')

# Function to handle serial reading and file writing for each COM port
def handle_serial_data(serial_port):
    ser = serial.Serial(serial_port, baudrate=9600, timeout=1)
    
    # Define the filename and path (with timestamp)
    filename = os.path.join(output_folder, f"uwb_filtered_data_{serial_port}_{time.strftime('%Y%m%d_%H%M%S')}.csv")
    
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

                        # Append the parsed data to the CSV file
                        csv_writer.writerow([timestamp, range_m, rx_power_dBm, from_address, sampling_rate, f"{anchor_1}:{anchor_2}"])

                        # Optionally print the extracted data
                        print(f"[{serial_port}] {timestamp}: Range = {range_m} m, RX Power = {rx_power_dBm} dBm, From = {from_address}, Sampling = {sampling_rate} Hz, Anchor = {anchor_1}:{anchor_2}")
            except KeyboardInterrupt:
                print(f"Stopped by user on {serial_port}")
                break

    ser.close()
    print(f"Data saved to {filename}")

# List available COM ports
ports_list = list(serial.tools.list_ports.comports())

if len(ports_list) <= 0:
    print("No serial port devices found.")
else:
    print("Available serial port devices:")
    for comport in ports_list:
        print(f"{list(comport)[0]} - {list(comport)[1]}")

# Ask the user to enter the COM ports they want to use (separated by commas)
selected_ports = input("Please enter the serial ports you want to open (e.g., COM3,COM4), or a for All: ").split(',')
if 'a' in selected_ports:
    selected_ports = [comport[0] for comport in ports_list]

# Start a separate thread for each selected COM port
threads = []
for port in selected_ports:
    thread = threading.Thread(target=handle_serial_data, args=(port.strip(),))
    threads.append(thread)
    thread.start()

# Wait for all threads to finish
for thread in threads:
    thread.join()

print("All COM port data collection finished.")