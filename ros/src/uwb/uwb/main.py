import json
import math
import numpy as np
import time
import threading
import serial
import signal
import sys
from typing import Callable, Optional, Tuple
from data_structure import UWBDataMatrix, UWBDevice, UWBData
from data_manager import DataManager, UWBDataManager
from config import SAVE_DATA, DATA_FOLDER, TURN_TO_ANCHOR_SYMBOLS, NUM_TAGS, NUM_ANCHORS
from serial_manager import SerialManager, UWBSerialDataProcessor
from algorithms import build_3D_coord


def dbg(*args, **kwargs):
    print(*args, **kwargs)


class Timer:
    
    def __init__(self, interval: float, callback: Callable):
        self.interval = interval
        self.callback = callback
        self.timer = None
        self.is_running = False
        
    def start(self):
        if not self.is_running:
            self.is_running = True
            self._run()
    
    def stop(self):
        self.is_running = False
        if self.timer:
            self.timer.cancel()
    
    def _run(self):
        if self.is_running:
            try:
                self.callback()
            except Exception as e:
                dbg(f"Timer callback error: {e}")
            
            
            self.timer = threading.Timer(self.interval, self._run)
            self.timer.start()


class runner():
    serials: list[serial.Serial] = []
    uwb_data_matrix: UWBDataMatrix = None
    state: str = "built_coord_1" 
        # 狀態機的狀態。其他的狀態：build_coord_2, build_coord_3, self_calibration, flying
    target_state: str = "11" 
        # 狀態機的目標狀態，將透過 Serial 發給 UWB Devices。
        # 為減少突波的影響，UWBDevice_Rewrite 要讀取連續兩個相同的 char，故我們要連續發送
        # 其他值可能是 "22", "33", "44", "55", "66", "77", "88", "ff" 等，對應的 state 為
        # - build_coord_1~3 跟 "11"~"33" 對應。
        # - self_calibration 對應多個 target_state。變化規則是：
        #   - 先進入 "44"
        #   - 當 00:05~00:08 任一進入其 anchor_state 後，由 "55", "66", "77", "88" 的各種組合（如 "5588", "886677"）來通知 anchor 切換 state
    anchors: list[UWBDevice] = []
    tags: list[UWBDevice] = []
    
    # Add running flag to control the system
    running: bool = False
    
    def __init__(self):
        super().__init__()

        self.data_manager = DataManager(save_data=SAVE_DATA)
        
        self.anchors = [UWBDevice(f"00:{i + 1:02}") for i in range(NUM_ANCHORS)]
        self.tags = [UWBDevice(f"{i + 1:02}:{i + 1:02}") for i in range(NUM_TAGS)]
        print("1")
        self.uwb_data_matrix = UWBDataMatrix(time_threshold=0.2, anchors=self.anchors, tags=self.tags)
        self.uwb_calibration_data_matrix = UWBDataMatrix(time_threshold=180, anchors=self.anchors, tags=self.anchors[1:NUM_ANCHORS])
        print("2")
        self.uwb_data_manager = UWBDataManager(self.uwb_data_matrix, self.uwb_calibration_data_matrix,self.data_manager)
        print ("3")
        self.serial_manager = SerialManager(data_manager=self.data_manager)
        self.calib_serial_manager = self.serial_manager
        
        
        
        self.serial_data_processor = UWBSerialDataProcessor(self.uwb_data_manager, calibration=False)
        self.calibration_processor = UWBSerialDataProcessor(self.uwb_data_manager, calibration=True)

        self.serial_data_processor.set_state_callback(self._handle_state_change)
        
        print("starting to build coordinates...")
        
        self.build_coord()
        
        print ("4")
  
        self.read_serial_loop1 = Timer(0.05, lambda: self.serial_manager.read_serial(self.serial_data_processor))
        self.read_serial_loop2 = Timer(0.05, lambda: self.serial_manager.read_serial(self.calibration_processor))
        self.broadcast_target_state_loop = Timer(2.0, lambda: self.serial_manager.broadcast_target_state(self.target_state))
        drone_tag_euis = [tag.eui for tag in self.tags[0:4]]
        target_tag_euis = [tag.eui for tag in self.tags[4:]]
        self.publish_drone_tag_position_loop = Timer(0.1, lambda: self.publish_tag_position(drone_tag_euis))  # Changed from 0 to 0.1
        self.publish_target_tag_position_loop = Timer(0.1, lambda: self.publish_tag_position(target_tag_euis, force_output=True))
        self.self_calibration_loop = Timer(1.0, lambda: self.self_calibration_handler())
        
        self.position_file = './uwb_positions.json'
        self.position_buffer = []
        
        self.timers = [
            self.read_serial_loop1,
            self.read_serial_loop2,
            self.broadcast_target_state_loop,
            self.publish_drone_tag_position_loop,
            self.publish_target_tag_position_loop,
            self.self_calibration_loop
        ]
        
        
        self.is_in_anchor_state = {
            "00:05": False,
            "00:06": False,
            "00:07": False,
            "00:08": False
        }
    
    def create_timer(self, interval: float, callback: Callable) -> Timer:
        return Timer(interval, callback)
    
    def start_system(self):
        
        self.running = True
        
        for timer in self.timers:
            timer.start()
        
        
    def _handle_state_change(self, new_state: str):
        """Handle state changes from serial data"""
        
        self.state = new_state
        print(f"State changed to: {new_state}")
    
    def stop_system(self):
       
        self.running = False
        
        for timer in self.timers:
            timer.stop()
        
        dbg("UWB system stopped")
        
    def save_positions_to_file(self):
        """Save current positions to file for ROS publisher"""
        if self.position_buffer:
            try:
                with open(self.position_file, 'w') as f:
                    json.dump(self.position_buffer, f)
                self.position_buffer = []  # Clear buffer after saving
            except Exception as e:
                dbg(f"Error saving positions to file: {e}")
                
    
    def publish_tag_position(self, tag_euis: list[str], force_output: bool = False):
        
    
        
        # This would typically publish to ROS topics or other systems
        for eui in tag_euis:
        
            try:
                
                position = self.uwb_data_matrix.locate_tag(eui)
            
                if position is not None or force_output:
                    print("(", position[0], ",", position[1], ")")
                    self.position_buffer.append({
                        'eui': eui,
                        'coordinate': position.tolist() if isinstance(position, np.ndarray) else position,
                        'timestamp': time.time()
                    })
            except Exception as e:
                if force_output:
                    dbg(f"Error getting position for tag {eui}: {e}")
            
            self.save_positions_to_file()        
    
    def self_calibration_handler(self):
        """Wrapper for self_calibration to handle it in timer context"""
        if self.state != "self_calibration":
            return
        
        try:
            self.self_calibration(self.uwb_data_matrix)
        except Exception as e:
            dbg(f"Self calibration error: {e}")
    
    def broadcast_target_state(self):
        """Broadcast target state to all devices"""
        
        self.serial_manager.broadcast_target_state(self.target_state)
   
    # 進行 Self Calibration：取得 Calibration Data，建立 Coordinate 並設定 Anchors 座標
    
    def have_enough_data_between(self, tag_euis: list[str], anchor_euis: list[str]) -> bool:
        # uwb_calibration_data_matrix.clear_outdated_measurements(tag_euis[0], anchor_euis[0])
        dbg("- -", "\n- - ".join(
            f"from {tag_eui} to {anchor_eui}: {len(self.uwb_calibration_data_matrix.data[tag_eui][anchor_eui])}" 
            for tag_eui in tag_euis
            for anchor_eui in anchor_euis
        ))
        return all(
            len(self.uwb_calibration_data_matrix.data[tag_eui][anchor_eui]) >= 20
            for tag_eui in tag_euis
            for anchor_eui in anchor_euis
        )
    
    def build_coord(self):
        """Build coordinate system for anchors 1-4"""

        # 用於判斷 tag-like anchors 到 anchors 之間的資料量已足夠
        # 等一下用來建立 coordinate 的距離 matrix
        # [i][j] 表示 i & j 之間的距離
        distance_matrix = np.zeros((4, 4))

        # note: 設定 self.target_state 並 broadcast_target_state() 之後，如果成功
        #       read_serial() 會讀到回覆，並且更改 self.state，進而進入下一步

        dbg("- built_coord_1")

        ## get_calib_data_1 階段
        self.state = "built_coord_1"
        self.target_state = "11"
        
        count = 0
        
        while self.state == "built_coord_1": #現在有奇怪原因一直重複所以加上3的限制
            count += 1
            dbg("- built_coord_1")
            # dbg("- - broadcasting target state")
            self.broadcast_target_state()
            # dbg("- - reading serial")
            self.serial_manager.read_serial(self.calibration_processor)

            if self.have_enough_data_between(["00:02", "00:03", "00:04"], ["00:01"]):
                self.state = "built_coord_2"
                self.target_state = "22"
                ## 假定 UWB Anchors 此時不會移動，故預先把距離資料儲存，以免稍後遭清除
                for i in range(1, 4):
                    distance_matrix[0][i] \
                    = distance_matrix[i][0] \
                    = self.uwb_calibration_data_matrix.get_distance(f"00:0{i + 1}", "00:01")
                break
            time.sleep(0.1) 
            dbg("end of built_coord_1 loop, count:", count) 
        
        dbg("- built_coord_2")

        ## get_calib_data_2 階段
        while self.state == "built_coord_2":
            dbg("- built_coord_2")
            self.broadcast_target_state()
            self.serial_manager.read_serial(self.calibration_processor)
            if self.have_enough_data_between(["00:03", "00:04"], ["00:02"]):
                self.state = "built_coord_3"
                self.target_state = "33"
                for i in range(2, 4):
                    distance_matrix[1][i] \
                    = distance_matrix[i][1] \
                    = self.uwb_calibration_data_matrix.get_distance(f"00:0{i + 1}", "00:02")
                break
            time.sleep(0.1)
        
        dbg("- built_coord_3")

        ## get_calib_data_3 階段 
        while self.state == "built_coord_3" :
            dbg("- built_coord_3")

            self.broadcast_target_state()
            self.serial_manager.read_serial(self.calibration_processor)
            if self.have_enough_data_between(["00:04"], ["00:03"]):
                # self.state = "self_calibration"
                # self.target_state = "s"         # don't change state yet
                for i in range(3, 4):
                    distance_matrix[2][i] \
                    = distance_matrix[i][2] \
                    = self.uwb_calibration_data_matrix.get_distance(f"00:0{i + 1}", "00:03")
                break
            time.sleep(0.1)


        ## 重試到 build_3D_coord 成功
        retry_count = 0
        max_retries = 100
        
        while retry_count < max_retries:
            # 設定 Anchor 00:01~00:04 座標
            dbg("distance_matrix is", distance_matrix)
            anchor_coords = build_3D_coord(distance_matrix)
            
            if not all(anchor_coord is not None for anchor_coord in anchor_coords) \
            or not all(
                all(
                    not math.isinf(num) and not math.isnan(num)
                    for num in anchor_coord
                ) for anchor_coord in anchor_coords
            ):
                # 如果 build_3D_coord 失敗（結果含有非實數），獲取新資料重試
                dbg(f"build_3D_coord failed, retrying ({retry_count + 1}/{max_retries})", anchor_coords)
                retry_count += 1
                
                self.calib_serial_manager.read_serial(self.calibration_processor)

                for i in range(3, 4):
                    distance_matrix[2][i] \
                    = distance_matrix[i][2] \
                    = self.uwb_calibration_data_matrix.get_distance(f"00:0{i + 1}", "00:03")
                    
                time.sleep(0.1)
                continue
                
            # Success!
            print("anchor_coords are", anchor_coords)
            for i in range(4):
                self.anchors[i].update_coordinate(anchor_coords[i])
            break
        
        if retry_count >= max_retries:
            dbg("Failed to build coordinates after maximum retries")
            self.stop_system()
            return

        # Transition to self calibration
        self.target_state = "44"
        self.state = "self_calibration"
        
        
        

    def self_calibration(self, uwb_data_matrix) -> None:
        
        if self.state != "self_calibration":
            return
            
        # 在完成前四個anchor定位後, 定位後四個EFHG
        ## self_calibration 階段

        # 重試直到 Anchor 5~8 都從 Tag State 進入 Anchor State
        for eui in self.is_in_anchor_state.keys():
            if self.is_in_anchor_state[eui]:
                continue
                
            self.serial_manager.read_serial(self.calibration_processor)

            # Check if we have enough data from this anchor to established anchors
            established_anchors = [anchor for anchor in self.anchors[:4] if anchor.coordinate is not None]
            established_anchor_euis = [anchor.eui for anchor in established_anchors]
            
            if len(established_anchors) >= 4:
                # Check if we have enough calibration data
                enough_data = sum(1 for anchor_eui in established_anchor_euis 
                                if len(self.uwb_calibration_data_matrix.data[eui][anchor_eui]) >= 10) >= 4
                
                if enough_data:
                    # Use calibration data to locate this anchor
                    coord = self.locate_anchor_using_calibration_data(eui, established_anchors)
                    
                    print(f"locate_anchor({eui}) returns: {coord}")
                    if coord is not None and all(not (math.isnan(x) or math.isinf(x)) for x in coord):
                        # Find the anchor index and update its coordinate
                        anchor_index = int(eui.split(':')[1]) - 1
                        if 4 <= anchor_index < NUM_ANCHORS:
                            self.anchors[anchor_index].update_coordinate(coord)
                            
                            # Update target state
                            self.target_state += TURN_TO_ANCHOR_SYMBOLS[eui]
                            self.is_in_anchor_state[eui] = True
                            print(f"- - Anchor {eui} is built successfully with coordinates: {coord}")
        
        # Check if all anchors are calibrated
        if all(self.is_in_anchor_state.values()):
            self.state = "flying"
            self.target_state = "ff"
            print("Self calibration complete, entering flying mode")

    def locate_anchor_using_calibration_data(self, anchor_eui: str, established_anchors: list) -> Optional[Tuple[float, float, float]]:
        """
        Locate an anchor using calibration data stored in calibration_data_matrix.
        This anchor acts as a 'tag' being located by established anchors.
        """
        # Get distances from this anchor (acting as tag) to established anchors
        distances = []
        anchor_coords = []
        
        print(f"Attempting to locate anchor {anchor_eui} using established anchors:")
        
        for established_anchor in established_anchors:
            established_anchor_eui = established_anchor.eui
            if established_anchor.coordinate is not None:
                # Get distance from calibration data
                distance = self.uwb_calibration_data_matrix.get_distance(anchor_eui, established_anchor_eui)
                print(f"  Distance from {anchor_eui} to {established_anchor_eui}: {distance}")
                
                if distance is not None and distance > 0:
                    distances.append(distance)
                    anchor_coords.append(established_anchor.coordinate)
        
        print(f"  Found {len(distances)} valid distances")
        
        if len(distances) >= 4:
            # Use multilateration to find position
            coord = self.multilaterate_position(anchor_coords, distances, tol=1e-6)
            if coord is not None:
                print(f"  Successfully calculated position: {coord}")
            else:
                print(f"  Multilateration failed")
            return coord
        else:
            print(f"  Not enough distances ({len(distances)} < 4)")
        
        return None

    def multilaterate_position(self, anchor_coords: list, distances: list, tol: float = 1e-6) -> Optional[Tuple[float, float, float]]:
        """
        Perform multilateration to find position given anchor coordinates and distances.
        Uses the existing gps_solve function from algorithms.py
        """
        from algorithms import gps_solve
        import numpy as np
        
        if len(anchor_coords) < 4 or len(distances) < 4:
            return None
        
        try:
            # Convert anchor coordinates to numpy arrays if they aren't already
            stations_coordinates = [np.array(coord) if not isinstance(coord, np.ndarray) else coord 
                                for coord in anchor_coords[:4]]
            distances_to_station = distances[:4]
            
            # Use your existing gps_solve function
            result = gps_solve(distances_to_station, stations_coordinates, tol=tol)
            
            if result is not None:
                # Verify the solution makes sense
                if all(not (math.isnan(x) or math.isinf(x)) for x in result):
                    return tuple(result)
        except Exception as e:
            print(f"Multilateration failed: {e}")
        
        return None

def main(args=None):
    dbg("Starting UWB Positioning System...")

    run = runner()

    # def handle_sigint(sig, frame):
    #     dbg("Received interrupt signal, stopping system...")
    #     run.stop_system()
    #     sys.exit(0)

    # signal.signal(signal.SIGINT, handle_sigint)

    try:
        run.start_system()

        while run.running:
            time.sleep(1)

    except KeyboardInterrupt:
        dbg("Keyboard interrupt received, stopping system...")
        run.stop_system()
        sys.exit(0)
   
       


if __name__ == '__main__':
    main()
    
    
