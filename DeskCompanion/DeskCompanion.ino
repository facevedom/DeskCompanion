// included libraries
#include <Arduino.h>
#include <OneButton.h>
#include <SH1106Wire.h>
#include <Wire.h>

// included files
#include "debug.h"
#include "display.h"
#include "SharedState.h"
#include "hardwareConstants.h"
#include "src/LeverSwitch.h"
#include "src/Stopwatch.h"

// namespaces
using namespace Display;

// the global (ew) shared state, so every object can know what is going on
SharedState state;

// instantiation of classes
SH1106Wire display(DISPLAY_ADDRESS, SDA, SCL, GEOMETRY_128_64);
LeverSwitch lever_switch(SWITCH_LEFT_PIN, SWITCH_RIGHT_PIN);
Stopwatch stopwatch(&display, &state);

// button
OneButton main_button = OneButton(
  MAIN_BUTTON_PIN, // input pin for the button
  true, // button is active LOW
  true  // enable internal pull-up resistor
);

// runnables are all classes with setup() and loop()
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

void attachMainButtonEvents() {
  main_button.attachClick([]() {
    state.queued_main_button_event = "activate stopwatch";
    DEBUG_PRINTLN("changed");
  });
}

void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
    DEBUG_PRINTLN("\nDesk companion debug:\n");
  #endif
  
  state.offline_mode = false;
  state.active_stopwatch = false;
  state.queued_main_button_event = "";

  Display::initialize();
  setupAllRunnables();
  offlineModeCheck();
  attachMainButtonEvents();
  Display::welcome();
}

void loop() {
  loopAllRunnables();
  main_button.tick();
}
