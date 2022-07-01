#include <Arduino.h>
#define ENA             33
#define IN1             32
#define IN2             19
#define ENB             4
#define IN3             18
#define IN4             5
#define LEDC_KANAL_0    0
#define LEDC_KANAL_1    1
#define LEDC_AUFLOESUNG 10
#define LEDC_FREQ       30000
#define TCS230_OUT      22
#define MINIMUMSPEED    723
#define THRESHOLD_DARK  65    // Sensor-Schwellenwert
int motorSpeed = 800;         // 1023 = Maximum
int lenkSpeed  = 750;
int lenkDauer  = 50;
int clearFrequency = 0;
// Motoren Fahrt und Richtung schalten
void motorenStop () {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
void vorwaerts () {
  ledcWrite(LEDC_KANAL_0, motorSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  ledcWrite(LEDC_KANAL_1, motorSpeed);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void links () {
  ledcWrite(LEDC_KANAL_0, lenkSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  delay (lenkDauer);
}
void rechts () {
  ledcWrite(LEDC_KANAL_1, lenkSpeed);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay (lenkDauer);
}
void findPath () {
  motorenStop ();
  int timesMove = 3;
  // links weiter ?
  for (int i = 1; i < timesMove; i++) {
    links();
    motorenStop ();
    clearFrequency = pulseIn(TCS230_OUT, LOW);
    if (clearFrequency > THRESHOLD_DARK) {
      // Pfad gefunden
      Serial.println("path found");
      vorwaerts ();
      return;
    } 
    delay (1000);
  }
  // zurück bewegen = timesMove
  // => rechts: maximal (timesMove * 2) - 1

  // rechts weiter ?
  for (int i = 1; i < (timesMove * 2)-1; i++) {
    rechts();
    motorenStop ();
    clearFrequency = pulseIn(TCS230_OUT, LOW); 
    Serial.println(clearFrequency); 
    if (clearFrequency > THRESHOLD_DARK) {
      // Pfad gefunden
      Serial.println("path found");
      vorwaerts ();
      return;
    } 
    delay (1000);
  }
  // Serial.println("path lost");
  motorenStop ();
  delay (10000);
}


void setup(void){
  Serial.begin(115200);    
  pinMode(TCS230_OUT, INPUT);  
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT); 
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  ledcSetup(LEDC_KANAL_0, LEDC_FREQ, LEDC_AUFLOESUNG); 
  ledcAttachPin(ENA, LEDC_KANAL_0);
  ledcWrite(LEDC_KANAL_0, motorSpeed);
  ledcSetup(LEDC_KANAL_1, LEDC_FREQ, LEDC_AUFLOESUNG); 
  ledcAttachPin(ENB, LEDC_KANAL_1);
  ledcWrite(LEDC_KANAL_1, motorSpeed); 
}
void loop(void){
  clearFrequency = pulseIn(TCS230_OUT, LOW);
  if (clearFrequency < THRESHOLD_DARK) {
    findPath ();
  }
  vorwaerts();
  delay (100);                 // einstellen
  motorenStop();
}