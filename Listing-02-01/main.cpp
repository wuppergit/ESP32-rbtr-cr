#include <Arduino.h>
void setup() {
  Serial.begin (115200);
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  Serial.println ("Start");  
}
void loop() {
  digitalWrite(2, HIGH);
  Serial.println ("high");
  delay(500);
  digitalWrite(2, LOW);
  Serial.println ("low");
  delay(500);
}