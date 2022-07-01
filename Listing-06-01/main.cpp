#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiMulti.h> 
#include <WebServer.h>
#define ENA             33    // D33 -  Motor-A EnableA      
#define IN1             32    // D32 -  Motor-A IN1
#define IN2             19    // D19 -  Motor-A IN2
#define ENB             4     // D04 -  Motor-A EnableB
#define IN3             18    // D18 -  Motor-B IN3
#define IN4             5     // D05 -  Motor-B IN4
#define LEDC_KANAL_0    0     // Kanal 0: LEDC-Kanäle
#define LEDC_KANAL_1    1     // Kanal 1; LEDC-Kanäle
#define LEDC_AUFLOESUNG 10    // 10-Bit Tastv. (0-1023)
#define LEDC_FREQ       30000 // 5.000 Hz Frequenz
#include "htmlSeiten.h"
#define MINIMUMSPEED    723   // "Minimal-Geschindigkeit" damit der Mototr läuft
// Definition von Router-Zugangsdaten (credentials)
const char* ssid1      = "IhreSSID";
const char* password1  = "IhrPasswort";
WiFiMulti wifiMulti;     
WebServer webServer(80);
int motorSpeed = 1023;      // Geschwindigkeit 
int lenkSpeed  = 850;       // Lenkbewegungen
// Motoren Fahrt und Richtung schalten
void motorenStop () {
  Serial.println("stop");
  digitalWrite(IN1, LOW);  
  digitalWrite(IN2, LOW);       
  digitalWrite(IN3, LOW);  
  digitalWrite(IN4, LOW);       
}  
void vorwaerts () {
  Serial.println("vor");  
  ledcWrite(LEDC_KANAL_0, motorSpeed);     
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);    
  ledcWrite(LEDC_KANAL_1, motorSpeed);    
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);       
  Serial.print  ("motorSpeed ");        
  Serial.println(motorSpeed);     
}
void rueckwaerts () {
  Serial.println("rueck"); 
  ledcWrite(LEDC_KANAL_0, motorSpeed);     
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);  
  ledcWrite(LEDC_KANAL_1, motorSpeed);     
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);    
  Serial.print  ("motorSpeed ");        
  Serial.println(motorSpeed);    
}
void links () {
  Serial.println("links"); 
  ledcWrite(LEDC_KANAL_0, lenkSpeed);   
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);     
  ledcWrite(LEDC_KANAL_1, lenkSpeed);   
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);  
  Serial.print  ("lenkSpeed ");        
  Serial.println(lenkSpeed);   
}
void rechts () {
  Serial.println("rechts");       
  ledcWrite(LEDC_KANAL_0, lenkSpeed);     
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);     
  ledcWrite(LEDC_KANAL_1, lenkSpeed);     
  digitalWrite(IN3, LOW);
  Serial.print  ("lenkSpeed ");        
  Serial.println(lenkSpeed);   
}
// senden der Start HTML-Seite, bei einem URI '/' oder 
// bei einem URI 'motor' nach Auswertung der Parameter
void handleRoot() {   
  Serial.println("handleRoot");
  String htmlPage;  
  htmlPage.concat(motorHTML_Seite1);             // HTML-Seite aufbauen
  String s = (String("width: ") + String((motorSpeed*100)/1023) + String("%;"));   
  htmlPage.concat(s);  
  htmlPage.concat(motorHTML_Seite2);    
  webServer.send(200, "text/html", htmlPage);    // HTML-Seite senden 
}
// Auswertung einer HTML-Anfrage einem URI '/motor' 
void handleMotor() {                          
  Serial.println("handleMotor"); 
  if(webServer.arg("fahr") == "langsamer") { 
    Serial.println("langsamer");  
    motorSpeed-=20;
    if (motorSpeed < MINIMUMSPEED) {
      motorSpeed = MINIMUMSPEED; 
    }
    ledcWrite(LEDC_KANAL_0, motorSpeed);       // Tastverhältnis 
    ledcWrite(LEDC_KANAL_1, motorSpeed);       
  }  
  if(webServer.arg("fahr") == "schneller") { 
    Serial.println("schneller");      
    motorSpeed+=20;   
    if (motorSpeed > 1023) {
      motorSpeed = 1023; 
    }
    ledcWrite(LEDC_KANAL_0, motorSpeed);
    ledcWrite(LEDC_KANAL_1, motorSpeed);
  }  
  if(webServer.arg("fahr") == "stop") { 
    motorenStop ();    
  } 
  if(webServer.arg("fahr") == "vor") { 
    vorwaerts ();    
  }
  if(webServer.arg("fahr") == "rueck") {
    rueckwaerts ();    
  } 
  if(webServer.arg("fahr") == "links") { 
    links ();        
  }    
  if(webServer.arg("fahr") == "rechts") { 
    rechts ();    
  }
  handleRoot();
}
// sendet HTTP status 404 (Not Found) 
// wenn es keinen Handler fuer den URI im HTTP-Request gibt
void handleNotFound(){
  Serial.println("handleNotFound");  
  webServer.send(404, "text/plain", "404: Not found"); 
}
void setup(void){
  Serial.begin(115200);         
  delay(10);
  // Motor-Control-Pins als Output öffnen
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT); 
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);   
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);  
  // Motor-Control-Pins auf LOW setzen  
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
  // WiFi-Netzwerke hinzufügen, mit denen eine Verbindung aufgebaut werden soll 
  wifiMulti.addAP(ssid1, password1); 
  wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2");
  wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");
  WiFi.mode(WIFI_STA);  
  Serial.println("WiFi Station-Modus Verbindungsaufbau ");
  // auf WiFi-Verbindung warten: d.h. scan WiFi-Netzwerke und verbinden 
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(250);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.print("verbunden mit ");
  Serial.println(WiFi.SSID());              
  Serial.print("IP-Adresse:\t");
  Serial.println(WiFi.localIP());       
  // Call der 'handleRoot' Funktion bei einer Client-Amforderung URI "/"
  webServer.on("/", HTTP_GET, handleRoot);     
  // Call der 'handleRoot' Funktion bei einer Client-Amforderung URI "/"
  webServer.on("/motor", HTTP_GET, handleMotor);
  webServer.onNotFound(handleNotFound);          // bei einem unbekannten URI
  webServer.begin();                             // Start des Webservers
  Serial.println("HTTP server gestartet");
}
void loop(void){
  //testControlledSpeed();    
  webServer.handleClient();     // Anfragen v. Clients?
  delay (50);
}