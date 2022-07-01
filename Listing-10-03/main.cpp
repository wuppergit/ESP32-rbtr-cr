#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>
#define LED_PIN                    2    // LED an GPIO02
BH1750 luxMeter;
String str;
bool automatik = true, ledStatus = false;
void setup(){
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN , LOW);      
  // I2C-Bus initialisieren (nicht automatisch durch BH1750 Bibliothek)
  Wire.begin();
  luxMeter.begin();
  Serial.println(F("Start BH1750"));
}
void loop() {
  float lux = luxMeter.readLightLevel();
  Serial.println("Helligkeit: " + String(lux) ); 
  if (automatik) {
    if (!ledStatus & lux < 100 ) {
      ledStatus = true;
      digitalWrite(LED_PIN, HIGH);  
      Serial.println("{\"Lampe\":\"ON\"}");       
    }
    if (ledStatus & lux >= 100) {
      ledStatus = false;
      digitalWrite(LED_PIN, LOW);  
      Serial.println("{\"Lampe\":\"OFF\"}");         
    }
  }
  if (Serial.available() > 0) { 
    while (Serial.available () == 0) {     // warten auf Eingabe    
    }
    str = Serial.readString();    
    Serial.println("{\"Serial-In\":\"" + String(str) + "\"}");  
    if (!automatik) {                      // manuelle Steuerung
      if (str == "ON\n") {
        digitalWrite(LED_PIN, HIGH);  
        Serial.println("{\"Lampe\":\"ON\"}");    
      }
      if (str == "OFF\n") {
        digitalWrite(LED_PIN, LOW);  
        Serial.println("{\"Lampe\":\"OFF\"}");            
      }
    }
    if (str == "automatik\n") {
      automatik = true;
    }   
    if (str == "manuell\n") {
      automatik = false;
    } 
  }  
  delay(4000);
}