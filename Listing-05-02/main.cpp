#include <Arduino.h>
#define ENA    33  // GPIO33 = Motor-Treiber A EnableA     
#define IN1    32  // GPIO32 = Motor-Treiber A IN1
#define IN2    19  // GPIO19 = Motor-Treiber A IN2
#define LEDC_KANAL_0  0     // Kanal 0; einer der 16-LEDC-Kanäle
#define LEDC_AUFLOESUNG 10  // 10-Bit Tastverh.aufl. (0-1023)
#define LEDC_FREQ     30000 // 30.000 Hz Frequenz
#define MINIMUMSPEED  510   // "Minimal-Geschindigkeit" damit der Mototr läuft
// die Motor-Geschwindigkeit wächst von 0 % auf 100 % und reduziert sich dann 
// wieder von 100 % auf 0 %
// die Höchstgeschwindigkeit ergibt sich aus dem Moor selbst und der Betriebsspannung
void testControlledSpeed() {
  Serial.println("\nMotor an: erhöhe Geschwindigkeit");
  // Geschwindigkeit von Minimum auf Fullspeed erhöhen
  for (int i = MINIMUMSPEED; i < 1024; i+=20) {
    ledcWrite(LEDC_KANAL_0, i);           // Tastverhältnis i  
    Serial.print (i); Serial.print (" ");    
    delay(500);
  }
  delay(4000); 
  // Geschwindigkeit von Fullspeed auf Minimum reduzieren
  Serial.println("\nreduziere Geschwindigkeit");
  for (int i = 1023; i >= MINIMUMSPEED; i-=20) {
    ledcWrite(LEDC_KANAL_0, i);           // Tastverhältnis i
    Serial.print (i); Serial.print (" ");
    delay(500);
  }
  delay(4000);
}
void setup() {
  Serial.begin(115200);    
  // Motor-Control-Pins als Output öffnen
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT); 
  pinMode(ENA, OUTPUT);
  // Vorwärtslauf  
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);  
  ledcSetup(LEDC_KANAL_0, LEDC_FREQ, LEDC_AUFLOESUNG); 
  ledcAttachPin(ENA, LEDC_KANAL_0);
  Serial.println("Setup beendet");
}
void loop() {  
  testControlledSpeed();  
}