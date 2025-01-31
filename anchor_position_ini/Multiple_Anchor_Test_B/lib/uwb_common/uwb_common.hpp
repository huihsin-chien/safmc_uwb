#ifndef UWB_COMMON_HPP
#define UWB_COMMON_HPP

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

// Struct to hold position coordinates
typedef struct Position {
    double x;
    double y;
} Position;
byte main_anchor_address[] = {0x01, 0x00};

// Enum class for managing states
enum class State {
    built_coord_1,
    built_coord_2,
    self_calibration
};

// Common configuration structures
extern device_configuration_t DEFAULT_CONFIG;
extern frame_filtering_configuration_t TAG_FRAME_FILTER_CONFIG;
extern frame_filtering_configuration_t ANCHOR_FRAME_FILTER_CONFIG;
extern sleep_configuration_t SLEEP_CONFIG;

void setupUWB(const char* EUI, uint16_t device_address);
void handleRanging(uint32_t& range_self, uint8_t* currentTagShortaddress);
void tagTWR(uint32_t blink_rate);

#endif // UWB_COMMON_HPP