import csv
import math
import os
import time
from typing import Optional, Tuple
from collections import deque
from config import SAVE_DATA, DATA_FOLDER, MULTILATERATION_TOLERANCE
from algorithms import gps_solve, align_coordinates, ClassicalMDS


SAVE_DATA = False  # Set to True if you want to enable data saving


class UWBData:
    
    def __init__(self, distance: float):
        self.distance = distance
        self.timestamp = time.time()
        
class UWBDevice:
    
    def __init__(self, eui: str):
        self.eui = eui
        self.coordinate = None

    def update_coordinate(self, coordinate) -> None:
        self.coordinate = coordinate
        
        
class UWBDataMatrix:

    def __init__(self, time_threshold: float, anchors: list[UWBDevice] = [], tags: list[UWBDevice] = []):
        self.time_threshold = time_threshold
        self.anchors = { anchor.eui: anchor for anchor in anchors }
        self.tags = { tag.eui: tag for tag in tags }
        self.data = { tag.eui: { anchor.eui: deque() for anchor in anchors } for tag in tags }

        if SAVE_DATA:
            self.timestamp_str = time.strftime('%Y%m%d_%H%M%S') # 記錄建立時間，用於儲存資料時的檔名

            # 為每個 Anchor 建立一個檔案，用以儲存測量資訊
            for anchor in anchors: 
                anchor_eui_encoded = anchor.eui.replace(":", "-")
                anchor_file_path = os.path.join(DATA_FOLDER, f"Anchor{anchor_eui_encoded}_{self.timestamp_str}.csv")
                with open(anchor_file_path, mode='w') as file:
                    csv_writer = csv.writer(file, escapechar='"')
                    csv_writer.writerow(["timestamp", "tag_eui", "distance"]) # 標題
                
                # 建立檔案，用以儲存定位結果
                self.multilateration_file = os.path.join(DATA_FOLDER, f"multilateration_results_{self.timestamp_str}.csv")
                with open(self.multilateration_file, mode='w', newline='') as file:
                    csv_writer = csv.writer(file, escapechar='"')
                    csv_writer.writerow(["timestamp", "x", "y", "z"])

    # 將測量資訊加入資料庫
    def add_measurement(self, tag_eui: str, anchor_eui: str, distance: float) -> None:
        
        if tag_eui not in self.data:
            return
        if anchor_eui not in self.data[tag_eui]:
            return

        self.data[tag_eui][anchor_eui].append(UWBData(distance))
        if SAVE_DATA:
            timestamp_str = time.strftime('%Y-%m-%d %H:%M:%S')
            anchor_eui_encoded = anchor_eui.replace(":", "-")
            anchor_file_path = os.path.join(DATA_FOLDER, f"Device_{anchor_eui_encoded}_{self.timestamp_str}.csv")
            with open(anchor_file_path, mode='a') as file:
                csv_writer = csv.writer(file, escapechar='"')
                csv_writer.writerow([timestamp_str, tag_eui, distance])

    # 清除過時的測量資訊
    def clear_outdated_measurements(self, tag_eui: str, anchor_eui: str) -> None:
        measurements = self.data[tag_eui][anchor_eui]
        while len(measurements) > 0 and measurements[0].timestamp < time.time() - self.time_threshold:
            self.data[tag_eui][anchor_eui].popleft()

    # 取得去極值後的距離
    def get_distance(self, tag_eui: str, anchor_eui: str) -> Optional[float]:
        self.clear_outdated_measurements(tag_eui, anchor_eui)

       
        measurements: deque[UWBData] = self.data[tag_eui][anchor_eui]
        distances: list[float] = [measurement.distance for measurement in measurements]

        # # 去除極值
        # q1 = np.percentile(distances, 25) if distances else float("-inf")
        # q3 = np.percentile(distances, 75) if distances else float("inf")
        # filtered_distances = [distance for distance in distances if q1 <= distance <= q3]

        # # 資料不足提早離開
        # if len(filtered_distances) <= 0:
        #     return None

        # # 線性修正固定偏差值 & 縮放比例，來提高精準度
        # trimmed_mean = np.mean(filtered_distances)
        # estimated_real_distance = (trimmed_mean - 0.1766) / 1.0349

        # # 計算並回傳平均值
        # return estimated_real_distance

        # 資料不足提早離開
        if len(distances) <= 0:
            return None
        
        distances.sort()
        return distances[len(distances) // 2]

    # 計算多點定位（multilateration）的結果
    def locate_tag(self, tag_eui: str, tol: float=MULTILATERATION_TOLERANCE) -> Optional[Tuple[float, float, float]]:
        # tol: 0.0009 m^2 = (3 cm)^2

        # 如果沒有這個 Tag 的資料，則回傳 None
        # if tag_eui not in self.data:
            # return None 

        # 蒐集該 tag 到各 anchor 的距離 & 各 anchor 座標
        distances_to_stations = []
        stations_coordinates = []
        for anchor_eui in self.anchors.keys():
            distance = self.get_distance(tag_eui, anchor_eui)
            if distance is not None and self.anchors[anchor_eui].coordinate is not None:
                distances_to_stations.append(distance)
                stations_coordinates.append(self.anchors[anchor_eui].coordinate)
            if len(distances_to_stations) > 4: # 一旦有五筆資料便提早離開，以加速進程
                break

        # 如果資訊不足，致無法定位，則回傳 None
        if len(distances_to_stations) < 4:
            return None
        
        # 若出現奇怪的數學問題，回傳 None
        try:
            coordinate = gps_solve(distances_to_stations, stations_coordinates, initial_guess=self.tags[tag_eui].coordinate, tol=tol)
        except Exception as e:
            print(f"Error locating tag {tag_eui}: {e}")
            return None

        # 如果有 inf/-inf/nan，則回傳 None
        if any(math.isinf(num) or math.isnan(num) for num in coordinate):
            return None

        if coordinate is not None:
            self.tags[tag_eui].update_coordinate(coordinate)

        return coordinate

    