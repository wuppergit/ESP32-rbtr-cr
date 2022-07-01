#include <Arduino.h>
int ledPin = 2;
int delayPeriod = 1000;
bool ledStatus = false;
int globalLoopCounter = 0;
void toggleLED (int pin, int delPe) {
  int functionCounter = 0;
  if (ledStatus)  digitalWrite(pin, HIGH);
  else            digitalWrite(pin, LOW); 
  delay(delPe);  
  functionCounter++;
  Serial.print   ("functionCounter:   ");
  Serial.println (functionCounter);
}
void setup() {
  Serial.begin (115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  Serial.println ("Start");  
}  
void loop() {
  int localLoopCounter = 0;
  toggleLED (ledPin, delayPeriod);
  ledStatus =! ledStatus;
  localLoopCounter++;
  globalLoopCounter++;
  Serial.print   ("localLoopCounter:  ");
  Serial.println (localLoopCounter);
  Serial.print   ("globalLoopCounter: ");
  Serial.println (globalLoopCounter);
}