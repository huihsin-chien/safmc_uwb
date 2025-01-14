#include <DW1000Ng.hpp>
#include <DW1000NgUtils.hpp>
#include <DW1000NgRanging.hpp>
#include <DW1000NgRTLS.hpp>

// connection pins
#if defined(ESP8266)
const uint8_t PIN_SS = 15;
const uint8_t PIN_RST = 16;
const uint8_t PIN_IRQ = 5;
#elif defined(ESP32)
const uint8_t PIN_SS = 5;
const uint8_t PIN_RST = 17;
const uint8_t PIN_IRQ = 16;
#endif

byte tag_short_address[] = {0x01, 0x01}; // 設定當前 tag 的短地址

void setup() {
    Serial.begin(9600);
    Serial.println(F("### arduino-DW1000Ng-ranging-tag ###"));
    DW1000Ng::initializeNoInterrupt(PIN_SS, PIN_RST);
    Serial.println(F("DW1000Ng initialized ..."));
    DW1000Ng::applyConfiguration(DEFAULT_CONFIG);
    DW1000Ng::enableFrameFiltering(ANCHOR_FRAME_FILTER_CONFIG);
    DW1000Ng::setEUI("AA:BB:CC:DD:EE:FF:00:01");
    DW1000Ng::setPreambleDetectionTimeout(64);
    DW1000Ng::setSfdDetectionTimeout(273);
    DW1000Ng::applyInterruptConfiguration(DEFAULT_INTERRUPT_CONFIG);
}

void loop() {
    if (DW1000NgRTLS::receiveFrame()) {
        size_t recv_len = DW1000Ng::getReceivedDataLength();
        byte recv_data[recv_len];
        DW1000Ng::getReceivedData(recv_data, recv_len);

        // 檢查數據包類型是否為 RANGING_INITIATION
        if (recv_data[15] == RANGING_INITIATION) {
            // 檢查數據包中的短地址是否與當前 tag 的短地址匹配
            if (recv_data[16] == tag_short_address[0] && recv_data[17] == tag_short_address[1]) {
                // 當前 tag 是目標 tag，進行相應的處理
                Serial.println("Ranging initiation received for this tag.");
                // 進行測距請求
                DW1000NgRTLS::transmitPoll(main_anchor_address);
                DW1000NgRTLS::waitForTransmission();
                
                // 等待測距響應
                if (DW1000NgRTLS::receiveFrame()) {
                    size_t response_len = DW1000Ng::getReceivedDataLength();
                    byte response_data[response_len];
                    DW1000Ng::getReceivedData(response_data, response_len);

                    // 檢查數據包類型是否為 RANGING_RESPONSE
                    if (response_data[9] == RANGING_RESPONSE) {
                        // 處理測距響應
                        Serial.println("Ranging response received.");
                        // 計算距離並顯示
                        double range = DW1000NgUtils::calculateRange(response_data);
                        Serial.print("Range: ");
                        Serial.print(range);
                        Serial.println(" m");
                    }
                }
            }
        }
    }
}