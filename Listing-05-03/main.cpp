#include "ESP32MotorControl.h"  
ESP32MotorControl MotorControl = ESP32MotorControl();  
void setup() {
  Serial.begin (115200);       
  Serial.println ("setup mcpwm");
  MotorControl.attachMotor(13, 33);
}
void loop() {
  MotorControl.motorsStop();
  delay (500);
  for (int speed = 0; speed <= 100; speed +=10) {
    MotorControl.motorForward(0, speed);
    delay (500);
  }  
  MotorControl.motorsStop();
  delay (500);    
  for (int speed = 0; speed <= 100; speed +=10) {    
    MotorControl.motorReverse(0, speed);
    delay (500);  
  }     
}