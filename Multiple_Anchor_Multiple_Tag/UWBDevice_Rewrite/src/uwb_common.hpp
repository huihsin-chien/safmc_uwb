#ifndef UWB_COMMON_HPP
#define UWB_COMMON_HPP

#include <DW1000Ng.hpp>
#include <DW1000NgUtils.hpp>
#include <DW1000NgRanging.hpp>
#include "DW1000NgRTLS_ext.hpp"

#pragma once

// connection pins
#if defined(ESP8266)
const uint8_t PIN_SS = 10;
#else
const uint8_t PIN_SS = SS; // spi select pin
const uint8_t PIN_RST = 15;
#endif

// Common configuration structures
extern device_configuration_t DEFAULT_CONFIG;
extern frame_filtering_configuration_t TAG_FRAME_FILTER_CONFIG;
extern frame_filtering_configuration_t ANCHOR_FRAME_FILTER_CONFIG;
extern sleep_configuration_t SLEEP_CONFIG;

void setupUWB(const char* EUI, uint16_t device_address, frame_filtering_configuration_t FRAME_FILTER_CONFIG);

#endif // UWB_COMMON_HPP