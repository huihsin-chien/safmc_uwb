# serial_manager.py
import serial
import serial.tools.list_ports
import time
from typing import List, Callable, Optional
from config import SERIAL_BAUDRATE, SERIAL_TIMEOUT, SERIAL_PORT_FILTER
from data_manager import DataManager



class UWBSerialDataProcessor:
    def __init__(self, uwb_data_manager, calibration:bool):
        self.uwb_data_manager = uwb_data_manager
        self.state_callback = None
        self.calibration = calibration
        

    def set_state_callback(self, callback: Callable[[str], None]):
        self.state_callback = callback

    def process_serial_data(self, port: str, data: str, calibration:bool) -> None:
        try:
            line = data.strip()
            # 距離資料格式 anchor_range,距離,from_eui,to_eui
            if line.startswith("anchor_range,"):
                values = line.split(",")
                if len(values) == 4:
                    _, distance, from_eui, to_eui = values
                    try:
                        distance = float(distance)
                        self.uwb_data_manager.add_measurement(from_eui, to_eui, distance, calibration)
                    except Exception as e:
                        print(f"Error parsing range data: {e}")
                else:
                    print(f"Malformed anchor_range line: {line}")
            
            else:
                pass # 可加入其他型態資料處理

        except Exception as e:
            print(f"Error processing serial data from {port}: {data} - {e}")

    def get_data_processor(self) -> Callable[[str, str], None]:
        return self.process_serial_data

class SerialManager:
    """串口管理器，負責串口的連接、讀取和寫入"""
    
    def __init__(self, data_manager: Optional[DataManager] = None):
        self.serials: List[serial.Serial] = []  # 初始化為空列表
        self.data_manager = data_manager
        self.port_filter = SERIAL_PORT_FILTER
        self.baudrate = SERIAL_BAUDRATE
        self.timeout = SERIAL_TIMEOUT
        
        
        
    def update_serial_list(self) -> None:  # 修正返回類型為 None
        """更新串口列表，新增新端口並移除無效端口"""
        # 取得目前可用端口
        existing_ports = set(
            port.device for port in serial.tools.list_ports.comports()
            if all(filter_str not in port.device for filter_str in SERIAL_PORT_FILTER)
        )
        print (existing_ports)
        
        
        # 移除無效端口
        opened_ports = set()
        invalid_ports = set()
        updated_serials = []
        
        for serial_connection in self.serials:
            if serial_connection.portstr not in existing_ports:
                invalid_ports.add(serial_connection.portstr)
                try:
                    serial_connection.close()
                except:
                    pass
            else:
                updated_serials.append(serial_connection)
                opened_ports.add(serial_connection.portstr)
        
        # 新增新端口
        new_ports = existing_ports - opened_ports
        for port in new_ports:
            try:
                new_serial = serial.Serial(
                    port, 
                    baudrate=self.baudrate, 
                    timeout=self.timeout
                )
                updated_serials.append(new_serial)
                print(f"Successfully opened port: {port}")
            except Exception as e:
                print(f"Failed to open port {port}: {e}")
        
        # 更新串口列表
        self.serials = updated_serials
        
        # 顯示端口變化
        if invalid_ports or new_ports:
            print(f"Ports changed! (={len(existing_ports)}, +{len(new_ports)}, -{len(invalid_ports)})")
            if new_ports:
                print(f"+ New ports: {new_ports}")
            if invalid_ports:
                print(f"- Invalid ports: {invalid_ports}")
        
        # 初始化時顯示所有已開啟的端口
        if self.serials:
            print(f"Total active ports: {len(self.serials)}")
            print(f"Active ports: {[s.port for s in self.serials]}")
    
    def broadcast_target_state(self, message: str, repeat_count: int = 3) -> None:
        """向所有串口廣播訊息"""
        print("braodcasting target state", message)
        self.update_serial_list()  # 確保串口列表是最新的
    
        for serial_connection in self.serials:
            try:
                # 重複發送訊息以確保可靠性
                
    
                bytes_written = serial_connection.write(message.encode('utf-8'))
                
                # 如果寫入失敗，重試
                retry_count = 0
                while bytes_written <= 0 and retry_count < 3:
                    time.sleep(0.01)
                    bytes_written = serial_connection.write(message.encode('utf-8'))
                    retry_count += 1
                    print(f"Retry {retry_count} for port {serial_connection.port}")
                    
            except Exception as e:
                print(f"Error sending message to {serial_connection.port}: {e}")
                self._try_reopen_port(serial_connection)
                
            time.sleep(0.1)  # 確保每次發送之間有足夠的間隔
                
        print ("end of broadcasting")
    
    def _try_reopen_port(self, serial_connection: serial.Serial) -> None:
        """嘗試重新開啟串口"""
        try:
            serial_connection.close()
            time.sleep(0.1)
            serial_connection.open()
        except Exception as e:
            print(f"Failed to reopen port {serial_connection.port}: {e}")
    
    def read_serial(self, data_processor:UWBSerialDataProcessor) -> None:
        """讀取所有串口資料並處理"""
        
        # print (len(self.serials), "serials to read")
        for serial_connection in self.serials:
            # print(f"Reading from {serial_connection.port}")
            try:
                lines = self._read_serial_lines(serial_connection)
                
                for line in lines:
                    if line.strip():
                        # 儲存原始資料
                        if self.data_manager:
                            self.data_manager.save_serial_data(serial_connection.port, line)
                        
                        # 處理資料
                        data_processor.process_serial_data(
                            serial_connection.port, line, data_processor.calibration
                        )
                        
                        
            except Exception as e:
                print(f"Error reading from {serial_connection.port}: {e}")
    
    def _read_serial_lines(self, serial_connection: serial.Serial) -> List[str]:
        """從單一串口讀取資料行"""
        # print("reading from", serial_connection.port)
        try:
            raw_lines = serial_connection.readlines()
            # print(f"Read {len(raw_lines)} lines from {serial_connection.port}")
            # print(f"Raw lines: {raw_lines}")
            return [line.decode("utf-8").strip() for line in raw_lines]
        except Exception as e:
            print(f"Error decoding data from {serial_connection.port}: {e}")
            return []
    
    def close_all_ports(self) -> None:
        """關閉所有串口"""
        for serial_connection in self.serials:
            try:
                serial_connection.close()
            except:
                pass
        self.serials.clear()
    
    def get_port_count(self) -> int:
        """取得活動端口數量"""
        return len(self.serials)
    
    def get_port_names(self) -> List[str]:
        """取得活動端口名稱列表"""
        return [serial_connection.port for serial_connection in self.serials]
    