#include "utils.h"

void delayMilliseconds(uint32_t milliseconds) {
    delay(milliseconds);
}

uint64_t getCurrentTimestamp() {
    return millis();
}

void copyData(const byte* source, byte* destination, size_t length) {
    memcpy(destination, source, length);
}

float convertToFloat(const byte* data) {
    float value;
    memcpy(&value, data, sizeof(float));
    return value;
}

void printDebugMessage(const String& message) {
    Serial.println(message);
}