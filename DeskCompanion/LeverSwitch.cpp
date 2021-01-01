#include "LeverSwitch.h"

LeverSwitch::LeverSwitch(byte leftPin, byte rightPin) {
    this -> leftPin = leftPin;
    this -> rightPin = rightPin;
}

void LeverSwitch::setup() {
    pinMode(leftPin, INPUT_PULLUP);
    pinMode(rightPin, INPUT_PULLUP);
    readPosition();
}

void LeverSwitch::loop() {
    readPosition();
}

void LeverSwitch::readPosition() {
    if (digitalRead(leftPin)) {
        position = digitalRead(rightPin) ? ON_CENTER : ON_LEFT;
    } else {
        position = ON_RIGHT;
    }
}

