#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <Arduino.h>
#include <SH1106Wire.h>
#include "Runnable.h"
#include "../SharedState.h"

class Stopwatch: public Runnable {
    private:
        SH1106Wire *display;
        SharedState *globalState;
        bool running;

    public:
        Stopwatch(SH1106Wire *display, SharedState *globalState);
        void setup();
        void loop();
        void activate();
};

#endif
