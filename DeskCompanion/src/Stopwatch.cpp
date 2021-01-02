#include "Stopwatch.h"
#include <SH1106Wire.h>

Stopwatch::Stopwatch(SH1106Wire *display) : display(display) {

}

void Stopwatch::setup() {
    display->drawString(0, 40, "Stopwatch");
}

void Stopwatch::loop() {

}
