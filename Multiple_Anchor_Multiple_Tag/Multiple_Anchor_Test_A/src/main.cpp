
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
#else
const uint8_t PIN_RST = 15;
const uint8_t PIN_SS = SS; // spi select pin
#endif

char EUI[] = "AA:BB:CC:DD:EE:FF:00:01";
Position position_self = {0,0};
Position position_B = {3,0};
Position position_C = {3,2.5};

double range_self;
double range_B;
double range_C;

boolean received_B = false;

byte tag1_shortAddress[] = {0x01, 0x01};
byte tag2_shortAddress[] = {0x02, 0x02};


byte anchor_b[] = {0x02, 0x00};
uint16_t next_anchor = 2;
byte anchor_c[] = {0x03, 0x00};


byte main_anchor_address[] = {0x01, 0x00};

double tag1_recommendation = 0;
double tag2_recommendation = 0;

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
    true
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
    Serial.println(F("### arduino-DW1000Ng-ranging-anchor-A ###"));
    // initialize the driver
    #if defined(ESP8266)
    DW1000Ng::initializeNoInterrupt(PIN_SS);
    #else
    DW1000Ng::initializeNoInterrupt(PIN_SS, PIN_RST);
    #endif
    Serial.println(F("DW1000Ng initialized ..."));
    DW1000Ng::applyConfiguration(DEFAULT_CONFIG);
    DW1000Ng::enableFrameFiltering(ANCHOR_FRAME_FILTER_CONFIG);
    DW1000Ng::setEUI(&EUI[0]);
    DW1000Ng::setPreambleDetectionTimeout(64);
    DW1000Ng::setSfdDetectionTimeout(273);
    DW1000Ng::setReceiveFrameWaitTimeoutPeriod(5000);
    DW1000Ng::setNetworkId(RTLS_APP_ID);
    DW1000Ng::setDeviceAddress(1);
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
    // DW1000Ng::applyInterruptConfiguration(DEFAULT_INTERRUPT_CONFIG);
    DW1000Ng::enableLedBlinking();
    DW1000Ng::setGPIOMode(15, LED_MODE);
    DW1000Ng::setGPIOMode(14, LED_MODE);
    DW1000Ng::setGPIOMode(13, LED_MODE);
    DW1000Ng::setGPIOMode(12,   LED_MODE);

    delay(5000);
}

void handleRanging(byte tag_shortAddress[]);
void calculatePosition(double &x, double &y);

void loop() {  
    // Handle ranging for tag1 and tag2
    handleRanging(tag1_shortAddress);
    handleRanging(tag2_shortAddress);
}

