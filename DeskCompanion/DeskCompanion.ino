#include <Arduino.h>
#include <OneButton.h>
#include <SH1106Wire.h>
#include <Wire.h>

#include "debug.h"
#include "LeverSwitch.h"

#define SWITCH_LEFT_PIN 18
#define SWITCH_RIGHT_PIN 19

struct sharedState {
  bool offlineMode;
} state;

LeverSwitch leverSwitch(SWITCH_LEFT_PIN, SWITCH_RIGHT_PIN);

bool offlineModeCheck() {
  if (leverSwitch.position == LeverSwitch::ON_CENTER) {
    state.offlineMode = true;
  } else {
    state.offlineMode = false;
  }

  DEBUG_PRINT("Offline mode: ");
  DEBUG_PRINTLN(state.offlineMode);
}

void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
  #endif
  DEBUG_PRINTLN("Debugging is enabled");

  leverSwitch.setup();

  offlineModeCheck();
}

void loop() {
  leverSwitch.loop();
}
