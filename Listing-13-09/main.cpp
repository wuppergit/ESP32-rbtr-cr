
#include <Arduino.h>
#include <Wire.h>                  // I2C-Bus
#include <Servo.h>                 // Servo-Bibliothek
#define LSM6DS3_ADDR 0x6B 
#define SERVO_PIN_PITCH 23
#define SERVO_PIN_ROLL  15
#define SERVO_MINPW     520
#define SERVO_MAXPW     2600
// manuell vorgegebene Offsets
#define ACC_OFS_X       -50  // Accelero Offset X-Achse
#define ACC_OFS_Y       150  // Accelero Offset Y-Achse
#define ACC_OFS_Z       999  // Accelero Offset Z-Achse
#define GYRO_OFS_X      -10  // Gyroskop Offset X-Achse
#define GYRO_OFS_Y      50   // Gyroskop Offset Y-Achse
// Offsets; durch Vorgabe oder Kalibrierung überschrieben
float accOfsX  = ACC_OFS_X,   accOfsY  = ACC_OFS_Y,   accOfsZ  = ACC_OFS_Z;
float gyroOfsX = GYRO_OFS_X,  gyroOfsY = GYRO_OFS_Y;
// Accelerometer- und Gyroskop- Roh-Werte
int16_t accRaw[3], gyroRaw[3];
float   gyroScal[3];
// Winkel (theta = pitch, phi = roll, psi = yaw) 
// Accelerometer 
float thetaA  = 0, phiA  = 0, psiA  = 0;
// Accelerometer durch Low-Pass-Filter gefilterte Werte
float thetaAfAlt = 0, phiAfAlt = 0, psiAfAlt = 0;
// endgültige System Werte 
float theta = 0, phi = 0, psi = 0;
// für die Berechnung des Deltas Zeit
unsigned long t_now;
unsigned long t_last;
// globale Variablen Servo 
Servo myServoPitch;
Servo myServoRoll;
void printCharWithLeading(char c) {
  for (unsigned int mask = 0x80; mask; mask >>= 1) {
    Serial.print(mask&c ? '1':'0');
  }
  Serial.println();
}
// lesen / schreiben LSM6DS3-Register
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
    ret = Wire.read();;
  }  
  return ret;
}
char I2CwriteRegister (byte i2cAddr, byte regaddr, byte b) {
  byte error;
  Wire.beginTransmission(i2cAddr);
  Wire.write(regaddr);
  Wire.write(b);
  error = Wire.endTransmission();  
  return error;
}
// lesen LSM6DS3-Daten
void readLSM6DS3() {
  // Accelerometer
  Wire.beginTransmission(LSM6DS3_ADDR);
  Wire.write(0x28);
  Wire.endTransmission(false);
  Wire.requestFrom(LSM6DS3_ADDR, 6, true);
  // reading registers: 
  for(int i=0; i<3; i++) {
    accRaw[i] = Wire.read() | Wire.read()<<8;
  } 
  // Gyroskop
  Wire.beginTransmission(LSM6DS3_ADDR);
  Wire.write(0x22);
  Wire.endTransmission(false);
  Wire.requestFrom(LSM6DS3_ADDR, 4, true);
  // reading registers:
  for(int i=0; i<2; i++) {
    gyroRaw[i] = Wire.read() | Wire.read()<<8;
  }
}
// berechnen der Winkel von pitch und roll
void berechnenWinkel() {
  float k1 = 0.98;
  float dt =(t_now - t_last) / 1000.0;  
  // Schritt 1: Winkelberechnung Accelerometer
  //pitch
  thetaA = -atan2 (accRaw[0] , accRaw[2]) / 2 / 3.14159 *360;
  //roll
  phiA  =  atan2 (accRaw[1] , accRaw[2]) / 2 / 3.14159 *360; 
  // Schritt 2: Gyroskopdaten skalieren   
  gyroScal[0] = ((float)gyroRaw[0]) * 2000 / 32768;             
  gyroScal[1] = ((float)gyroRaw[1]) * 2000 / 32768; 
  // Schritt 3: Komplementärfilter
  // schnelle Bewegungen vom Gyroskop - langsame vom Accelerometer
  // LSM6DS3: gyro-pitch in gyroRaw[1]
  theta = (theta + gyroScal[1]*dt) * k1  +  thetaA * (1 - k1);
  phi   = (phi   + gyroScal[0]*dt) * k1  +  phiA   * (1 - k1);
}
// initialisieren LSM6DS3
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
  // Register CTRL1_XL (0x10)
  error = I2CwriteRegister(LSM6DS3_ADDR,0x10,0x66);  
  if (error != 0) {
    Serial.print  ("error set Linear acceleration sensor control register 1");
    Serial.print  ("Error-Code 0x");
    Serial.println(error, HEX);
    while(1);
  }
  // Register CTRL4_C  (0x13)
  error = I2CwriteRegister(LSM6DS3_ADDR,0x13,0x80);
  if (error != 0) {
    Serial.print  ("error set Control register 4");
    Serial.print  ("Error-Code 0x");
    Serial.println(error, HEX);
    while(1);
  }
  // Register CTRL2_G  (0x11)
  error = I2CwriteRegister(LSM6DS3_ADDR,0x11,0x6C);  
  if (error != 0) {
    Serial.print  ("error set Angular rate sensor control register 2");
    Serial.print  ("Error-Code 0x");
    Serial.println(error, HEX);
    while(1);
  }
  Serial.println();
}
// kalibrieren des LSM6DS3
void kalibrierenLSM6DS3() {  
  int readings = 100;
  // löschen möglicherweise manuell gesetzter Offsets
  accOfsX  = 0;
  accOfsY  = 0;
  accOfsZ  = 0;  
  gyroOfsX = 0;
  gyroOfsY = 0;  
  for (int i = 0; i < readings; i++) {  
    readLSM6DS3(); 
    accOfsX  += accRaw[0];
    accOfsY  += accRaw[1];   
    accOfsZ  += accRaw[2];          
    gyroOfsX += gyroRaw[0];
    gyroOfsY += gyroRaw[1];     
    delay (5);    
  }
  accOfsX  = accOfsX / readings;  
  accOfsY  = accOfsY / readings; 
  accOfsZ  = (accOfsZ / readings) - (32768 / 16);
  gyroOfsX = gyroOfsX / readings;  
  gyroOfsY = gyroOfsY / readings; 
}
void setup() {
  Serial.begin(115200);
  Wire.begin();
  // Sensor initialisieren
  initLSM6DS3();  
  // kalibrieren des LSM6DS3
  kalibrierenLSM6DS3();  
  // Servos anbinden
  myServoPitch.attach(SERVO_PIN_PITCH);
  myServoRoll.attach(SERVO_PIN_ROLL);  
  t_last = millis();
}
void loop() {
  // Read the raw values.
  readLSM6DS3();  
  // Offset aus Kalibrierung / manueller Vorgabe
  accRaw[0]  -= accOfsX;
  accRaw[1]  -= accOfsY;
  accRaw[2]  -= accOfsZ;  
  gyroRaw[0] -= gyroOfsX;
  gyroRaw[1] -= gyroOfsY;
  // Winkel berechnen mit Gyroskop und Kompl.-filter
  berechnenWinkel();
  //Servo positionieren
  float fPitch =  map(thetaA, -90, 90, 0, 180);  
  float fRoll  =  map(phiA, -90, 90, 0, 180);
  // Zeit merken
  t_last = t_now; 
  delay(5);
}