#include "Arduino.h"

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("hello, world!");
}

void loop() {
  Serial.println("loop running...");
  delay(1000);
}