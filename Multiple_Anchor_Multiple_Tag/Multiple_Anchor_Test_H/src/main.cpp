#include <uwb_common.cpp>
#include <uwb_common.hpp>

// Extended Unique Identifier register. 64-bit device identifier. Register file: 0x01
char EUI[] = "AA:BB:CC:DD:EE:FF:00:08"; 
uint16_t next_anchor = 9 ;

byte currentTagShortaddress[2];
// ranging counter (per second)
uint16_t successRangingCount[8] = {0};
uint32_t rangingCountPeriod = 0;
float samplingRate = 0;

uint16_t blink_rate = 50;
uint16_t self_device_address = 8;

double range_self;

byte currentTagEUI[8]; // Array to store the tag's EUI (8 bytes)

void ranging_flying();
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
            tagTWR(blink_rate, &EUI[0]);               
            break;
        case State::built_coord_2:
            tagTWR(blink_rate, &EUI[0]);
          break;
        case State::built_coord_3:
          tagTWR(blink_rate, &EUI[0]);
          break;
        case State::self_calibration:
          tagTWR(blink_rate, &EUI[0]);
          break;
        case State::flying:
          ranging_flying();
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
            successRangingCount[i] = 0;
          }
          startTime = millis();
          Serial.println("State changed to built_coord_2");
        }if(receivedChar == '3' && state != State::built_coord_3){
          state = State::built_coord_3;
          sample_count = 0;
          for(int i = 0; i < 8; i++){
            successRangingCount[i] = 0;
          }
          startTime = millis();
          Serial.println("State changed to built_coord_3");
        }if(receivedChar == 's' && state != State::self_calibration){
          state = State::self_calibration;
          sample_count = 0;
          for(int i = 0; i < 8; i++){
            successRangingCount[i] = 0;
          }
          startTime = millis();
          Serial.println("State changed to self_calibration");
        }if(receivedChar == 'f' && state != State::flying){
          state = State::flying;
          sample_count = 0;
          for(int i = 0; i < 8; i++){
            successRangingCount[i] = 0;
          }
          startTime = millis();
          Serial.println("State changed to flying");
        }
      }
    }
    void addSampleCount(){
      sample_count++;
    }
    State getState(){
      return state;
    }
};

void setup() {
    delay(5000);
    Serial.begin(9600);
    setupUWB(&EUI[0], self_device_address, TAG_FRAME_FILTER_CONFIG); 
    delay(5000); 
}

StateMachine anchorTagStateMachine;
void loop() {
  anchorTagStateMachine.update();
  anchorTagStateMachine.ranging();
}



void ranging_flying() {  
    RangeAcceptResult result = DW1000NgRTLS::anchorRangeAccept(NextActivity::ACTIVITY_FINISHED, blink_rate);
    if(result.success) {
        uint32_t curMillis = millis();
        delay(4); // Tweak based on your hardware
        range_self = result.range;

        // Get the tag short address from the received data
        size_t recv_len = DW1000Ng::getReceivedDataLength();
        byte recv_data[recv_len];
        DW1000Ng::getReceivedData(recv_data, recv_len);
        // memcpy(currentTagShortAddress, &recv_data[16], 2); // position: see void transmitRangingInitiation(byte tag_eui[], byte tag_short_address[]);
        memcpy(currentTagEUI, &recv_data[2], 8); // EUI starts at position 2 (assuming EUI is 8 bytes long)
        memcpy(currentTagShortaddress, &recv_data[2], 2);
   
        String rangeString = "Range: "; rangeString += range_self; rangeString += " m";
        rangeString += "\t RX power: "; rangeString += DW1000Ng::getReceivePower(); rangeString += " dBm distance between anchor/tag:";
        rangeString += recv_data[7]; rangeString += recv_data[8];
        rangeString += " from Anchor ";rangeString  += EUI[18]; rangeString += EUI[19]; rangeString += EUI[20];rangeString += EUI[21];rangeString += EUI[22];rangeString += EUI[23];
        Serial.println(rangeString);
        if(recv_data[7] == 0x01 && recv_data[8] == 0x01){
            successRangingCount[1]++;
        }
        else if ( recv_data[7] == 0x02 && recv_data[8] == 0x02){
            successRangingCount[2]++;
        }
        else if ( recv_data[7] == 0x03 && recv_data[8] == 0x03){
            successRangingCount[3]++;
        }
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


