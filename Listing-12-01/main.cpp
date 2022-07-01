#include <Arduino.h>
#define S2               12
#define S3               14
#define sensorOut        22
int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;
int clearFrequency = 0;
void setup() {
  Serial.begin(115200);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
}
void loop() {
  // lesen der „Blau-Filter-Photodioden“
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  blueFrequency = pulseIn(sensorOut, LOW);
  delay(100);
  // lesen der „Rot-Filter-Photodioden“
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);  
  redFrequency = pulseIn(sensorOut, LOW);
  delay(100);
  // lesen der „Grün-Filter-Photodioden“
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  greenFrequency = pulseIn(sensorOut, LOW);
  delay(100);
  // lesen der Photodioden ohne Filter
  digitalWrite(S2,HIGH);
  digitalWrite(S3,LOW);
  clearFrequency = pulseIn(sensorOut, LOW);
  Serial.print("R = ");
  Serial.print(redFrequency);  
  Serial.print(", G = ");
  Serial.print(greenFrequency);  
  Serial.print(", B = ");
  Serial.print(blueFrequency);  
  Serial.print(", C = ");
  Serial.println(clearFrequency);  
  delay(1000);  
}