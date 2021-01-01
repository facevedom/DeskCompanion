#include <Arduino.h>
#include <OneButton.h>
#include <SH1106Wire.h>
#include <Wire.h>

#include "debug.h"

void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
  #endif
  
  DEBUG_PRINT("Debugging is enabled");
}

void loop() {
}
