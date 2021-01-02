#ifndef SWITCH_H
#define SWITCH_H

#include <Arduino.h>
#include "Runnable.h"

class LeverSwitch: public Runnable {
    private:
        byte leftPin;
        byte rightPin;
        void readPosition();

    public:
        LeverSwitch(byte leftPin, byte rightPin);
        void setup();
        void loop();
        enum Position{
            ON_LEFT,
            ON_CENTER,
            ON_RIGHT
        } position;
};

#endif
