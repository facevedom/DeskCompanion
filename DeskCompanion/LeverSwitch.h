#ifndef SWITCH_H
#define SWITCH_H

#include <Arduino.h>

class LeverSwitch {
    private:
        byte leftPin;
        byte rightPin;
        void readPosition();

    public:
        LeverSwitch(byte leftPin, byte rightPin);
        void setup();
        void loop();
        int getPosition();
        enum Position{
            ON_LEFT,
            ON_CENTER,
            ON_RIGHT
        } position;
};

#endif
