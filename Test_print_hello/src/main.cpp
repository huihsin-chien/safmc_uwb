#include <Arduino.h>
unsigned long previousMillis;  // 記錄前一次的時間
long interval;    // 設定間隔時間，1000ms
char chr;

void setup() {
  Serial.begin(9600);
  previousMillis = 0;
  interval = 1000;
}

void loop() {
  unsigned long currentMillis = millis();
  // Serial.println(currentMillis);
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    Serial.print("hello!");
    Serial.println(currentMillis);
  }
}
