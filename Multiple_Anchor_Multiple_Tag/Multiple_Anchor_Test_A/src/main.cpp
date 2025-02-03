#include <uwb_common.cpp>
#include <uwb_common.hpp>

char EUI[] = "AA:BB:CC:DD:EE:FF:00:01";
Position position_self = {0,0,0};

double range_self;
double range_B;
double range_C;
uint16_t self_device_address =1;

boolean received_B = false;

byte tag1_shortAddress[] = {0x01, 0x01};
byte tag2_shortAddress[] = {0x02, 0x02};
byte tag3_shortAddress[] = {0x03, 0x03};


byte anchor_b[] = {0x02, 0x00};
uint16_t next_anchor = 2;
byte anchor_c[] = {0x03, 0x00};

// ranging counter (per second)
uint16_t successRangingCount[8] = {0};
uint32_t rangingCountPeriod = 0;
float samplingRate = 0;
uint16_t blink_rate = 200;

void handleRanging_coord_1();
void handleRanging_coord_2();
void handleRanging_otherAnchor();
void handleRanging();
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
          handleRanging_coord_1();    
          break;
        case State::built_coord_2:
          handleRanging_coord_2();
          break;
        case State::self_calibration:
          handleRanging_otherAnchor();
          // handleRanging();
          break;
        case State::flying:
          handleRanging();
          break;
      }
    }
    void update(){ // sample_count haven't been implemented in ranging function
      switch(state){
        case State::built_coord_1:
          if(sample_count == 100 || (millis() - startTime) > 12000){
            state = State::built_coord_2;
            sample_count = 0;
            for(int i = 0; i < 8; i++){
              successRangingCount[i] = 0;
            }
            startTime = millis();
            Serial.println("State changed to built_coord_2");
          }
          break;
        case State::built_coord_2:
          if(sample_count == 100 || (millis() - startTime) > 50000){
            state = State::self_calibration;
            sample_count = 0;
            for(int i = 0; i < 8; i++){
              successRangingCount[i] = 0;
            }
            startTime = millis();
            Serial.println("State changed to self_calibration");
          }
          break;
        case State::self_calibration:
          if(sample_count == 100 || (millis() - startTime) > 50000){
            state = State::flying;
            sample_count = 0;
            for(int i = 0; i < 8; i++){
              successRangingCount[i] = 0;
            }
            startTime = millis();
            Serial.println("State changed to flying");
          }
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
    setupUWB(EUI, self_device_address, ANCHOR_FRAME_FILTER_CONFIG);
    delay(5000);
}

StateMachine anchorStateMachine;
void loop() {  
  anchorStateMachine.ranging();
  anchorStateMachine.update();
}


void handleRanging_coord_1() {
  byte tag_shortAddress[2];
  if(DW1000NgRTLS::receiveFrame()){

    size_t recv_len = DW1000Ng::getReceivedDataLength();
    byte recv_data[recv_len];
    DW1000Ng::getReceivedData(recv_data, recv_len);

    if(recv_data[0] == BLINK) {
      uint32_t curMillis = millis();
      // Serial.print("Tag EUI: "); Serial.print(recv_data[2]); Serial.println(recv_data[3]);

      if (recv_data[2] == 2 && recv_data[3] == 0) { // if received AnchorB blink
        tag_shortAddress[0] = (byte)0x02;
        tag_shortAddress[1] = (byte)0x00;
        successRangingCount[1]++;
      } else if (recv_data[2] == 3 && recv_data[3] == 0) { // if received AnchorC blink
        tag_shortAddress[0] = (byte)0x03;
        tag_shortAddress[1] = (byte)0x00;
        successRangingCount[2]++;
      }else{ // if received other anchor blink
        return; 
      }

      // Serial.print("Tag short address: "); Serial.print(tag_shortAddress[0]); Serial.println(tag_shortAddress[1]);
      DW1000NgRTLS::transmitRangingInitiation(&recv_data[2], tag_shortAddress);
      DW1000NgRTLS::waitForTransmission();
      // ranginginitiation 有成功
      RangeAcceptResult result = DW1000NgRTLS::anchorRangeAccept(NextActivity::ACTIVITY_FINISHED, blink_rate);
      if(!result.success) return;
      range_self = result.range;
      String rangeString = "Range: "; rangeString += range_self; rangeString += " m";
      rangeString += "\t RX power: "; rangeString += DW1000Ng::getReceivePower(); rangeString += " dBm distance between anchor/tag:";
      rangeString += recv_data[2]; rangeString += recv_data[3];
      rangeString += " from Anchor ";rangeString  += EUI[18]; rangeString += EUI[19]; rangeString += EUI[20];rangeString += EUI[21];rangeString += EUI[22];rangeString += EUI[23];
      Serial.println(rangeString);


      if (curMillis - rangingCountPeriod > 1000) {
        
        samplingRate = (1000.0f * successRangingCount[1]) / (curMillis - rangingCountPeriod);
        Serial.print("AnchorB Sampling rate: "); Serial.print(samplingRate); Serial.println(" Hz");
        samplingRate = (1000.0f * successRangingCount[2]) / (curMillis - rangingCountPeriod);
        Serial.print("AnchorC Sampling rate: "); Serial.print(samplingRate); Serial.println(" Hz");
        rangingCountPeriod = curMillis;
        successRangingCount[1] = 0;
        successRangingCount[2] = 0;
      }
    } 
  }
}
void handleRanging_coord_2() {
  byte tag_shortAddress[2];
  if(DW1000NgRTLS::receiveFrame()){

    size_t recv_len = DW1000Ng::getReceivedDataLength();
    byte recv_data[recv_len];
    DW1000Ng::getReceivedData(recv_data, recv_len);

    if(recv_data[0] == BLINK) {
      uint32_t curMillis = millis();
      // Serial.print("Tag EUI: "); Serial.print(recv_data[2]); Serial.println(recv_data[3]);

      if (recv_data[2] == 3 && recv_data[3] == 0) { // if received AnchorC blink
        tag_shortAddress[0] = (byte)0x03;
        tag_shortAddress[1] = (byte)0x00;
        successRangingCount[2]++;
      }else{
        return;
      }

      // Serial.print("Tag short address: "); Serial.print(tag_shortAddress[0]); Serial.println(tag_shortAddress[1]);
      DW1000NgRTLS::transmitRangingInitiation(&recv_data[2], tag_shortAddress);
      DW1000NgRTLS::waitForTransmission();
      // ranginginitiation 有成功
      RangeAcceptResult result = DW1000NgRTLS::anchorRangeAccept(NextActivity::RANGING_CONFIRM, next_anchor);
      if(!result.success) return;
      range_self = result.range;
      String rangeString = "Range: "; rangeString += range_self; rangeString += " m";
      rangeString += "\t RX power: "; rangeString += DW1000Ng::getReceivePower(); rangeString += " dBm distance between anchor/tag:";
      rangeString += recv_data[2]; rangeString += recv_data[3];
      rangeString += " from Anchor ";rangeString  += EUI[18]; rangeString += EUI[19]; rangeString += EUI[20];rangeString += EUI[21];rangeString += EUI[22];rangeString += EUI[23];
      Serial.println(rangeString);


      if (curMillis - rangingCountPeriod > 1000) {
        
        samplingRate = (1000.0f * successRangingCount[2]) / (curMillis - rangingCountPeriod);
        Serial.print("AnchorC Sampling rate: "); Serial.print(samplingRate); Serial.println(" Hz");
        rangingCountPeriod = curMillis;
        successRangingCount[2] = 0;
      }
    } 
  }
}
void handleRanging_otherAnchor(){ 
  
  byte tag_shortAddress[2];
  if(DW1000NgRTLS::receiveFrame()){
    // Serial.println("let's go~");
    size_t recv_len = DW1000Ng::getReceivedDataLength();
    byte recv_data[recv_len];
    DW1000Ng::getReceivedData(recv_data, recv_len);

    if(recv_data[0] == BLINK) {
      uint32_t curMillis = millis();
      // Serial.print("Tag EUI: "); Serial.print(recv_data[2]); Serial.println(recv_data[3]);

      if (recv_data[2] == 4 && recv_data[3] == 0) { // if received AnchorD blink
        tag_shortAddress[0] = (byte)0x04;
        tag_shortAddress[1] = (byte)0x00;
        successRangingCount[3]++;
      } else if (recv_data[2] == 5 && recv_data[3] == 0) { // if received AnchorE blink
        tag_shortAddress[0] = (byte)0x05;
        tag_shortAddress[1] = (byte)0x00;
        successRangingCount[4]++;
      }else if (recv_data[2] == 6 && recv_data[3] == 0) { // if received AnchorF blink
        tag_shortAddress[0] = (byte)0x06;
        tag_shortAddress[1] = (byte)0x00;
        successRangingCount[5]++;
      } else if (recv_data[2] == 7 && recv_data[3] == 0) { // if received AnchorG blink
        tag_shortAddress[0] = (byte)0x07;
        tag_shortAddress[1] = (byte)0x00;
        successRangingCount[6]++;
      }else if (recv_data[2] == 8 && recv_data[3] == 0) { // if received AnchorH blink
        tag_shortAddress[0] = (byte)0x08;
        tag_shortAddress[1] = (byte)0x00;
        successRangingCount[7]++;}

      // Serial.print("Tag short address: "); Serial.print(tag_shortAddress[0]); Serial.println(tag_shortAddress[1]);
      DW1000NgRTLS::transmitRangingInitiation(&recv_data[2], tag_shortAddress);
      DW1000NgRTLS::waitForTransmission();
      // ranginginitiation 有成功
      RangeAcceptResult result = DW1000NgRTLS::anchorRangeAccept(NextActivity::RANGING_CONFIRM, next_anchor);
      if(!result.success) return;
      range_self = result.range;
      String rangeString = "Range: "; rangeString += range_self; rangeString += " m";
      rangeString += "\t RX power: "; rangeString += DW1000Ng::getReceivePower(); rangeString += " dBm distance between anchor/tag:";
      rangeString += recv_data[2]; rangeString += recv_data[3];
      rangeString += " from Anchor ";rangeString  += EUI[18]; rangeString += EUI[19]; rangeString += EUI[20];rangeString += EUI[21];rangeString += EUI[22];rangeString += EUI[23];
      Serial.println(rangeString);

      if (curMillis - rangingCountPeriod > 1000) {
        samplingRate = (1000.0f * successRangingCount[3]) / (curMillis - rangingCountPeriod);
        Serial.print("Sampling rate AnchorD: "); Serial.print(samplingRate); Serial.println(" Hz");
        samplingRate = (1000.0f * successRangingCount[4]) / (curMillis - rangingCountPeriod);
        Serial.print("Sampling rate AnchorE: "); Serial.print(samplingRate); Serial.println(" Hz");
        samplingRate = (1000.0f * successRangingCount[5]) / (curMillis - rangingCountPeriod);
        Serial.print("Sampling rate AnchorF: "); Serial.print(samplingRate); Serial.println(" Hz");
        samplingRate = (1000.0f * successRangingCount[6]) / (curMillis - rangingCountPeriod);
        Serial.print("Sampling rate AnchorG: "); Serial.print(samplingRate); Serial.println(" Hz");
        samplingRate = (1000.0f * successRangingCount[7]) / (curMillis - rangingCountPeriod);
        Serial.print("Sampling rate AnchorH: "); Serial.print(samplingRate); Serial.println(" Hz");
        rangingCountPeriod = curMillis;
        successRangingCount[4] = 0;
        successRangingCount[5] = 0;
        successRangingCount[6] = 0;
        successRangingCount[7] = 0;
        successRangingCount[8] = 0;
      }
    } 
  }
}
void handleRanging() { // for tag, need to be modified

  byte tag_shortAddress[2];
  if(DW1000NgRTLS::receiveFrame()){
    // Serial.println("let's go~");
    size_t recv_len = DW1000Ng::getReceivedDataLength();
    byte recv_data[recv_len];
    DW1000Ng::getReceivedData(recv_data, recv_len);

    if(recv_data[0] == BLINK) {
      uint32_t curMillis = millis();
      // Serial.print("Tag EUI: "); Serial.print(recv_data[2]); Serial.println(recv_data[3]);

      if (recv_data[2] == 0 && recv_data[3] == 0) { // if received tag1 blink
        tag_shortAddress[0] = tag1_shortAddress[0];
        tag_shortAddress[1] = tag1_shortAddress[1];
        successRangingCount[1]++;
      } else if (recv_data[2] == 2 && recv_data[3] == 2) { // if received tag2 blink
        tag_shortAddress[0] = tag2_shortAddress[0];
        tag_shortAddress[1] = tag2_shortAddress[1];
        successRangingCount[2]++;
      }else if (recv_data[2] == 3 && recv_data[3] == 3) {  // if received tag3 blink
        tag_shortAddress[0] = tag3_shortAddress[0];
        tag_shortAddress[1] = tag3_shortAddress[1];
        successRangingCount[3]++;
      }

      // Serial.print("Tag short address: "); Serial.print(tag_shortAddress[0]); Serial.println(tag_shortAddress[1]);
      DW1000NgRTLS::transmitRangingInitiation(&recv_data[2], tag_shortAddress);
      DW1000NgRTLS::waitForTransmission();
      // ranginginitiation 有成功
      RangeAcceptResult result = DW1000NgRTLS::anchorRangeAccept(NextActivity::RANGING_CONFIRM, next_anchor);
      if(!result.success) return;
      range_self = result.range;
      // Serial.println("Data from tag: ");
      // for (uint16_t i = 0; i < 18; i++) {
      //   Serial.print(" 0x"); Serial.print(recv_data[i], HEX);
      // }
      String rangeString = "Range: "; rangeString += range_self; rangeString += " m";
      rangeString += "\t RX power: "; rangeString += DW1000Ng::getReceivePower(); rangeString += " dBm distance between anchor/tag:";
      rangeString += recv_data[2]; rangeString += recv_data[3];
      rangeString += " from Anchor ";rangeString  += EUI[18]; rangeString += EUI[19]; rangeString += EUI[20];rangeString += EUI[21];rangeString += EUI[22];rangeString += EUI[23];
      Serial.println(rangeString);

      if (curMillis - rangingCountPeriod > 1000) {
        
        samplingRate = (1000.0f * successRangingCount[1]) / (curMillis - rangingCountPeriod);
        Serial.print("Sampling rate 1: "); Serial.print(samplingRate); Serial.println(" Hz");
        samplingRate = (1000.0f * successRangingCount[2]) / (curMillis - rangingCountPeriod);
        Serial.print("Sampling rate 2: "); Serial.print(samplingRate); Serial.println(" Hz");
        samplingRate = (1000.0f * successRangingCount[3]) / (curMillis - rangingCountPeriod);
        Serial.print("Sampling rate 3: "); Serial.print(samplingRate); Serial.println(" Hz");
          rangingCountPeriod = curMillis;
          successRangingCount[1] = 0;
          successRangingCount[2] = 0;
          successRangingCount[3] = 0;
      }
    } 

  }
}
