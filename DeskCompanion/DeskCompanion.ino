#include <Arduino.h>
#include <OneButton.h>
#include <SH1106Wire.h>
#include <Wire.h>

#include "debug.h"
#include "display.h"
#include "hardwareConstants.h"
#include "src/LeverSwitch.h"
#include "src/Stopwatch.h"
using namespace Display;

struct sharedState {
  bool offline_mode;
} state;

SH1106Wire display(DISPLAY_ADDRESS, SDA, SCL, GEOMETRY_128_64);
LeverSwitch lever_switch(SWITCH_LEFT_PIN, SWITCH_RIGHT_PIN);
Stopwatch stopwatch(&display);

Runnable *runnables[] = {
  &lever_switch,
  &stopwatch
};

void setupAllRunnables() {
  for (int i = 0; i < sizeof(runnables) / sizeof(*runnables); i++)
    runnables[i]->setup();
  DEBUG_PRINTLN("All runnables are set");
}

void loopAllRunnables() {
  for (int i = 0; i < sizeof(runnables) / sizeof(*runnables); i++)
    runnables[i]->loop();
}

bool offlineModeCheck() {
  state.offline_mode = (lever_switch.position == LeverSwitch::ON_CENTER);
  DEBUG_PRINTLN("Offline mode: " + String(state.offline_mode));
}

void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
    DEBUG_PRINTLN("\nDesk companion debug:\n");
  #endif
  
  Display::initialize();
  setupAllRunnables();
  offlineModeCheck();

  Display::welcome();
}

void loop() {
  loopAllRunnables();
}
