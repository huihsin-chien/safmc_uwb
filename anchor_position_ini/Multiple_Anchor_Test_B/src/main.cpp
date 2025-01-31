#include <uwb_common.cpp>
#include <uwb_common.hpp>

char EUI[] = "AA:BB:CC:DD:EE:FF:00:02";
byte currentTagShortaddress[2];
uint32_t blink_rate = 200;
double range_self;
uint16_t self_device_address = 2;

// ranging counter (per second)
uint16_t successRangingCount = 0;
uint32_t rangingCountPeriod = 0;
float samplingRate = 0;

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
          tagTWR(blink_rate, &EUI[0]);   
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
            DW1000Ng::spiWakeup();
            DW1000Ng::setDeviceAddress(2);
            Serial.println("change to built_coord_2");
          }
          break;
        case State::built_coord_2:
          break;
        case State::self_calibration:
          break;
      }
    }
    void addSampleCount(){
      sample_count++;
    }
    State getState(){
      return state;
    }
};

StateMachine anchorTagStateMachine;

void setup() {
    delay(5000);
    Serial.begin(9600);
    setupUWB(&EUI[0], self_device_address, TAG_FRAME_FILTER_CONFIG); // 2 is the device address of the anchorB
    delay(5000); 
  }

void transmitRangeReport() {
  byte rangingReport[] = {DATA, SHORT_SRC_AND_DEST, DW1000NgRTLS::increaseSequenceNumber(), 0,0, 0,0, 0,0, 0x60, 0,0 };
  DW1000Ng::getNetworkId(&rangingReport[3]);
  memcpy(&rangingReport[5], main_anchor_address, 2);
  DW1000Ng::getDeviceAddress(&rangingReport[7]);
  DW1000NgUtils::writeValueToBytes(&rangingReport[10], static_cast<uint16_t>((range_self*1000)), 2);
  memcpy(&rangingReport[16], currentTagShortaddress, 2);
  DW1000Ng::setTransmitData(rangingReport, sizeof(rangingReport));
  DW1000Ng::startTransmit();
  Serial.println("Transmitting range report");
}
void handleRanging_coord_2(){
  RangeAcceptResult result = DW1000NgRTLS::anchorRangeAccept(NextActivity::ACTIVITY_FINISHED, 200);
  // Serial.println("handleRanging_coord_2");
  int32_t curMillis = millis();

  // printMemoryUsage(); // Add this line to check memory usage
  if(result.success) {
    Serial.println("anchorRangeAccept!");
    delay(2);
    range_self = result.range;
    size_t recv_len = DW1000Ng::getReceivedDataLength();
    byte recv_data[recv_len];
    DW1000Ng::getReceivedData(recv_data, recv_len);
    memcpy(currentTagShortaddress, &recv_data[2], 2);
    transmitRangeReport();


    String rangeString = "Range: "; rangeString += range_self; rangeString += " m";
    rangeString += "\t RX power: "; rangeString += DW1000Ng::getReceivePower(); rangeString += " dBm from ";
    rangeString += recv_data[2]; rangeString += recv_data[3];
    rangeString += "\t Sampling: "; rangeString += samplingRate; rangeString += " Hz";
    Serial.println(rangeString);
    successRangingCount++;
    if (curMillis - rangingCountPeriod > 1000) {
        samplingRate = (1000.0f * successRangingCount) / (curMillis - rangingCountPeriod);
        rangingCountPeriod = curMillis;
        successRangingCount = 0;
    }
  }
}

void loop() {
  anchorTagStateMachine.update();
  anchorTagStateMachine.ranging();
}
