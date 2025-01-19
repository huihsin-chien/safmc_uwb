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
#else
const uint8_t PIN_RST = 15;
const uint8_t PIN_SS = SS; // spi select pin
#endif
char EUI[] = "AA:BB:CC:DD:EE:FF:00:02";
byte main_anchor_address[] = {0x01, 0x00};
uint16_t next_anchor = 3;

double range_self;

byte currentTagShortaddress[2]; // Array to store the tag's EUI (8 bytes)

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
    DW1000Ng::setEUI(EUI);
    DW1000Ng::setNetworkId(RTLS_APP_ID);

    // DW1000Ng::applyInterruptConfiguration(DEFAULT_INTERRUPT_CONFIG);

    DW1000Ng::setPreambleDetectionTimeout(64);
    DW1000Ng::setSfdDetectionTimeout(273);
    DW1000Ng::setReceiveFrameWaitTimeoutPeriod(5000);
    DW1000Ng::setNetworkId(RTLS_APP_ID);
    DW1000Ng::setDeviceAddress(2);
	
    DW1000Ng::setAntennaDelay(16436);

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
    delay(5000); 
}

void transmitRangeReport() {
    byte rangingReport[] = {DATA, SHORT_SRC_AND_DEST, DW1000NgRTLS::increaseSequenceNumber(), 0,0, 0,0, 0,0, 0x60, 0,0 };
    DW1000Ng::getNetworkId(&rangingReport[3]);
    memcpy(&rangingReport[5], main_anchor_address, 2);
    DW1000Ng::getDeviceAddress(&rangingReport[7]);
    DW1000NgUtils::writeValueToBytes(&rangingReport[10], static_cast<uint16_t>((range_self*1000)), 2);

    // Serial.print("Tag's short address: "); Serial.print(currentTagShortaddress[0], HEX); Serial.println(currentTagShortaddress[1], HEX);
    
    memcpy(&rangingReport[16], currentTagShortaddress, 2); // Add tag short address to the report
    Serial.println("Range report: ");
    for(size_t i = 0; i < 18; i++) {
        Serial.print(rangingReport[i], HEX); Serial.print(" ");
    }
    DW1000Ng::setTransmitData(rangingReport, sizeof(rangingReport));
    DW1000Ng::startTransmit();
    Serial.println("Transmitting range report");
}

void loop() {  
    RangeAcceptResult result = DW1000NgRTLS::anchorRangeAccept(NextActivity::RANGING_CONFIRM, next_anchor);
    if(result.success) {
        delay(2); // Tweak based on your hardware
        range_self = result.range;

        // Get the tag EUI from the received data
        size_t recv_len = DW1000Ng::getReceivedDataLength();
        byte recv_data[recv_len];
        DW1000Ng::getReceivedData(recv_data, recv_len);
        memcpy(currentTagShortaddress, &recv_data[7], 2); // EUI starts at position 2 (assuming EUI is 8 bytes long)
        // Serial.print("Received data: ");
        // for (size_t i = 0; i < recv_len; i++) {
        //     Serial.print(recv_data[i], HEX);
        //     Serial.print(" ");
        // }
        // Serial.println();
        Serial.print("Tag's short address: ");
        Serial.print(recv_data[7], HEX);Serial.println(recv_data[8], HEX);
        // recv_data[7] and recv_data[8] contain the tag's short address

        transmitRangeReport();

        String rangeString = "Range: "; rangeString += range_self; rangeString += " m";
        rangeString += "\t RX power: "; rangeString += DW1000Ng::getReceivePower(); rangeString += " dBm from";
        rangeString += recv_data[7]; rangeString += recv_data[8];
        Serial.println(rangeString);
    }
}

