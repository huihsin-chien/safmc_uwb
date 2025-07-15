#ifndef CONFIG_HPP
#define CONFIG_HPP
#pragma once
#endif // CONFIG_HPP

#define TAG_1
bool AUTO_RESTART = false;


#include <DW1000NgUtils.hpp>
#include <DW1000NgRanging.hpp>
#include "DW1000NgRTLS_ext.hpp"

// == START OF Device Config ==

// const char becomeTagSuccessMessage[] = "";
// const char becomeAnchorSuccessMessage[] = "";

#ifdef ANCHOR_1
const char EUI[] = "AA:BB:CC:DD:EE:FF:00:01";
const uint16_t self_device_address = 0x0001;
const bool isAnchorByDefault = true;
const char becomeTagSymbols[] = "";
const char becomeAnchorSymbols[] = "";

#endif


#if defined(TAG_1) || defined(TAG_2) || defined(TAG_3) || defined(TAG_4) \
    || defined(TAG_5) || defined(TAG_6) || defined(TAG_7) || defined(TAG_8) \
    || defined(TAG_9) || defined(TAG_10)
const bool isAnchorByDefault = false;
const char becomeTagSymbols[] = "";
const char becomeAnchorSymbols[] = "";
#endif


#ifdef TAG_1
const char EUI[] = "AA:BB:CC:DD:EE:FF:01:01";
const uint16_t self_device_address = 0x0101;
#endif

#ifdef TAG_2
const char EUI[] = "AA:BB:CC:DD:EE:FF:02:02";
const uint16_t self_device_address = 0x0202;
#endif

// ==  END OF Device Config  ==


#ifdef FIXED_BLINK_RATE
uint32_t blink_rate = FIXED_BLINK_RATE;
#else
uint32_t blink_rate = 50;
#endif