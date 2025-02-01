#include <uwb_common.cpp>
#include <uwb_common.hpp>


// Device EUI
char EUI[] = "AA:BB:CC:DD:EE:FF:00:01";

// Position of the device itself
Position position_self = {0, 0};

// Ranges to other devices
double range_self;
double range_B;
double range_C;
uint16_t self_device_address =1;
// Flags to indicate if data from other devices has been received
boolean received_B = false;

// Short addresses for tags
byte tag1_shortAddress[] = {0x01, 0x01};
byte tag2_shortAddress[] = {0x02, 0x02};

// Addresses for anchors
byte anchor_b[] = {0x02, 0x00};
uint16_t next_anchor = 2;
byte anchor_c[] = {0x03, 0x00};

// ranging counter (per second)
uint16_t successRangingCount = 0;
uint32_t rangingCountPeriod = 0;
float samplingRate = 0;

byte tag1_recommendation ;
byte tag2_recommendation;



void handleRanging_coord_1();
void handleRanging_coord_2();

class StateMachine{
  private:
    State state;
    int sample_count = 0;
    unsigned long startTime = 0;
  public:
    StateMachine(){
      state = State::built_coord_1;
    }
    void ranging(){
      switch(state){
        case State::built_coord_1:
          // handleRanging_coord_1();
          handleRanging_coord_2();    
          break;
        case State::built_coord_2:
          handleRanging_coord_2();
          break;
      }
    }
    void update(){
      switch(state){
        case State::built_coord_1:
          if(sample_count == 100 || (millis() - startTime) > 12000){
            state = State::built_coord_2;
            sample_count = 0;
            startTime = millis();
            Serial.println("State changed to built_coord_2");
          }
          break;
        case State::built_coord_2:
          // if(sample_count == 100 || (millis() - startTime) > 12000){
          //   state = State::self_calibration;
          //   sample_count = 0;
          //   startTime = millis();
          //   Serial.println("State changed to self_calibration");
          // }
          break;
        case State::self_calibration:
          break;
      }
    }
    State getState(){
      return state;
    }
    void addSampleCount(){
      sample_count++;
    }
  
};


void setup() {
    delay(5000);
    Serial.begin(9600);
    setupUWB(&EUI[0], self_device_address, ANCHOR_FRAME_FILTER_CONFIG); // 2 is the device address of the anchorB
    delay(5000);
}
StateMachine anchorStateMachine;
void handleRanging(byte tag_shortAddress[]);
void calculatePosition(double &x, double &y);

void loop() {  
  anchorStateMachine.ranging();
  anchorStateMachine.update();
}

void handleRanging_coord_1() {
  byte tag_shortAddress[2];
  tag_shortAddress[0] = anchor_c[0];
  tag_shortAddress[1] = anchor_c[1];
  if(DW1000NgRTLS::receiveFrame()){
    size_t recv_len = DW1000Ng::getReceivedDataLength();
    byte recv_data[recv_len];
    DW1000Ng::getReceivedData(recv_data, recv_len);

    if(recv_data[0] == BLINK) {
      
      Serial.println("Received blink");
      Serial.print("Tag EUI: "); Serial.print(recv_data[2]); Serial.println(recv_data[3]);
      Serial.print("Tag short address: "); Serial.print(tag_shortAddress[0]); Serial.println(tag_shortAddress[1]);
      DW1000NgRTLS::transmitRangingInitiation(&recv_data[2], tag_shortAddress);
      DW1000NgRTLS::waitForTransmission();
      RangeAcceptResult result = DW1000NgRTLS::anchorRangeAccept(NextActivity::ACTIVITY_FINISHED, 200);//uint16_t blink_rate = 200;
      if(!result.success) return;
      range_self = result.range;
      // Serial.println("Data from tag: ");
      // for (uint16_t i = 0; i < 18; i++) {
      //   Serial.print(" 0x"); Serial.print(recv_data[i], HEX);
      // }
      String rangeString = "Range: "; rangeString += range_self; rangeString += " m";
      rangeString += "\t RX power: "; rangeString += DW1000Ng::getReceivePower(); rangeString += " dBm    distance from anchor/tag:";
      rangeString += recv_data[2]; rangeString += recv_data[3];
      Serial.println(rangeString);
      anchorStateMachine.addSampleCount();  // todo: 分別收到哪個 anchor 的記數
    } 
  }
}

void handleRanging_coord_2() {
  // Serial.println("handleRanging_coord_2");
  byte tag_shortAddress[2];
  if(DW1000NgRTLS::receiveFrame()){
    size_t recv_len = DW1000Ng::getReceivedDataLength();
    byte recv_data[recv_len];
    DW1000Ng::getReceivedData(recv_data, recv_len);

    if(recv_data[0] == BLINK) {
      
      Serial.println("Received blink");
      // Serial.print("Tag EUI: "); Serial.print(recv_data[2]); Serial.println(recv_data[3]);
      
      // if (recv_data[2] == 1 && recv_data[3] == 1) {
      //   tag_shortAddress = tag1_shortAddress;
      //   tag1_recommendation = recv_data[12];
      // } 
      // else if (recv_data[2] == 2 && recv_data[3] == 2) {
      //   tag_shortAddress = tag2_shortAddress;
      //   tag2_recommendation = recv_data[12];
      // }
      Serial.print("Tag short address: "); Serial.print(tag_shortAddress[0]); Serial.println(tag_shortAddress[1]);
      DW1000NgRTLS::transmitRangingInitiation(&recv_data[2], tag_shortAddress);
      DW1000NgRTLS::waitForTransmission();
      // ranginginitiation 有成功
      RangeAcceptResult result = DW1000NgRTLS::anchorRangeAccept(NextActivity::RANGING_CONFIRM, next_anchor);
      if(!result.success) return;
      range_self = result.range;
      String rangeString = "Range: "; rangeString += range_self; rangeString += " m";
      rangeString += "\t RX power: "; rangeString += DW1000Ng::getReceivePower(); rangeString += " dBm distance from anchor/tag:";
      rangeString += recv_data[2]; rangeString += recv_data[3];
      rangeString += "\t Sampling: "; rangeString += samplingRate; rangeString += " Hz    Anchor:" ; rangeString  += EUI[18]; EUI[19]; rangeString += EUI[20];rangeString += EUI[21];rangeString += EUI[22];rangeString += EUI[23];   
      Serial.println(rangeString);
      uint32_t curMillis = millis();
      successRangingCount++;
    if (curMillis - rangingCountPeriod > 1000) {
        samplingRate = (1000.0f * successRangingCount) / (curMillis - rangingCountPeriod);
        rangingCountPeriod = curMillis;
        successRangingCount = 0;
    }
    } 
    else if(recv_data[9] == 0x60 /*&& recv_data[12] == tag1_recommendation*/) { 
      // Serial.print("tag1_recommendation="); Serial.println(tag1_recommendation);
      double range = static_cast<double>(DW1000NgUtils::bytesAsValue(&recv_data[10],2) / 1000.0);
      String rangeReportString = "Range from: "; rangeReportString += recv_data[7]; // anchor's device address?
      rangeReportString += " = "; rangeReportString += range;
      Serial.println(rangeReportString);
      Serial.print("distance between ");Serial.println(recv_data[12]);
      if(received_B == false && recv_data[7] == anchor_b[0] && recv_data[8] == anchor_b[1]) {
        range_B = range;
        received_B = true;
      } 
      anchorStateMachine.addSampleCount();
    }
  }
}

