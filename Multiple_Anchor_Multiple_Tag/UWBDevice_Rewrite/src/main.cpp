#include "uwb_common.hpp"

// == START OF Device Config ==
#define TAG_1

// const char becomeTagSuccessMessage[] = "";
// const char becomeAnchorSuccessMessage[] = "";

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
    || defined(TAG_5) || defined(TAG_6) || defined(TAG_7) || defined(TAG_8)
const bool isAnchorByDefault = false;
const char becomeTagSymbols[] = "";
const char becomeAnchorSymbols[] = "";
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

// ==  END OF Device Config  ==

uint32_t blink_rate = 50;
bool isAnchor = false;

void as_tag();
void as_anchor();

void setup() {
    Serial.println("### DW1000Ng-arduino-ranging-tag2 ###");
    isAnchor = isAnchorByDefault;
    Serial.begin(9600);
    setupUWB(&EUI[0], self_device_address, isAnchor ? ANCHOR_FRAME_FILTER_CONFIG : TAG_FRAME_FILTER_CONFIG); // 2 is the device address of the anchorB
}

void try_to_change_state() {
    // 讀取狀態轉換指令
    char ch = '\0';
    while(Serial.available()){
        char newCh = Serial.read();

        // 僅在連續輸入相同字元時才判斷，以避免雜訊
        if(newCh != ch) {
            ch = newCh;
            continue;
        }

        // 依照指令轉換狀態
        // 是 anchor 就判斷轉換成 tag，是 tag 就判斷轉換成 anchor
        if(isAnchor) {
            for(int i = 0; becomeTagSymbols[i] != '\0'; i++){
                if(ch == becomeTagSymbols[i]) {
                    isAnchor = false;
                    DW1000Ng::enableFrameFiltering(TAG_FRAME_FILTER_CONFIG);
                    // Serial.print(becomeTagSuccessMessage);
                }
            }
        } else {
            for(int i = 0; becomeAnchorSymbols[i] != '\0'; i++){
                if(ch == becomeAnchorSymbols[i]) {
                    isAnchor = true;
                    DW1000Ng::enableFrameFiltering(ANCHOR_FRAME_FILTER_CONFIG);
                    // Serial.print(becomeAnchorSuccessMessage);
                }
            }
        }
    }
}

void loop() {
    if(Serial.available() > 0)
        try_to_change_state();

    try{
        if(isAnchor) {
            as_anchor();
        } else {
            as_tag();
        }
    } catch (const std::exception& e) {
    }
}


// 回傳新的 blink_rate
void as_tag() {
    // Serial.println("let's go~");
    // DW1000Ng::deepSleep();
    delay(blink_rate);
    // DW1000Ng::spiWakeup();

    for (uint16_t target_anchor = 1; target_anchor <= 8; target_anchor++) {
        RangeResult result = DW1000NgRTLS_ext::tagFinishRange(target_anchor, 1500);
        if(result.success) {
            Serial.println("result is right!");
            blink_rate = result.new_blink_rate;
        }
    }
}

void as_anchor(){
    // 取得 tagFinishRange() 傳出的封包，並回傳接受
    RangeAcceptResult result = DW1000NgRTLS_ext::anchorRangeAccept(NextActivity::ACTIVITY_FINISHED, blink_rate);
    if(!result.success) 
        return;
    
    delay(2); // Tweak based on your hardware

    // 取得 tagFinishRange() 呼叫的 transmitPoll() 傳出的封包的資料
    size_t recv_len = DW1000Ng::getReceivedDataLength();
    byte recv_data[recv_len];
    DW1000Ng::getReceivedData(recv_data, recv_len);

    // 印出距離訊息
    String rangeString = "Range: "; rangeString += result.range; rangeString += " m";
    rangeString += "\t RX power: "; rangeString += DW1000Ng::getReceivePower(); rangeString += " dBm distance between anchor/tag:";
    rangeString += recv_data[7]; rangeString += recv_data[8]; // tag 的短 EUI
    rangeString += " from Anchor "; rangeString += EUI[18]; rangeString += EUI[19]; rangeString += EUI[20]; rangeString += EUI[21]; rangeString += EUI[22]; rangeString += EUI[23];
    Serial.println(rangeString);
}
