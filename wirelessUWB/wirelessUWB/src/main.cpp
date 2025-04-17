#include "uwb_common.hpp"

// == START OF Device Config ==
#define TAG_4
#ifdef ANCHOR_1
const char EUI[] = "AA:BB:CC:DD:EE:FF:00:01";
const uint16_t self_device_address = 0x0001;
const bool isAnchorByDefault = true;
const char becomeTagSymbols[] = "";
const char becomeAnchorSymbols[] = "";
#endif

#ifdef ANCHOR_2
const char EUI[] = "AA:BB:CC:DD:EE:FF:00:02";
const uint16_t self_device_address = 0x0002;
const bool isAnchorByDefault = false;
const char becomeTagSymbols[] = "1";
const char becomeAnchorSymbols[] = "234567f";
#endif

#ifdef ANCHOR_3
const char EUI[] = "AA:BB:CC:DD:EE:FF:00:03";
const uint16_t self_device_address = 0x0003;
const bool isAnchorByDefault = false;
const char becomeTagSymbols[] = "12";
const char becomeAnchorSymbols[] = "34567f";
#endif

#ifdef ANCHOR_4
const char EUI[] = "AA:BB:CC:DD:EE:FF:00:04";
const uint16_t self_device_address = 0x0004;
const bool isAnchorByDefault = false;
const char becomeTagSymbols[] = "123";
const char becomeAnchorSymbols[] = "4567f";
#endif

#ifdef ANCHOR_5
const char EUI[] = "AA:BB:CC:DD:EE:FF:00:05";
const uint16_t self_device_address = 0x0005;
const bool isAnchorByDefault = false;
const char becomeTagSymbols[] = "1234";
const char becomeAnchorSymbols[] = "5f";
#endif

#ifdef ANCHOR_6
const char EUI[] = "AA:BB:CC:DD:EE:FF:00:06";
const uint16_t self_device_address = 0x0006;
const bool isAnchorByDefault = false;
const char becomeTagSymbols[] = "1234";
const char becomeAnchorSymbols[] = "6f";
#endif

#ifdef ANCHOR_7
const char EUI[] = "AA:BB:CC:DD:EE:FF:00:07";
const uint16_t self_device_address = 0x0007;
const bool isAnchorByDefault = false;
const char becomeTagSymbols[] = "1234";
const char becomeAnchorSymbols[] = "7f";
#endif

#ifdef ANCHOR_8
const char EUI[] = "AA:BB:CC:DD:EE:FF:00:08";
const uint16_t self_device_address = 0x0008;
const bool isAnchorByDefault = false;
const char becomeTagSymbols[] = "1234";
const char becomeAnchorSymbols[] = "8f";
#endif

#if defined(TAG_1) || defined(TAG_2) || defined(TAG_3) || defined(TAG_4) \
    || defined(TAG_5) || defined(TAG_6) || defined(TAG_7) || defined(TAG_8) \
    || defined(TAG_9) || defined(TAG_10)
const bool isAnchorByDefault = false;
const char becomeTagSymbols[] = "";
const char becomeAnchorSymbols[] = "";
#endif

#if defined(TAG_5) || defined(TAG_6) || defined(TAG_7) || defined(TAG_8) \
    || defined(TAG_9) || defined(TAG_10)
#define FIXED_BLINK_RATE 1000
#endif

#ifdef TAG_1
const char EUI[] = "AA:BB:CC:DD:EE:FF:01:01";
const uint16_t self_device_address = 0x0101;
#endif

#ifdef TAG_2
const char EUI[] = "AA:BB:CC:DD:EE:FF:02:02";
const uint16_t self_device_address = 0x0202;
#endif

#ifdef TAG_3
const char EUI[] = "AA:BB:CC:DD:EE:FF:03:03";
const uint16_t self_device_address = 0x0303;
#endif

#ifdef TAG_4
const char EUI[] = "AA:BB:CC:DD:EE:FF:04:04";
const uint16_t self_device_address = 0x0404;
#endif

