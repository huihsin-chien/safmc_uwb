# data_manager.py
import os
import csv
import time
from typing import List, Optional, Tuple
from data_structure import UWBDataMatrix
from config import SAVE_DATA, DATA_FOLDER

class DataManager:
    """數據管理器，負責數據的保存和加載"""
    
    def __init__(self, save_data: bool = SAVE_DATA):
        self.save_data = save_data
        self.data_folder = DATA_FOLDER
        
        if self.save_data:
            self._initialize_data_storage()
    
    def _initialize_data_storage(self):
        """初始化數據存儲"""
        os.makedirs(self.data_folder, exist_ok=True)
        
        # 建立時間戳用於檔名
        self.timestamp_str = time.strftime('%Y%m%d_%H%M%S')
        
        # 初始化檔案路徑
        self.multilateration_file = os.path.join(
            self.data_folder, 
            f"multilateration_results_{self.timestamp_str}.csv"
        )
        
        self.serial_read_file = os.path.join(
            self.data_folder, 
            f"serial_read_{self.timestamp_str}.csv"
        )
        
        # 建立多點定位結果檔案
        self._create_multilateration_file()
        
        # 建立串口讀取結果檔案
        self._create_serial_read_file()
    
    def _create_multilateration_file(self):
        """建立多點定位結果檔案"""
        with open(self.multilateration_file, mode='w', newline='') as file:
            csv_writer = csv.writer(file, escapechar='"')
            csv_writer.writerow(["timestamp", "tag_eui", "x", "y", "z"])
    
    def _create_serial_read_file(self):
        """建立串口讀取結果檔案"""
        with open(self.serial_read_file, mode='w', newline='') as file:
            csv_writer = csv.writer(file, escapechar='"')
            csv_writer.writerow(["timestamp", "portstr", "line"])
    
    def save_multilateration_result(self, tag_eui: str, coordinate: Tuple[float, float, float]):
        """保存多點定位結果"""
        if not self.save_data:
            return
            
        timestamp_str = time.strftime('%Y-%m-%d %H:%M:%S')
        
        try:
            with open(self.multilateration_file, mode='a', newline='') as file:
                csv_writer = csv.writer(file, escapechar='"')
                csv_writer.writerow([timestamp_str, tag_eui, coordinate[0], coordinate[1], coordinate[2]])
        except Exception as e:
            print(f"Error saving multilateration result: {e}")
    
    def save_serial_data(self, port_str: str, line: str):
        """保存串口數據"""
        if not self.save_data:
            return
            
        timestamp_str = time.strftime('%Y-%m-%d %H:%M:%S')
        
        try:
            with open(self.serial_read_file, mode='a', newline='') as file:
                csv_writer = csv.writer(file, escapechar='"')
                csv_writer.writerow([timestamp_str, port_str, line])
        except Exception as e:
            print(f"Error saving serial data: {e}")
    
    def save_measurement_data(self, anchor_eui: str, tag_eui: str, distance: float):
        """保存測量數據"""
        if not self.save_data:
            return
            
        timestamp_str = time.strftime('%Y-%m-%d %H:%M:%S')
        anchor_eui_encoded = anchor_eui.replace(":", "-")
        anchor_file_path = os.path.join(
            self.data_folder, 
            f"Device_{anchor_eui_encoded}_{self.timestamp_str}.csv"
        )
        
        try:
            # 檢查檔案是否存在，如果不存在則建立
            if not os.path.exists(anchor_file_path):
                with open(anchor_file_path, mode='w', newline='') as file:
                    csv_writer = csv.writer(file, escapechar='"')
                    csv_writer.writerow(["timestamp", "tag_eui", "distance"])
            
            # 添加數據
            with open(anchor_file_path, mode='a', newline='') as file:
                csv_writer = csv.writer(file, escapechar='"')
                csv_writer.writerow([timestamp_str, tag_eui, distance])
        except Exception as e:
            print(f"Error saving measurement data: {e}")

class UWBDataManager:
    """UWB數據管理器的擴充版本"""
    
    def __init__(self, data_matrix: UWBDataMatrix, calibration_data_matrix:UWBDataMatrix, data_manager: DataManager):
        self.data_matrix = data_matrix
        self.calibration_data_matrix = calibration_data_matrix
        
        self.data_manager = data_manager
    
    def add_measurement(self, tag_eui: str, anchor_eui: str, distance: float, calibration: bool):
        """添加測量數據並保存"""
        # print(f"Adding measurement: {tag_eui} -> {anchor_eui}: {distance}")
        
        if calibration:
            self.calibration_data_matrix.add_measurement(tag_eui, anchor_eui, distance)
            self.data_manager.save_measurement_data(anchor_eui, tag_eui, distance)
        else:
            self.data_matrix.add_measurement(tag_eui, anchor_eui, distance)
            self.data_manager.save_measurement_data(anchor_eui, tag_eui, distance)
    
    def locate_tag(self, tag_eui: str, tol: float = 0.0009) -> Optional[Tuple[float, float, float]]:
        """定位tag並保存結果"""
        from algorithms import estimate_position_with_fallback
        
        coordinate = estimate_position_with_fallback(
            self.data_matrix, tag_eui, self.data_matrix.anchors, tol
        )
        
        if coordinate is not None:
            # 更新tag座標
            if tag_eui in self.data_matrix.tags:
                self.data_matrix.tags[tag_eui].update_coordinate(list(coordinate))
              
            # 保存結果
            self.data_manager.save_multilateration_result(tag_eui, coordinate)
            
        return coordinate
    
    def get_anchor_coordinates(self) -> dict:
        """取得所有anchor的座標"""
        coordinates = {}
        for anchor_eui, anchor in self.data_matrix.anchors.items():
            if anchor.coordinate is not None:
                coordinates[anchor_eui] = anchor.coordinate
        return coordinates
    
    def get_tag_coordinates(self) -> dict:
        """取得所有tag的座標"""
        coordinates = {}
        for tag_eui, tag in self.data_matrix.tags.items():
            if tag.coordinate is not None:
                coordinates[tag_eui] = tag.coordinate
        return coordinates
    
    
    def cleanup_old_data(self, max_age: float = 300):
        """清理舊數據"""
        for tag_eui in self.data_matrix.tags.keys():
            for anchor_eui in self.data_matrix.anchors.keys():
                self.data_matrix.clear_outdated_measurements(tag_eui, anchor_eui)