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
    Channel::CHANNEL_5,
    DataRate::RATE_850KBPS,
    PulseFrequency::FREQ_16MHZ,
    PreambleLength::LEN_256,
    PreambleCode::CODE_3
};

frame_filtering_configuration_t TAG_FRAME_FILTER_CONFIG = {
    false, false, true, false, false, false, false, false
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

char EUI[] = "AA:BB:CC:DD:EE:FF:00:02";
byte currentTagShortaddress[2];
byte main_anchor_address[] = {0x01, 0x00};
uint32_t blink_rate = 200;
double range_self;
// byte currentTagEUI[8]; // Array to store the tag's EUI (8 bytes)

// ranging counter (per second)
uint16_t successRangingCount = 0;
uint32_t rangingCountPeriod = 0;
float samplingRate = 0;

enum class State{
  built_coord_1,
  built_coord_2,
  self_calibration
};

void tagTWR();
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
            tagTWR();
            // handleRanging_coord_2();    

          break;
        case State::built_coord_2:
            handleRanging_coord_2();   
            // tagTWR(); 
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
    DW1000Ng::applySleepConfiguration(SLEEP_CONFIG);
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

    DW1000Ng::enableDebounceClock();
    DW1000Ng::enableLedBlinking();
    DW1000Ng::setGPIOMode(6, LED_MODE);
    DW1000Ng::setGPIOMode(8, LED_MODE);
    DW1000Ng::setGPIOMode(10, LED_MODE);
    DW1000Ng::setGPIOMode(12,   LED_MODE);
    delay(5000); 
}

StateMachine anchorTagStateMachine;

void printMemoryUsage() {
    Serial.print("Free heap: ");
    Serial.println(ESP.getFreeHeap());
    Serial.print("Free PSRAM: ");
    Serial.println(ESP.getFreePsram());
}
void tagTWR(){
  Serial.println("let's go~");
  // printMemoryUsage(); // Add this line to check memory usage
  DW1000Ng::deepSleep();
  delay(blink_rate);
  DW1000Ng::spiWakeup();
  RangeInfrastructureResult res = DW1000NgRTLS::tagTwrLocalize(1500);
  if(res.success){
      Serial.println("result is right!");
      blink_rate = res.new_blink_rate;
  }
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
#include <Arduino.h>


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
