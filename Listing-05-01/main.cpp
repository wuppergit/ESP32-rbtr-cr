#include <Arduino.h>
#define IN1             32    // GPIO32 Motor-A IN1
#define IN2             19    // GPIO19 Motor-A IN2
// Motor läuft in beide Richtungen mit voller Geschwindigkeit
void testFullSpeed() {
  Serial.println("Motor an");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  delay(5000);
  Serial.println("Motor aus");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);  
  delay(2000);
  Serial.println("Richtungswechsel");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  delay(5000);
  Serial.println("Motor aus");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  delay(2000);
}
void setup() {
  Serial.begin(115200);    
  // Motor-Control-Pins als Output öffnen
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);  
  Serial.println("Setup beendet");
}
void loop() {
  testFullSpeed();     
}