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
    false,
    false,
    true,
    false,
    false,
    false,
    false,
    false
};

void setup() {
    Serial.begin(9600);
    Serial.println(F("### DW1000Ng-arduino-ranging-tag2 ###"));
    DW1000Ng::initializeNoInterrupt(PIN_SS, PIN_RST);
    Serial.println("DW1000Ng initialized ...");
    DW1000Ng::applyConfiguration(DEFAULT_CONFIG);
    DW1000Ng::enableFrameFiltering(TAG_FRAME_FILTER_CONFIG);
    DW1000Ng::setEUI("AA:BB:CC:DD:EE:FF:00:05");
    DW1000Ng::setNetworkId(RTLS_APP_ID);
    DW1000Ng::setAntennaDelay(16436);
    DW1000Ng::setPreambleDetectionTimeout(15);
    DW1000Ng::attachInterrupt(handleInterrupt);
    DW1000Ng::enableInterrupt();
}

void loop() {
    DW1000Ng::deepSleep();
    delay(200);
    DW1000Ng::spiWakeup();
    DW1000Ng::setEUI("AA:BB:CC:DD:EE:FF:00:05");

    RangeInfrastructureResult res = DW1000NgRTLS::tagTwrLocalize(1500);
    if(res.success)
        delay(res.new_blink_rate);
}

void handleInterrupt() {
    DW1000Ng::interruptHandler();
}