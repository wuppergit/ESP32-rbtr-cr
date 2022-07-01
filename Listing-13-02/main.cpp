#include <Arduino.h>
#include "SparkFunLSM6DS3.h"
#include "Wire.h"
#include "SPI.h"
#include <Servo.h>              // Servo-Bibliothek
#define SERVO_PIN         13    // GPIO13 -  Servo
#define SERVO_KANAL       3
#define SERVO_MINPW       520   // default 544
#define SERVO_MAXPW       2600  // default 2400
// für LSM6DS3-Sensor
LSM6DS3 myIMU;                  // Konstructor I2C
// fuer Servo
Servo myServo;                  // Servo-Objekt
void setup() {
  Serial.begin(115200);
  delay(10); 
  // LSM6DS3 konfigurieren und starten
  if( myIMU.begin() != 0 ) {
    Serial.print("Init LSM6DS3 fehlgeschlagen!");
    while (1);
  }
  // Servo-Steuerung an SERVO_PIN
  myServo.attach(SERVO_PIN, SERVO_KANAL);
}
void loop() {
  float f, theta;  
  // hängt von Montage des Moduls ab 
  f =  map(myIMU.readFloatAccelX() * 1000, -1000, 1000, -90, 90); 
  theta = constrain(f, -90, 90);
  Serial.print ("Winkel: ");
  Serial.print  (theta);
  // Servo positionieren
  myServo.write(theta);
  Serial.print ("   Servo-Position: ");
  // Position auslesen
  Serial.println(myServo.read() );
  delay(1000);
}