void handleRanging(byte tag_shortAddress[]) {
  // Serial.println("Ranging for tag");
  if(DW1000NgRTLS::receiveFrame()){
    // Serial.println("let's go~");
    size_t recv_len = DW1000Ng::getReceivedDataLength();
    byte recv_data[recv_len];
    DW1000Ng::getReceivedData(recv_data, recv_len);

    if(recv_data[0] == BLINK) {
      
      Serial.println("Received blink");
      
      // Extract tag EUI
      /*String tag_EUI = "";
      for (uint8_t i = 2; i < 4; i++) {
        tag_EUI += String(recv_data[i], HEX);
        if (i != 3) tag_EUI += ":";
      }
      Serial.print("Tag EUI: "); Serial.println(tag_EUI);*/
      Serial.print("Tag EUI: "); Serial.print(recv_data[2]); Serial.println(recv_data[3]);
      
      if (recv_data[2] == 1 && recv_data[3] == 1) {
        tag_shortAddress = tag1_shortAddress;
        tag1_recommendation = recv_data[12];
      } 
      else if (recv_data[2] == 2 && recv_data[3] == 2) {
        tag_shortAddress = tag2_shortAddress;
        tag2_recommendation = recv_data[12];
      }
      Serial.print("Tag short address: "); Serial.print(tag_shortAddress[0]); Serial.println(tag_shortAddress[1]);
      DW1000NgRTLS::transmitRangingInitiation(&recv_data[2], tag_shortAddress);
      DW1000NgRTLS::waitForTransmission();
      // ranginginitiation 有成功
      RangeAcceptResult result = DW1000NgRTLS::anchorRangeAccept(NextActivity::RANGING_CONFIRM, next_anchor);
      if(!result.success) return;
      range_self = result.range;
      Serial.println("Data from tag: ");
      for (uint16_t i = 0; i < 18; i++) {
        Serial.print(" 0x"); Serial.print(recv_data[i], HEX);
      }
      String rangeString = "Range: "; rangeString += range_self; rangeString += " m";
      rangeString += "\t RX power: "; rangeString += DW1000Ng::getReceivePower(); rangeString += " dBm from";
      rangeString += recv_data[2]; rangeString += recv_data[3];
      Serial.println(rangeString);
    } 
    else if(recv_data[9] == 0x60 && recv_data[12] == tag1_recommendation) { // tag1's short address
      double range = static_cast<double>(DW1000NgUtils::bytesAsValue(&recv_data[10],2) / 1000.0);
      String rangeReportString = "Range from: "; rangeReportString += recv_data[7]; // anchor's device address?
      rangeReportString += " = "; rangeReportString += range;
      Serial.println(rangeReportString);
      if(received_B == false && recv_data[7] == anchor_b[0] && recv_data[8] == anchor_b[1]) {
        range_B = range;
        received_B = true;
      } else if(received_B == true && recv_data[7] == anchor_c[0] && recv_data[8] == anchor_c[1]){
        range_C = range;
        double x,y;
        calculatePosition(x,y);
        String positioning = "Found position of Tag1 - x: ";
        positioning += x; positioning +=" y: ";
        positioning += y;
        Serial.println(positioning);
      }
    }
    else if(recv_data[9] == 0x60 && recv_data[12] == tag2_recommendation) { // tag2's short address
      double range = static_cast<double>(DW1000NgUtils::bytesAsValue(&recv_data[10],2) / 1000.0);
      String rangeReportString = "Range from: "; rangeReportString += recv_data[7]; // anchor's device address?
      rangeReportString += " = "; rangeReportString += range;
      Serial.println(rangeReportString);
      if(received_B == false && recv_data[7] == anchor_b[0] && recv_data[8] == anchor_b[1]) {
        range_B = range;
        received_B = true;
      } else if(received_B == true && recv_data[7] == anchor_c[0] && recv_data[8] == anchor_c[1]){
        range_C = range;
        double x,y;
        calculatePosition(x,y);
        String positioning = "Found position of Tag2 - x: ";
        positioning += x; positioning +=" y: ";
        positioning += y;
        Serial.println(positioning);
      }
    }
    // else if (recv_data[9]==0x60){
    //   Serial.println("Received range report");
    //   Serial.print("Anchor: "); Serial.print(recv_data[7]);
    //   // print all received data
    //   Serial.print("Received length: ");Serial.println(recv_len);
    //   for (uint16_t i = 0; i < 18; i++) {
    //     Serial.print(i);Serial.print(": 0x"); Serial.print(recv_data[i], HEX);
    //   }

    //   double range = static_cast<double>(DW1000NgUtils::bytesAsValue(&recv_data[10],2) / 1000.0);
    //   String rangeReportString = "Range from: "; rangeReportString += recv_data[16];rangeReportString += recv_data[17]; // anchor's device address?
    //   rangeReportString += "Range = "; rangeReportString += range;
    //   Serial.println(rangeReportString);
    // }
  }
}

/* using https://math.stackexchange.com/questions/884807/find-x-location-using-3-known-x-y-location-using-trilateration */
void calculatePosition(double &x, double &y) {

    /* This gives for granted that the z plane is the same for anchor and tags */
    double A = ( (-2*position_self.x) + (2*position_B.x) );
    double B = ( (-2*position_self.y) + (2*position_B.y) );
    double C = (range_self*range_self) - (range_B*range_B) - (position_self.x*position_self.x) + (position_B.x*position_B.x) - (position_self.y*position_self.y) + (position_B.y*position_B.y);
    double D = ( (-2*position_B.x) + (2*position_C.x) );
    double E = ( (-2*position_B.y) + (2*position_C.y) );
    double F = (range_B*range_B) - (range_C*range_C) - (position_B.x*position_B.x) + (position_C.x*position_C.x) - (position_B.y*position_B.y) + (position_C.y*position_C.y);

    x = (C*E-F*B) / (E*A-B*D);
    y = (C*D-A*F) / (B*D-A*E);
}
