#include <Arduino.h>
#include <OneButton.h>
#include <SH1106Wire.h>
#include <Wire.h>

#include "debug.h"
#include "LeverSwitch.h"

#define SWITCH_LEFT_PIN 18
#define SWITCH_RIGHT_PIN 19

LeverSwitch leverSwitch(SWITCH_LEFT_PIN, SWITCH_RIGHT_PIN);

void checkFor

void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
  #endif
  DEBUG_PRINTLN("Debugging is enabled");

  leverSwitch.setup();
}

void loop() {
  leverSwitch.loop();
  delay(500);

  switch (leverSwitch.position) {
    case LeverSwitch::ON_LEFT:
    DEBUG_PRINTLN("at left");
    break;

    case LeverSwitch::ON_CENTER:
    DEBUG_PRINTLN("at center");
    break;

    case LeverSwitch::ON_RIGHT:
    DEBUG_PRINTLN("at right");
    break;
  }

}
