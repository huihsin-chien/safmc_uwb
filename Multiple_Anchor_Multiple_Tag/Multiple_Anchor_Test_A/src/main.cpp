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
byte tag4_shortAddress[] = {0x04, 0x04};
byte tag5_shortAddress[] = {0x05, 0x05};
byte tag6_shortAddress[] = {0x06, 0x06};
byte tag7_shortAddress[] = {0x07, 0x07};
byte tag8_shortAddress[] = {0x08, 0x08};
byte tag9_shortAddress[] = {0x09, 0x09};
byte tag10_shortAddress[] = {0x10, 0x10};

byte anchor_b[] = {0x02, 0x00};
uint16_t next_anchor = 2;
uint16_t next_anchor_count = 2;
byte anchor_c[] = {0x03, 0x00};

// ranging counter (per second)
uint32_t rangingCountPeriod = 0;
float samplingRate = 0;
uint16_t blink_rate = 50;

void handleRanging_coord_1();
void handleRanging_coord_2();
void handleRanging_coord_3();
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
        case State::built_coord_3:
          handleRanging_coord_3();
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
    void update(){ 
      //為了防止anchor沒收到state change的指令，所以每一秒傳送一次current state
      char receivedChar = ' ';
      if(Serial.available()>0){        
        receivedChar = Serial.read(); //讀取字元
        Serial.println(receivedChar); //打印出字元
        if(receivedChar == '2' && state != State::built_coord_2){
          state = State::built_coord_2;
          sample_count = 0;
          for(int i = 0; i < 8; i++){
          }
          startTime = millis();
          Serial.println("State changed to built_coord_2");
        }if(receivedChar == '3' && state != State::built_coord_3){
          state = State::built_coord_3;
          sample_count = 0;
          for(int i = 0; i < 8; i++){
          }
          startTime = millis();
          Serial.println("State changed to built_coord_3");
        }if(receivedChar == 's' && state != State::self_calibration){
          state = State::self_calibration;
          sample_count = 0;
          for(int i = 0; i < 8; i++){
          }
          startTime = millis();
          Serial.println("State changed to self_calibration");
        }if(receivedChar == 'f' && state != State::flying ){
          state = State::flying;
          sample_count = 0;
          for(int i = 0; i < 8; i++){
          }
          startTime = millis();
          Serial.println("State changed to flying");
        }
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
      } else if (recv_data[2] == 3 && recv_data[3] == 0) { // if received AnchorC blink
        tag_shortAddress[0] = (byte)0x03;
        tag_shortAddress[1] = (byte)0x00;
      }else if (recv_data[2] == 4 && recv_data[3] == 0) { // if received AnchorD blink
        tag_shortAddress[0] = (byte)0x04;
        tag_shortAddress[1] = (byte)0x00;
      } 
      else{ // if received other anchor blink
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
      }else if (recv_data[2] == 4 && recv_data[3] == 0) { // if received AnchorD blink
        tag_shortAddress[0] = (byte)0x04;
        tag_shortAddress[1] = (byte)0x00;
      }
      else{
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

    } 
  }
}
void handleRanging_coord_3() {
  byte tag_shortAddress[2];
  if(DW1000NgRTLS::receiveFrame()){

    size_t recv_len = DW1000Ng::getReceivedDataLength();
    byte recv_data[recv_len];
    DW1000Ng::getReceivedData(recv_data, recv_len);

    if(recv_data[0] == BLINK) {
      uint32_t curMillis = millis();
      // Serial.print("Tag EUI: "); Serial.print(recv_data[2]); Serial.println(recv_data[3]);
      if (recv_data[2] == 4 && recv_data[3] == 0) { // if received AnchorD blink
        tag_shortAddress[0] = (byte)0x04;
        tag_shortAddress[1] = (byte)0x00;
      }
      else{
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

      if (recv_data[2] == 5 && recv_data[3] == 0) { // if received AnchorE blink
        tag_shortAddress[0] = (byte)0x05;
        tag_shortAddress[1] = (byte)0x00;
      }else if (recv_data[2] == 6 && recv_data[3] == 0) { // if received AnchorF blink
        tag_shortAddress[0] = (byte)0x06;
        tag_shortAddress[1] = (byte)0x00;
      } else if (recv_data[2] == 7 && recv_data[3] == 0) { // if received AnchorG blink
        tag_shortAddress[0] = (byte)0x07;
        tag_shortAddress[1] = (byte)0x00;
      }else if (recv_data[2] == 8 && recv_data[3] == 0) { // if received AnchorH blink
        tag_shortAddress[0] = (byte)0x08;
        tag_shortAddress[1] = (byte)0x00;}

      // Serial.print("Tag short address: "); Serial.print(tag_shortAddress[0]); Serial.println(tag_shortAddress[1]);
      DW1000NgRTLS::transmitRangingInitiation(&recv_data[2], tag_shortAddress);
      DW1000NgRTLS::waitForTransmission();
      
      next_anchor_count ++;
      next_anchor_count = next_anchor_count%3;
      
      RangeAcceptResult result = DW1000NgRTLS::anchorRangeAccept(NextActivity::RANGING_CONFIRM, next_anchor_count+ 2);
      if(!result.success) return;
      range_self = result.range;
      String rangeString = "Range: "; rangeString += range_self; rangeString += " m";
      rangeString += "\t RX power: "; rangeString += DW1000Ng::getReceivePower(); rangeString += " dBm distance between anchor/tag:";
      rangeString += recv_data[2]; rangeString += recv_data[3];
      rangeString += " from Anchor ";rangeString  += EUI[18]; rangeString += EUI[19]; rangeString += EUI[20];rangeString += EUI[21];rangeString += EUI[22];rangeString += EUI[23];
      Serial.println(rangeString);

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
      } else if (recv_data[2] == 2 && recv_data[3] == 2) { // if received tag2 blink
        tag_shortAddress[0] = tag2_shortAddress[0];
        tag_shortAddress[1] = tag2_shortAddress[1];
      }else if (recv_data[2] == 3 && recv_data[3] == 3) {  // if received tag3 blink
        tag_shortAddress[0] = tag3_shortAddress[0];
        tag_shortAddress[1] = tag3_shortAddress[1];
      }else if (recv_data[2] == 4 && recv_data[3] == 4) {  // if received tag4 blink
        tag_shortAddress[0] = tag4_shortAddress[0];
        tag_shortAddress[1] = tag4_shortAddress[1];
      }else if (recv_data[2] == 5 && recv_data[3] == 5) {  // if received tag5 blink
        tag_shortAddress[0] = tag5_shortAddress[0];
        tag_shortAddress[1] = tag5_shortAddress[1];
      }else if (recv_data[2] == 6 && recv_data[3] == 6) {  // if received tag6 blink
        tag_shortAddress[0] = tag6_shortAddress[0];
        tag_shortAddress[1] = tag6_shortAddress[1];
      }else if (recv_data[2] == 7 && recv_data[3] == 7) {  // if received tag7 blink
        tag_shortAddress[0] = tag7_shortAddress[0];
        tag_shortAddress[1] = tag7_shortAddress[1];
      }else if (recv_data[2] == 8 && recv_data[3] == 8) {  // if received tag8 blink
        tag_shortAddress[0] = tag8_shortAddress[0];
        tag_shortAddress[1] = tag8_shortAddress[1];
      }else if (recv_data[2] == 9 && recv_data[3] == 9) {  // if received tag9 blink
        tag_shortAddress[0] = tag9_shortAddress[0];
        tag_shortAddress[1] = tag9_shortAddress[1];
      }else if (recv_data[2] == 10 && recv_data[3] == 10) {  // if received tag10 blink
        tag_shortAddress[0] = tag10_shortAddress[0];
        tag_shortAddress[1] = tag10_shortAddress[1];
      }
      
      else{
        Serial.println("Not a correct tag blink");
        return;
      }

      // Serial.print("Tag short address: "); Serial.print(tag_shortAddress[0]); Serial.println(tag_shortAddress[1]);
      DW1000NgRTLS::transmitRangingInitiation(&recv_data[2], tag_shortAddress);
      DW1000NgRTLS::waitForTransmission();
      // ranginginitiation 有成功


      next_anchor_count ++;
      next_anchor_count = next_anchor_count%3;

      RangeAcceptResult result = DW1000NgRTLS::anchorRangeAccept(NextActivity::RANGING_CONFIRM, next_anchor_count+ 2);
      if(!result.success) return;
      range_self = result.range;

      String rangeString = "Range: "; rangeString += range_self; rangeString += " m";
      rangeString += "\t RX power: "; rangeString += DW1000Ng::getReceivePower(); rangeString += " dBm distance between anchor/tag:";
      rangeString += recv_data[2]; rangeString += recv_data[3];
      rangeString += " from Anchor ";rangeString  += EUI[18]; rangeString += EUI[19]; rangeString += EUI[20];rangeString += EUI[21];rangeString += EUI[22];rangeString += EUI[23];
      Serial.println(rangeString);
      
    } 
  }
}