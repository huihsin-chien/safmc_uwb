#include "uwb_common.hpp"

const char EUI[] = "AA:BB:CC:DD:EE:FF:02:02";
const uint16_t self_device_address = 0x0202;

void getAnchorRangeReport();

void setup() {
    Serial.begin(9600);
    setupUWB(&EUI[0], self_device_address, ANCHOR_FRAME_FILTER_CONFIG);
}

void loop() {
    try{
        getAnchorRangeReport();
    }
    catch(const std::exception& e){
    }
}

void getAnchorRangeReport(){
    double range;
    if(!DW1000NgRTLS_ext::receiveFrame()){
        Serial.println("no frame recieved");
        return;
    }else{
        size_t poll_len = DW1000Ng::getReceivedDataLength();
        byte poll_data[poll_len];
        DW1000Ng::getReceivedData(poll_data, poll_len);

        if(poll_len > 9 && poll_data[9] == RANGING_TAG_POLL){
        // uint64_t timePollReceived = DW1000Ng::getReceiveTimestamp();

        // 印出距離訊息
        char rangeCharArr[256];
        snprintf(rangeCharArr, 256, "anchor_range,%x%x%x.%x%x,\ntagEUI: %x:%x,\nanchorEUI: %x:%x,\nRX_power: -%x%x%x.%x%x dBm\n", 
            poll_data[12], poll_data[13], poll_data[14], poll_data[15], poll_data[16], 
            poll_data[10], poll_data[11], 
            poll_data[7],poll_data[8], 
            poll_data[17], poll_data[18], poll_data[19], poll_data[20], poll_data[21]);
        Serial.println(rangeCharArr);
        }
    }

}

