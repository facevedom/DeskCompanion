
#if !defined(nullptr)
#define nullptr NULL
#endif

#include "FTDebouncer.h"

FTDebouncer pinDebouncer(30);

void setup(){
	Serial.begin(57600);
	pinDebouncer.addPin(4, HIGH, INPUT_PULLUP);
	pinDebouncer.begin();
	Serial.print("Size of debouncer instance in bytes: ");	
	Serial.println(sizeof(pinDebouncer));
	Serial.print("Amount of debounced pins: ");
	Serial.println(pinDebouncer.getPinCount());
	delay(5000);
}
void loop(){
	unsigned long us = micros();
	unsigned long usElapsed;
	pinDebouncer.update();
	usElapsed = micros() - us;
	Serial.print("Elapsed microseconds since last debounce: ");
	Serial.println(usElapsed);
}
void onPinActivated(int pinNr){
	Serial.print("Pin activated: ");
	Serial.println(pinNr);
}
void onPinDeactivated(int pinNr){
	Serial.print("Pin deactivated: ");
	Serial.println(pinNr);
}