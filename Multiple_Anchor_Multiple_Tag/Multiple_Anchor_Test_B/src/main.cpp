#include <DW1000Ng.hpp>
#include <DW1000NgUtils.hpp>
#include <DW1000NgRanging.hpp>
#include <DW1000NgRTLS.hpp>

typedef struct Position {
    double x;
    double y;
} Position;

// connection pins
#if defined(ESP8266)
const uint8_t PIN_SS = 15;
const uint8_t PIN_RST = 16;
const uint8_t PIN_IRQ = 5;
#elif defined(ESP32)
const uint8_t PIN_RST = 15;
const uint8_t PIN_SS = SS; // spi select pin
#endif

byte main_anchor_address[] = {0x01, 0x00};

uint16_t next_anchor = 3;


double range_self;

byte currentTagShortAddress[2];

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

frame_filtering_configuration_t ANCHOR_FRAME_FILTER_CONFIG = {
    false,
    false,
    true,
    false,
    false,
    false,
    false,
    false
};

// interrupt_configuration_t DEFAULT_INTERRUPT_CONFIG = {
//     true,
//     true,
//     true,
//     false,
//     true
// };

void setup() {
    delay(5000);
    Serial.begin(9600);
    Serial.println(F("### arduino-DW1000Ng-ranging-anchor-B ###"));
    DW1000Ng::initializeNoInterrupt(PIN_SS, PIN_RST);
    Serial.println(F("DW1000Ng initialized ..."));
    DW1000Ng::applyConfiguration(DEFAULT_CONFIG);
    DW1000Ng::enableFrameFiltering(ANCHOR_FRAME_FILTER_CONFIG);
    DW1000Ng::setEUI("AA:BB:CC:DD:EE:FF:00:02");
    DW1000Ng::setNetworkId(RTLS_APP_ID);

    DW1000Ng::setAntennaDelay(16436);

    // DW1000Ng::applyInterruptConfiguration(DEFAULT_INTERRUPT_CONFIG);

    DW1000Ng::setPreambleDetectionTimeout(64);
    DW1000Ng::setSfdDetectionTimeout(273);
    DW1000Ng::setReceiveFrameWaitTimeoutPeriod(5000);
    
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
    DW1000Ng::setGPIOMode(12, LED_MODE);
    DW1000Ng::setGPIOMode(13, LED_MODE);
    DW1000Ng::setGPIOMode(14, LED_MODE);
    DW1000Ng::setGPIOMode(15,LED_MODE);

    // DW1000Ng::applyInterruptConfiguration(DEFAULT_INTERRUPT_CONFIG);
    delay(5000); 
}

void transmitRangeReport() {
    byte rangingReport[] = {DATA, SHORT_SRC_AND_DEST, DW1000NgRTLS::increaseSequenceNumber(), 0,0, 0,0, 0,0, 0x60, 0,0 };
    DW1000Ng::getNetworkId(&rangingReport[3]);
    memcpy(&rangingReport[5], main_anchor_address, 2);
    DW1000Ng::getDeviceAddress(&rangingReport[7]);
    DW1000NgUtils::writeValueToBytes(&rangingReport[10], static_cast<uint16_t>((range_self*1000)), 2);
    memcpy(&rangingReport[12], currentTagShortAddress, 2); // Add tag short address to the report
    DW1000Ng::setTransmitData(rangingReport, sizeof(rangingReport));
    DW1000Ng::startTransmit();
}

void loop() {  
    RangeAcceptResult result = DW1000NgRTLS::anchorRangeAccept(NextActivity::RANGING_CONFIRM, next_anchor);
    if(result.success) {
        delay(2); // Tweak based on your hardware
        range_self = result.range;

        // Get the tag short address from the received data
        size_t recv_len = DW1000Ng::getReceivedDataLength();
        byte recv_data[recv_len];
        DW1000Ng::getReceivedData(recv_data, recv_len);
        memcpy(currentTagShortAddress, &recv_data[16], 2); // position: see void transmitRangingInitiation(byte tag_eui[], byte tag_short_address[]);

        transmitRangeReport();

        String rangeString = "Range: "; rangeString += range_self; rangeString += " m";
        rangeString += "\t RX power: "; rangeString += DW1000Ng::getReceivePower(); rangeString += " dBm";
        Serial.println(rangeString);
    }
}
