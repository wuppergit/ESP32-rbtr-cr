#include <Arduino.h>
#include "Wire.h"
#define LSM6DS3_ADDR 0x6B
int16_t accRaw[3];
float accScal[3];
void printCharWithLeading(char c) {
  for (unsigned int mask = 0x80; mask; mask >>= 1) {
    Serial.print(mask&c ? '1':'0');
  }
  Serial.println();
}
char I2CreadRegister (char reg) {
  // read register reg (Format 0x00) 
  byte error;  
  char ret = 0XFF;
  // read register  
  Wire.beginTransmission(LSM6DS3_ADDR);
  Wire.write(reg);
  error = Wire.endTransmission();
  if (error != 0) {
    Serial.print  ("error Wire.endTransmission - read Register: ");
    Serial.print  (reg);
    Serial.print  (", Error-Code: 0x");
    Serial.println(error, HEX);
  }
  Wire.requestFrom(LSM6DS3_ADDR, 1);
  if(1 == Wire.available()) {
    ret = Wire.read();
  }
  return ret;
}
char I2CwriteRegister (byte i2cAddr, byte regaddr, byte b) {
  byte error;   
  Wire.beginTransmission(i2cAddr);         // open communication
  Wire.write(regaddr);                     // select Register (Hex)
  Wire.write(b);                           // write Byte
  error = Wire.endTransmission();  
  return error;
}
void initLSM6DS3() {
  byte error;
  char c;  
  // test I2C-Verbindung 
  Wire.beginTransmission(LSM6DS3_ADDR);
  error = Wire.endTransmission();
  if (error != 0) {
    Serial.print  ("kein I2C-Gerät an Adresse 0x");
    Serial.println(LSM6DS3_ADDR, HEX);
    Serial.print  ("Error-Code 0x");
    Serial.println(error, HEX);
    while(1);
  }    
  error = I2CwriteRegister(LSM6DS3_ADDR,0x10,0x66);  
  if (error != 0) {
    Serial.print  ("error set Linear acceleration sensor control register 1");
    Serial.print  ("Error-Code 0x");
    Serial.println(error, HEX);
    while(1);
  }
  printCharWithLeading(I2CreadRegister(0x10));
  error = I2CwriteRegister(LSM6DS3_ADDR,0x13,0x80);  
  if (error != 0) {
    Serial.print  ("error set Control register 4");
    Serial.print  ("Error-Code 0x");
    Serial.println(error, HEX);
    while(1);
  }
  printCharWithLeading(I2CreadRegister(0x13)); 
  Serial.println();
}
void setup() {
  byte error;
  Serial.begin(115200);
  delay(10); 
  Wire.begin();
  initLSM6DS3();    
}
void loop() {    
  Wire.beginTransmission(LSM6DS3_ADDR);
  Wire.write(0x28);
  Wire.endTransmission(false);
  Wire.requestFrom(LSM6DS3_ADDR, 6, true);
  for(int i=0; i<3; i++) {
    accRaw[i] = Wire.read() | Wire.read()<<8;
  }
  // normalisieren
  for(int i=0; i<3; i++) {
    accScal[i] =  (float)accRaw[i] * 16 / 32768;          
  } 
  Serial.print  ("Roh-Werte:       ");  
  for(int i=0; i<3; i++) {
    Serial.print  (accRaw[i]);  
    Serial.print  (",  "); 
  }
  Serial.println();       
  Serial.print  ("scalierte Werte: ");
  for(int i=0; i<3; i++) {
    Serial.print  (accScal[i]); 
    Serial.print  (",  ");            
  }
  Serial.println();      
  Serial.println();    
  delay(1000);
}