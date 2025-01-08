// utils.h
#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

// Function to convert timestamp to a human-readable format
String timestampToString(uint64_t timestamp);

// Function to calculate the average of an array of floats
float calculateAverage(float* values, int length);

// Function to check if a value is within a specified range
bool isValueInRange(float value, float min, float max);

// Function to log messages to the serial console
void logMessage(const String& message);

#endif // UTILS_H