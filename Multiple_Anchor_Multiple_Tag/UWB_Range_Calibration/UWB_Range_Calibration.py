import serial
import time
import re
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np
from collections import deque

# Serial port configuration
PORT = 'COM24'  # Change this to your serial port
BAUD_RATE = 9600  # Change this to your baud rate

# Data storage
max_points = 100  # Number of points to display and use for averaging
distances = deque(maxlen=max_points)
timestamps = deque(maxlen=max_points)
avg_window = 10  # Window size for moving average

# Initialize plot
plt.style.use('ggplot')
fig, ax = plt.subplots(figsize=(12, 6))
line, = ax.plot([], [], 'b-', label='Distance')
avg_line, = ax.plot([], [], 'r-', label='Moving Average', linewidth=2)
ax.set_xlabel('Time (s)')
ax.set_ylabel('Distance (m)')
ax.set_title('Real-time Distance Measurements')
ax.grid(True)
ax.legend(loc='upper right')

# Start time for relative timestamps
start_time = time.time()

def extract_distance(data):
    """Extract distance value from serial data."""
    pattern = r"distance between anchor/tag:(\d+)"
    match = re.search(pattern, data)
    if match:
        # Convert from millimeters to meters
        return float(match.group(1)) / 1000.0
    return None

def init():
    """Initialize animation."""
    ax.set_xlim(0, 10)
    ax.set_ylim(0, 5)
    return line, avg_line

def update(frame):
    """Update plot."""
    if not distances:
        return line, avg_line
    
    # Update x-axis limits dynamically
    current_time = timestamps[-1]
    ax.set_xlim(max(0, current_time - 10), current_time + 0.5)
    
    # Update y-axis limits dynamically
    if distances:
        min_dist = min(distances)
        max_dist = max(distances)
        margin = (max_dist - min_dist) * 0.2 or 0.5  # Provide a default margin if values are identical
        ax.set_ylim(max(0, min_dist - margin), max_dist + margin)
    
    # Update lines
    line.set_data(list(timestamps), list(distances))
    
    # Calculate moving average
    if len(distances) >= avg_window:
        avgs = []
        for i in range(len(distances) - avg_window + 1):
            avg = sum(list(distances)[i:i+avg_window]) / avg_window
            avgs.append(avg)
        
        # Pad the beginning to match the timestamps
        padding = [None] * (len(timestamps) - len(avgs))
        avg_line.set_data(
            list(timestamps)[len(padding):], 
            avgs
        )
    else:
        avg_line.set_data([], [])
    
    return line, avg_line

def main():
    try:
        # Open serial port
        ser = serial.Serial(PORT, BAUD_RATE, timeout=1)
        print(f"Connected to {PORT} at {BAUD_RATE} baud")
        
        # Set up animation
        ani = FuncAnimation(fig, update, init_func=init, interval=100, blit=True)
        plt.ion()  # Turn on interactive mode
        plt.show()
        
        while True:
            if ser.in_waiting > 0:
                # Read data from serial port
                data = ser.readline().decode('utf-8').strip()
                print(f"Received: {data}")
                
                # Extract distance
                distance = extract_distance(data)
                if distance is not None:
                    current_time = time.time() - start_time
                    distances.append(distance)
                    timestamps.append(current_time)
                    
                    # Calculate and print moving average
                    if len(distances) >= avg_window:
                        avg = sum(list(distances)[-avg_window:]) / avg_window
                        print(f"Moving Average (last {avg_window} points): {avg:.3f} m")
                    
                    plt.pause(0.01)  # Allow plot to update
    
    except KeyboardInterrupt:
        print("Program stopped by user")
    except serial.SerialException as e:
        print(f"Serial port error: {e}")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
            print("Serial port closed")

if __name__ == "__main__":
    main()