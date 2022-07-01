#include <Arduino.h>
#include "Servo.h"                    // Servo-Bibliothek
#define SERVO_PIN       5             // GPIO5 -  Servo-Motor´
#define SERVO_MINPW     520           // default 544
#define SERVO_MAXPW     2600          // default 2400
Servo myServo;                        // Servo-Objekt erstellen
void setup() {
  Serial.begin (115200);    
  Serial.println(); 
  myServo.attach(SERVO_PIN);          // Servo-Steuerung an SERVO_PIN
  //myServo.attach(SERVO_PIN,SERVO_MINPW,SERVO_MAXPW);         
  Serial.println("start"); 
}
void loop() {
  int i;
  // Position in Grad über Seriellen Monitor eingeben 
  if (Serial.available() > 0) {
    i = Serial.readString().toInt();    
    Serial.print ("Eingabe :");        
    Serial.println(i); 
    myServo.write(i);                 // Servo positionieren
    Serial.print ("Servo-Position: "); 
    Serial.println(myServo.read() );  // Position auslesen
    Serial.print ("aktuelle Pulsweite in Microsekunnden: ");  
    Serial.println(myServo.readMicroseconds() );                
    delay (1000);
  }
}