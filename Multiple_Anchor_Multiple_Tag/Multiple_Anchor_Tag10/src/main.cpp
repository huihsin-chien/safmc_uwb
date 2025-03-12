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
    false, true, true, true, false,
    SFDMode::STANDARD_SFD,
    Channel::CHANNEL_2,
    DataRate::RATE_6800KBPS,
    PulseFrequency::FREQ_16MHZ,
    PreambleLength::LEN_64,
    PreambleCode::CODE_3
};

frame_filtering_configuration_t TAG_FRAME_FILTER_CONFIG = {
    false, false, true, false, false, false, false, false
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


byte tag_short_address[] = {0x10, 0x10}; // 設定當前 tag 的短地址
byte main_anchor_address[] = {0x01, 0x00};
char EUI[] = "AA:BB:CC:DD:EE:FF:10:10";
// byte RANGING_RESPONSE = 0x60;
volatile uint32_t blink_rate = 50;

int calculateRange(byte response_data[]) {
    uint16_t range_raw = DW1000NgUtils::bytesAsValue(&response_data[10], 2);
    return range_raw / 1000;
}

void setup() {
  delay(5000);
    // DEBUG monitoring
    Serial.begin(9600);
    Serial.println(F("### DW1000Ng-arduino-ranging-tag10 ###"));
    // initialize the driver
    #if defined(ESP8266)
    DW1000Ng::initializeNoInterrupt(PIN_SS);
    #else
    DW1000Ng::initializeNoInterrupt(PIN_SS, PIN_RST);
    #endif
    Serial.println("DW1000Ng initialized ...");
    // general configuration
    DW1000Ng::applyConfiguration(DEFAULT_CONFIG);
    DW1000Ng::enableFrameFiltering(TAG_FRAME_FILTER_CONFIG);
    
    DW1000Ng::setEUI(EUI);

    DW1000Ng::setNetworkId(RTLS_APP_ID);

    DW1000Ng::setAntennaDelay(16436);

    DW1000Ng::applySleepConfiguration(SLEEP_CONFIG);

    DW1000Ng::setPreambleDetectionTimeout(15);
    DW1000Ng::setSfdDetectionTimeout(273);
    DW1000Ng::setReceiveFrameWaitTimeoutPeriod(2000);
    
    Serial.println(F("Committed configuration ..."));
    // DEBUG chip info and registers pretty printed
    char msg[128];
    DW1000Ng::getPrintableDeviceIdentifier(msg);
    Serial.print("Device ID: "); Serial.println(msg);
    DW1000Ng::getPrintableExtendedUniqueIdentifier(msg);
    Serial.print("Unique ID: "); Serial.println(msg);
    DW1000Ng::getPrintableNetworkIdAndShortAddress(msg);
    Serial.print("Network ID & Device Address: "); Serial.println(msg);
    DW1000Ng::getPrintableDeviceMode(msg);
    Serial.print("Device mode: "); Serial.println(msg);   

    DW1000Ng::enableDebounceClock();
    DW1000Ng::enableLedBlinking();
    DW1000Ng::setGPIOMode(6, LED_MODE);
    DW1000Ng::setGPIOMode(8, LED_MODE);
    DW1000Ng::setGPIOMode(10, LED_MODE);
    DW1000Ng::setGPIOMode(12,   LED_MODE);
    delay(5000); // 等待 5 秒
}

void loop() {
    Serial.println("let's go~");
    DW1000Ng::deepSleep();
    delay(blink_rate);
    DW1000Ng::spiWakeup();
    DW1000Ng::setEUI(EUI);

    RangeInfrastructureResult res = DW1000NgRTLS::tagTwrLocalize(1500);
    if(res.success){
        Serial.println("result is right!");
        blink_rate = res.new_blink_rate;
    }
}
