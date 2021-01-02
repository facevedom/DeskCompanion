#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <Arduino.h>
#include <SH1106Wire.h>
#include "Runnable.h"

class Stopwatch: public Runnable {
    private:
        SH1106Wire *display;

    public:
        Stopwatch(SH1106Wire *display);
        void setup();
        void loop();
};

#endif
