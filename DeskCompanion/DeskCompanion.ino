#include <Arduino.h>
#include <OneButton.h>
#include <SH1106Wire.h>
#include <Wire.h>

#include "debug.h"
#include "hardwareConstants.h"
#include "src/LeverSwitch.h"
#include "src/Stopwatch.h"

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
  DEBUG_PRINT("Offline mode: ");
  DEBUG_PRINTLN(state.offline_mode);
}

void initializeDisplay() {
  display.init();
  display.flipScreenVertically();
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  DEBUG_PRINT("Display initialized");
}

void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
  #endif
  DEBUG_PRINTLN("Debugging is enabled");
  
  initializeDisplay();
  offlineModeCheck();
  setupAllRunnables();

  display.drawString(0, 0, "Desk companion started!");
  display.display();
}

void loop() {
  loopAllRunnables();
}