#ifdef TAG_5
const char EUI[] = "AA:BB:CC:DD:EE:FF:05:05";
const uint16_t self_device_address = 0x0505;
#endif

#ifdef TAG_6
const char EUI[] = "AA:BB:CC:DD:EE:FF:06:06";
const uint16_t self_device_address = 0x0606;
#endif

#ifdef TAG_7
const char EUI[] = "AA:BB:CC:DD:EE:FF:07:07";
const uint16_t self_device_address = 0x0707;
#endif

#ifdef TAG_8
const char EUI[] = "AA:BB:CC:DD:EE:FF:08:08";
const uint16_t self_device_address = 0x0808;
#endif

#ifdef TAG_9
const char EUI[] = "AA:BB:CC:DD:EE:FF:09:09";
const uint16_t self_device_address = 0x0909;
#endif

#ifdef TAG_10
const char EUI[] = "AA:BB:CC:DD:EE:FF:10:10";
const uint16_t self_device_address = 0x1010;
#endif


// ==  END OF Device Config  ==
#ifdef FIXED_BLINK_RATE
uint32_t blink_rate = FIXED_BLINK_RATE;
#else
uint32_t blink_rate = 50;
#endif

bool isAnchor = false;

void as_tag();
void as_anchor();

void setup() {
    // 為了測試方便，先不使用 Change state，跳過 self-calibration，讓所有 anchor 一開始就是 anchor。
    if (!isAnchorByDefault && sizeof(becomeTagSymbols) == 0){
        isAnchor = true;
    }

    Serial.println("### DW1000Ng-arduino-ranging-tag2 ###");
    isAnchor = isAnchorByDefault;
    Serial.begin(9600);
    setupUWB(&EUI[0], self_device_address, isAnchor ? ANCHOR_FRAME_FILTER_CONFIG : TAG_FRAME_FILTER_CONFIG); // 2 is the device address of the anchorB
}

void loop() {
    try{
        if (isAnchor) {
            as_anchor();
        } else {
            as_tag();
        }
    }
    catch(const std::exception& e){
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
        } else {
            // 未成功的話，印出失敗訊息。格式為：`tag_range,${tag_eui},failed,${anchorDeviceAddress}`
            // TODO: 補印強度 dB 資料
            Serial.print("tag_range,"); Serial.print(&EUI[18]); 
            Serial.print(",failed,"); Serial.println(target_anchor);
        }
    }
}

byte MediatorUWB_device_address[2] = {0x01, 0x00}; // MediatorUWB_device_address[0] = 1, MediatorUWB_device_address[1] = 0


void as_anchor(){
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

    // 印出距離訊息
    // char rangeNumString[32];
    // snprintf(rangeNumString, 32, "%lf", result.range);
    // String rangeString = "Range: "; rangeString += rangeNumString; rangeString += " m";
    // rangeString += "\t RX power: "; rangeString += DW1000Ng::getReceivePower(); rangeString += " dBm distance between anchor/tag:";
    // rangeString += recv_data[7]; rangeString += recv_data[8]; // tag 的短 EUI
    // rangeString += " from Anchor "; rangeString += EUI[18]; rangeString += EUI[19]; rangeString += EUI[20]; rangeString += EUI[21]; rangeString += EUI[22]; rangeString += EUI[23];
    // Serial.println(rangeString);

    // 印出距離訊息。格式為：`anchor_range,${distance},${tagEUI},${anchorEUI}`
    char rangeCharArr[256];
    snprintf(rangeCharArr, 256, "anchor_range,%lf,%02x:%02x,%s", result.range, recv_data[8], recv_data[7], &EUI[18]);
    Serial.println(rangeCharArr);

    // 將距離資料送給 MediatorUWB
    byte tagDeviceAddress[] = {recv_data[8], recv_data[7]};
    DW1000NgRTLS_ext::transmitDataToMediatorUWB(MediatorUWB_device_address, tagDeviceAddress, result.range, DW1000Ng::getReceivePower());

}

