#include <uwb_common.cpp>
#include <uwb_common.hpp>


char EUI[] = "AA:BB:CC:DD:EE:FF:00:03";
// byte RANGING_RESPONSE = 0x60;
volatile uint32_t blink_rate = 200;
uint16_t self_device_address = 3;

void setup() {
    delay(5000);
    Serial.begin(9600);
    setupUWB(&EUI[0], self_device_address, TAG_FRAME_FILTER_CONFIG); 
    delay(5000); 
}

void loop() {
    tagTWR(blink_rate, &EUI[0]);
}
