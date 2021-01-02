#include <Arduino.h>
#include <OneButton.h>
#include <SH1106Wire.h>
#include <Wire.h>

#include "debug.h"
#include "LeverSwitch.h"

#define SWITCH_LEFT_PIN 18
#define SWITCH_RIGHT_PIN 19

struct sharedState {
  bool offline_mode;
} state;

LeverSwitch lever_switch(SWITCH_LEFT_PIN, SWITCH_RIGHT_PIN);

Runnable *runnables[] = {
  &lever_switch
};

bool offline_modeCheck() {
  if (lever_switch.position == LeverSwitch::ON_CENTER) {
    state.offline_mode = true;
  } else {
    state.offline_mode = false;
  }

  DEBUG_PRINT("Offline mode: ");
  DEBUG_PRINTLN(state.offline_mode);
}

void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
  #endif
  DEBUG_PRINTLN("Debugging is enabled");

  for (int i = 0; i < sizeof(runnables) / sizeof(*runnables); i++)
    runnables[i]->setup();

  offline_modeCheck();
}

void loop() {
  for (int i = 0; i < sizeof(runnables) / sizeof(*runnables); i++)
    runnables[i]->loop();
}
