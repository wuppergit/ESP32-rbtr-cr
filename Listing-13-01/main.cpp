#include <Arduino.h>
#include "SparkFunLSM6DS3.h"
#include "Wire.h"
#include "SPI.h"
LSM6DS3 myIMU;     // Konstructor I2C (default), 0x6B
void setup() {   
  Serial.begin(115200);
  delay(10); 
  // LSM6DS3 konfigurieren und starten
  if( myIMU.begin() != 0 ) {
    Serial.print("Init LSM6DS3 fehlgeschlagen!");
    while (1);
  }
  Serial.print("AccX");
  Serial.print(",");
  Serial.print("AccY");
  Serial.print(",");
  Serial.print("AccZ");
  Serial.print(",");
  Serial.print("GyroX");
  Serial.print(",");
  Serial.print("GyroY");
  Serial.print(",");
  Serial.print("GyroZ");
  Serial.print(",");  
  Serial.print("Temp");  
  Serial.println();  
}
void loop() {
  Serial.print(myIMU.readFloatAccelX(), 4);
  Serial.print(",");
  Serial.print(myIMU.readFloatAccelY(), 4);
  Serial.print(",");
  Serial.print(myIMU.readFloatAccelZ(), 4);
  Serial.print(",");
  Serial.print(myIMU.readFloatGyroX(), 4);
  Serial.print(",");  
  Serial.print(myIMU.readFloatGyroY(), 4);
  Serial.print(",");
  Serial.print(myIMU.readFloatGyroZ(), 4);
  Serial.print(",");
  Serial.print(myIMU.readTempC(), 2);  
  Serial.println();  
  delay(1000);
}