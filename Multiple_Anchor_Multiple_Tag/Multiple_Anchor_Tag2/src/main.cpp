#include <DW1000Ng.hpp>
#include <DW1000NgUtils.hpp>
#include <DW1000NgRanging.hpp>
#include <DW1000NgRTLS.hpp>

// connection pins
#if defined(ESP8266)
const uint8_t PIN_SS = 10;
#else
const uint8_t PIN_SS = SS; // spi select pin
const uint8_t PIN_RST = 15;
#endif

device_configuration_t DEFAULT_CONFIG = {
    false,
    true,
    true,
    true,
    false,
    SFDMode::STANDARD_SFD,
    Channel::CHANNEL_5,
    DataRate::RATE_850KBPS,
    PulseFrequency::FREQ_16MHZ,
    PreambleLength::LEN_256,
    PreambleCode::CODE_3
};

frame_filtering_configuration_t TAG_FRAME_FILTER_CONFIG = {
    false, false, true, false, false, false, false, false
};

frame_filtering_configuration_t ANCHOR_FRAME_FILTER_CONFIG = {
    false, false, true, false, false, false, false,
    true /* This allows blink frames */
};

sleep_configuration_t SLEEP_CONFIG = {
    false,  // onWakeUpRunADC   reg 0x2C:00
    false,  // onWakeUpReceive
    false,  // onWakeUpLoadEUI
    true,   // onWakeUpLoadL64Param
    true,   // preserveSleep
    true,   // enableSLP    reg 0x2C:06
    false,  // enableWakePIN
    true    // enableWakeSPI
};

interrupt_configuration_t DEFAULT_INTERRUPT_CONFIG = {
    true,   // interruptOnSent
    true,   // interruptOnReceived
    false,  // interruptOnReceiveFailed
    false,  // interruptOnReceiveTimeout
    false,  // interruptOnReceiveTimestampAvailable
    false   // interruptOnAutomaticAcknowledgeTrigger
};


byte tag_short_address[] = {0x01, 0x02}; // 設定當前 tag 的短地址
byte main_anchor_address[] = {0x01, 0x00};
byte RANGING_RESPONSE = 0x60;

int calculateRange(byte response_data[]) {
    uint16_t range_raw = DW1000NgUtils::bytesAsValue(&response_data[10], 2);
    return range_raw / 1000;
}

void setup() {
    Serial.begin(9600);
    Serial.println(F("### arduino-DW1000Ng-ranging-tag ###"));
    DW1000Ng::initializeNoInterrupt(PIN_SS, PIN_RST);
    Serial.println(F("DW1000Ng initialized ..."));
    DW1000Ng::applyConfiguration(DEFAULT_CONFIG);
    DW1000Ng::enableFrameFiltering(ANCHOR_FRAME_FILTER_CONFIG);
    DW1000Ng::setEUI("AA:BB:CC:DD:EE:FF:00:05");
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
                        double range = calculateRange(response_data);
                        Serial.print("Range: ");
                        Serial.print(range);
                        Serial.println(" m");
                    }
                }
            }
        }
    }
}