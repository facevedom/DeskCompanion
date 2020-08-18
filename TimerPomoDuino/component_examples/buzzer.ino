const int buzzer = 5; //buzzer to arduino pin 9


void setup(){
 
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output

}

void loop(){
  tone(buzzer, 1000); // Send 1KHz sound signal...
  delay(500);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
  delay(500);        // ...for 1sec
  tone(buzzer, 500); // Send 1KHz sound signal...
  delay(500);        // ...for 1 sec
}