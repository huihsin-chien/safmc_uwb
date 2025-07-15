#include "uwb_common.hpp"
#include "config.hpp"
#include <ESP.h>



bool isAnchor = false;

unsigned long lastSerialOutput = 0;
unsigned long lastCheck = 0;
int serialOutputCount = 0;

void as_tag();
void as_anchor();

void setup() {
    isAnchor = isAnchorByDefault;
    Serial.begin(9600);
    setupUWB(&EUI[0], self_device_address, isAnchor ? ANCHOR_FRAME_FILTER_CONFIG : TAG_FRAME_FILTER_CONFIG); // 2 is the device address of the anchorB
}
void try_to_change_state() {
    size_t poll_len = DW1000Ng::getReceivedDataLength();
    if (poll_len > 6 ) return;

    byte poll_data[poll_len];
    DW1000Ng::getReceivedData(poll_data, poll_len);

    char ch1 = poll_data[0];
    char ch2 = poll_data[1];

   
        

    if (ch1 == ch2 && ch1 != '\0') {
        char ch = ch1;
        bool newIsAnchor = isAnchor;

        if (isAnchor) {
            for (int i = 0; becomeTagSymbols[i] != '\0'; i++) {
                if (ch == becomeTagSymbols[i]) {
                    newIsAnchor = false;
                    break;
                }
            }
        } else {
            for (int i = 0; becomeAnchorSymbols[i] != '\0'; i++) {
                if (ch == becomeAnchorSymbols[i]) {
                    newIsAnchor = true;
                    break;
                }
            }
        }

       
        //////////
        Serial.print("Received change state command: ");
        //Serial.println(poll_data);

        if (newIsAnchor != isAnchor) {
            Serial.print("State changed: ");
            Serial.println(newIsAnchor ? "Anchor" : "Tag");

            isAnchor = newIsAnchor;
            DW1000Ng::enableFrameFiltering(isAnchor ? ANCHOR_FRAME_FILTER_CONFIG : TAG_FRAME_FILTER_CONFIG);
        } else {
            Serial.println("State unchanged.");
        }


    }
}


void loop() {
   
    try_to_change_state();
    

    ///////
    static unsigned long lastPrint = 0;
    unsigned long now = millis();
    if(now - lastPrint > 5000) {  // 每5秒印一次
        Serial.print("Current state: ");
        Serial.println(isAnchor ? "Anchor" : "Tag");
        lastPrint = now;
    }

    try{
        if (isAnchor) {
            as_anchor();
        } else {
            as_tag();
        }
    }
    catch(const std::exception& e){
        Serial.println("catch an exception!");
    }
    if(AUTO_RESTART){
        // 每10秒檢查一次
        if(millis() - lastCheck > 10000) {
            lastCheck = millis();
            if(serialOutputCount < 3) { // 10秒內少於3次輸出就當死掉
                ESP.restart();
            }
            serialOutputCount = 0; // reset counter
        }
    }
}

// 回傳新的 blink_rate
void as_tag() {
    delay(blink_rate);

    for (uint16_t target_anchor = 1; target_anchor <= 8; target_anchor++) {
        RangeResult result = DW1000NgRTLS_ext::tagFinishRange(target_anchor, 1500);
        if(result.success) {
            #ifndef FIXED_BLINK_RATE
            blink_rate = result.new_blink_rate;
            #endif
            Serial.println("weee~");
        } else {
            // 未成功的話，印出失敗訊息。格式為：`tag_range,${tag_eui},failed,${anchorDeviceAddress}`
            // TODO: 補印強度 dB 資料
            
            //7/10 暫時註解掉
            // Serial.print("tag_range,"); Serial.print(&EUI[18]); 
            // Serial.print(",failed,"); Serial.println(target_anchor);
        }
    }
}

byte MediatorUWB_device_address[2] = {0x02, 0x02}; // MediatorUWB_device_address先設定 0202 (eui 02:02)

void as_anchor(){
    // Serial.println("u stupid");
    // 取得 tagFinishRange() 傳出的封包，並回傳接受
    RangeAcceptResult result = DW1000NgRTLS_ext::anchorRangeAccept(NextActivity::ACTIVITY_FINISHED, blink_rate);
    if(!result.success) 
        return;
    
    delay(2); // Tweak based on your hardware

    // 取得 tagFinishRange() 呼叫的 transmitPoll() 傳出的封包的資料 
    // 疑問：這是transmitPoll() 還是 transmitFinalMessage() 的資料？

    size_t recv_len = DW1000Ng::getReceivedDataLength();
    byte recv_data[recv_len];
    DW1000Ng::getReceivedData(recv_data, recv_len);

    // 如果計算結果明顯不合理，則提早進入下一個迴圈
    if(result.range < 0.001 || result.range > 500) 
        return;

    // 印出距離訊息。格式為：`anchor_range,${distance},${tagEUI},${anchorEUI}, RX power:${RX_power}`
    char rangeCharArr[256];
    snprintf(rangeCharArr, 256, "anchor_range,%lf,%02x:%02x,%s, RX power: %f", 
        result.range, recv_data[8], recv_data[7], &EUI[18], DW1000Ng::getReceivePower());
    Serial.println(rangeCharArr);

    // 將距離資料送給 MediatorUWB
    byte tagDeviceAddress[] = {recv_data[8], recv_data[7]};
    DW1000NgRTLS_ext::transmitDataToMediatorUWB(MediatorUWB_device_address, tagDeviceAddress, result.range, DW1000Ng::getReceivePower());

}
