#include "uwb_common.hpp"
#include "DW1000NgRTLS_ext.hpp"

const char EUI[] = "AA:BB:CC:DD:EE:FF:02:02";
const uint16_t self_device_address = 0x0202;

bool changeStateList[8] = {1, 0,0,0,0,0,0,0}; 
// 用來儲存 serial 中的 change state 資料，如果 changeStateList[n] 為0，則 anchor n+1 不用 change state
bool newChangeStateList[8] = {}; 


void getAnchorRangeReport();

void setup() {
    Serial.begin(9600);
    setupUWB(&EUI[0], self_device_address, ANCHOR_FRAME_FILTER_CONFIG);
}

void call_brodcast(byte reciever_short_address[]) {
    // 建立廣播訊息陣列
    byte broadcast_state[] = {DATA, SHORT_SRC_AND_DEST, DW1000NgRTLS_ext::increaseSequenceNumber(), 0,0, 0,0, 0,0, ACTIVITY_CONTROL, RANGING_CONTINUE, 0, 0};
    DW1000Ng::getNetworkId(&broadcast_state[3]);
    memcpy(&broadcast_state[5], reciever_short_address, 2);
    DW1000Ng::getDeviceAddress(&broadcast_state[7]);
    // 發送廣播訊息
    DW1000Ng::setTransmitData(broadcast_state, sizeof(broadcast_state));
    DW1000Ng::startTransmit();
    Serial.print("Broadcasting to: 0x");
    Serial.print(reciever_short_address[0], HEX);
    Serial.print(reciever_short_address[1], HEX);
    Serial.print(", 0x");
    Serial.println();
}


void change_broadcast_state() {
    // 讀取狀態轉換指令
    static char prev_ch = '\0';  // 使用 static 變數記住上一個字元


    while(Serial.available()){
        char newCh = Serial.read();
        
        // 收到的訊息可能為 11 or 22 or 33 等單個數字重複兩次，也有可能是 556677 or 5588 等多個數字重複兩次
        // 收到的數字，及代表該 anchor 應以 anchor 方式表現
        // 僅在連續輸入相同字元時才判斷，以避免雜訊
        if(newCh == prev_ch && newCh != '\0') {
            changeStateList[newCh-1-'0'] = true; 
            prev_ch = '\0';  // 重置，避免重複觸發  
        } else {
            prev_ch = newCh;
        }
    } // end of while(Serial.available())
}

void loop() {
    // 讀取狀態轉換指令 
    if(Serial.available() > 0)
        change_broadcast_state();

    try{
        getAnchorRangeReport();
    }
    catch(const std::exception& e){

        Serial.println("Exception in getAnchorRangeReport");
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

            //如果此 Anchor 在要換狀態的 anchor list 中，且現在狀態還不是anchor，則使用call_brodcast。如果現在已經是anchor，那就不傳送。
            //TODO: 無法確定對面是不易已經變成 anchor了。要讓對面回傳已經是anchor的通知
            if (changeStateList[(int)poll_data[7]-1] && !newChangeStateList[(int)poll_data[7]-1]){
                byte receiver_address[2] = { (byte)poll_data[8], (byte)poll_data[7]};
                call_brodcast(receiver_address);
                // newChangeStateList[(int)poll_data[7]-1] = true;
            }
            else{
                Serial.println('no change state list');
                Serial.println((int)poll_data[7]-1);
            }
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

