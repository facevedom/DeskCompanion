#include "Stopwatch.h"

Stopwatch::Stopwatch(SH1106Wire *display, SharedState *globalState) : display(display), globalState(globalState) {
    
}

void Stopwatch::setup() {
    running = false;
}

void Stopwatch::loop() {
    if (globalState->queued_main_button_event == "activate stopwatch") {
        activate();
        globalState->queued_main_button_event = "";
    }
}

void Stopwatch::activate() {
    running = true;
    display->drawString(0, 40, "Stopwatch is active");
    display->display();
}
