const int redPin = 8;
const int greenPin = 10;
const int bluePin = 12;
 
void setup() {}
 
void loop() {
  analogWrite(redPin, random(0,255));
  analogWrite(greenPin, random(0,255));
  analogWrite(bluePin, random(0,255));
  delay(500);
}