#include "uwb_common.hpp"

const char EUI[] = "AA:BB:CC:DD:EE:FF:02:02";
const uint16_t self_device_address = 0x0202;

void getAnchorRangeReport();

void setup() {
    Serial.begin(9600);
    setupUWB(&EUI[0], self_device_address, ANCHOR_FRAME_FILTER_CONFIG);
}


void change_broadcast_state(byte* broadcast_state) {
    // 讀取狀態轉換指令
    char ch = '\0';
    while(Serial.available()){
        char newCh = Serial.read();

        // 收到的訊息可能為 11 or 22 or 33 等單個數字重複兩次，也有可能是 556677 or 5588 等多個數字重複兩次
        
        // 僅在連續輸入相同字元時才判斷，以避免雜訊
        if(newCh != ch) {
            ch = newCh;
            continue;
        }
        

        
       
    } // end of while(Serial.available())
}

void loop() {
    // 讀取狀態轉換指令 TODO
    byte broadcast_state[5] = {0x00, 0x00, 0x00, 0x00, 0x00}; 
    if(Serial.available() > 0)
        change_broadcast_state(broadcast_state);

    try{
        getAnchorRangeReport();
    }
    catch(const std::exception& e){
    }
}

void getAnchorRangeReport(){
    if(!DW1000NgRTLS_ext::receiveFrame()){
        // Serial.println("no frame recieved");
        return;
    }else{
        size_t poll_len = DW1000Ng::getReceivedDataLength();
        byte poll_data[poll_len];
        DW1000Ng::getReceivedData(poll_data, poll_len);

        if(poll_len > 9 && poll_data[9] == RANGING_TAG_POLL){
        // uint64_t timePollReceived = DW1000Ng::getReceiveTimestamp();

        // 印出距離訊息
        // char temp_buff[256];
        // snprintf(temp_buff, 256, "data10: %x, data11: %x, data12: %x, data13: %x,data14: %x, data15: %x, data16: %x\n", 
        //     poll_data[10], poll_data[11],
        //     poll_data[12],poll_data[13], 
        //     poll_data[14],poll_data[15],poll_data[16]);
        // Serial.println(temp_buff);

        double range_scale = 0.0001;
        double power_scale = 0.1;
        char rangeCharArr[256];
        double range = 256.0 * (double)poll_data[12] + (double)poll_data[13];
        range *= range_scale;
        double rx_power = pow(256,2) * (double)poll_data[14] + 256 * (double)poll_data[15] + (double)poll_data[16];
        rx_power *= power_scale;
        snprintf(rangeCharArr, 256, "anchor_range,%f,%02x:%02x,%02x:%02x,-%f", //"anchor_range:%fm,tagEUI: %02x:%02x,anchorEUI: %02x:%02x,RX_power: -%f dBm"
            range, 
            poll_data[10], poll_data[11], 
            poll_data[8],poll_data[7], 
            rx_power);
        Serial.println(rangeCharArr);
        }
    }
}

