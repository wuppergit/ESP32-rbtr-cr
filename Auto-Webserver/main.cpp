/*
 *  Motor-Driver für 2 Motoren (Antrieb / Richtung eigener Motor
 *  WiFi-Steuerung (STA-Mode und AP-Mode)
 *  
 *  MCU:            ESP32   (ESP 32 Dev Module) 
 *  Funktion:       An / Aus sowie PWM-Beschleunigungs- und -Verzögerungsfunk. 
 *                  2 Motore
 *                  WiFi Station / WiFi Access-Point
 *                  OTA-Update                  
 *                  IR-Steuerung
 *                  Sensoren: US, Kontakt
 *                  Steuerung mit APP (MIT-App-Inventor)                  
 *                  ein Servo bewegt sich in seinem Winkelbereich
 *                  ein BHT1750 misst die Heeligkeit und sichert den 
 *                      Winkel mit dem hellsten Wert                  
 *                  GPS-Signnal empfangen    
 *                  
 *  Achtung:        - HC-SR04 muss mit 5 Volt betrieben werden; daher 
 *                      Spannungsteiler erforderlich !! (470 Ohm / 1k Ohm (Verhältnis ~ 1:2)
 *                  - nicht jeder ESP32-GPIO kann einen Servo ansteuern  
 *                  - das GPS-Modul braucht etwas Zeit, die Satelliten zu lokalisieren
 */
// folgende defines beziehen sich auf Eigenschaften des Roboter-Autos
// ist das define auskommentiert, verfügt das Roboter-Auto nicht über eine entsprechende Funktion
#define HAT_IR                // hat Infrarot-Steuerung
#define HAT_SENSOREN          // hat einen US-Sensor und einen Kontaktsensor
#define HAT_AUTONOMES_FAHREN  // für autonomes Fahren
#define HAT_GPS               // für GPS-Anbindung
  
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiMulti.h> 
#include <WebServer.h>                                                                            
#include <WiFiUdp.h>          // User Datagram Protocol (UDP; minimales, verbindungsloses Netzwerkprotokoll)
#include <ArduinoOTA.h>       // fuer OTA-Update
#include "htmlSeiten.h"

#if defined HAT_IR
  #include <IRrecv.h>
  #include <IRtext.h>
#endif
#if defined HAT_AUTONOMES_FAHREN
  #include <Servo.h>                    // Servo-Bibliothek
  #include <Wire.h>                     // I2C
  #include <BH1750.h>                   // für Lichtsensor BH1750
#endif
#if defined HAT_GPS
  #include <TinyGPS++.h>                // GPS-Bibliothek
#endif


#define ENA             33    // GPIO33 = Pin D33 -  Motor-Treiber A EnableA      
#define IN1             32    // GPIO32 = Pin D32 -  Motor-Treiber A IN1
#define IN2             19    // GPIO19 = Pin D19 -  Motor-Treiber A IN2
#define ENB             4     // GPIO04 = Pin D04 -  Motor-Treiber A EnableB
#define IN3             18    // GPIO18 = Pin D18 -  Motor-Treiber B IN3
#define IN4             5     // GPIO05 = Pin D05 -  Motor-Treiber B IN4
#define LEDC_KANAL_0    0     // Kanal 0; einer der 16-LEDC-Kanäle
#define LEDC_KANAL_1    1     // Kanal 1; einer der 16-LEDC-Kanäle
#define LEDC_AUFLOESUNG 10    // 10-Bit Tastverhältnisauflöung (0-1023)
#define LEDC_FREQ       30000 // 5.000 Hz Frequenz
#define IR_RECV_PIN     14    // GPIO14 = Pin D14 - IR-Receiver
#define INTERR_PIN      27    // GPIO27 = Pin D27 - externer Interrupt 
#define US_TRIG_PIN     26    // GPIO26 = Pin D26 - Trigger-Pin für den Ultrasch.-Sensor 
#define US_ECHO_PIN     25    // GPIO25 = Pin D25 - Echo-Pin für den Ultrasch.-Sensor 
#define SERVO_PIN       13    // GPIO13 - PIN D13 - Servo-Motor´
#define LEDC_KANAL_2    2     // Kanal 2; einer der 16-LEDC-Kanäle
#define SDA             21    // GPIO21 - Pin D21 - SDA I2C Kanal 1 
#define SCL             22    // GPIO22 - Pin D22 - SCL I2C Kanal 1
#define RXD2            16    // RX-Anschluss fuer GPS-Modul (Achtung: Verbindungen über Kreuz: RX an TX, TX an RX) 
#define TXD2            17    // TX-Anschluss fuer GPS-Modul 


// Funktions-Prototypen
void motorenStop ();          // Motorsteuerung
void rechts();
void links();
void vorwaerts();
void rueckwaerts();
void ausweichenRueck(int);
void ausweichenVor(int);
void controlMotors (String);  // WiFi
void checkIR();               // IR-Steuerung
void checkInterrupt();        // Sensoren
void IRAM_ATTR isrHandler();
void lesenHCSR04();
int lesenBH1750(int);         // autonomes Fahren
int findeLicht(bool); 
void autonomFahren();
void lesenGPS();              // GPS
void displayGPS();


// für die WiFi-Anbindung
String wifiModus = "STA";     // STA = Staion, AP = Access-Point 

// Definition von Router-Zugangsdaten (credentials)
const char* ssid1      = "IhreSSID";
const char* password1  = "IhrPasswort";
IPAddress lclIP (192,168,178,207);
IPAddress gateway (192,168,178,1);
IPAddress subnet (255,255,255,0);
IPAddress primaryDNS (8, 8, 8, 8);          //optional
IPAddress secondaryDNS (8, 8, 4, 4);        //optional
// Definition der AP-Zugangsdaten (credentials)
const char *ssidAP     = "ESP_AP";
const char *passwordAP = "1234abcd";

WiFiMulti wifiMulti;     
WebServer webServer(80);

   
// ###### fuer die Motorsteuerung                              #####

#define MINIMUMSPEED    723   // "Minimal-Geschindigkeit" damit der Mototr läuft
int motorSpeed = 1023;        // fuer Geschwindigkeit (1023 = Maximum)
int lenkSpeed  = 850;         // fuer Lenkbewegungen / Richtungswechsel
bool motorRun = 0;            // 1 = true = Motor an

// Motoren Fahrt und Richtung schalten
void motorenStop () {
  Serial.println("stop");  
  digitalWrite(IN1, LOW);  
  digitalWrite(IN2, LOW);       
  digitalWrite(IN3, LOW);  
  digitalWrite(IN4, LOW);     
  motorRun = false;   
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
  motorRun = true;    
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
  motorRun = true;      
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

void langsamer () {
  Serial.println("langsamer");  
  motorSpeed-=20;
  if (motorSpeed < MINIMUMSPEED) {
      motorSpeed = MINIMUMSPEED; 
  }
  ledcWrite(LEDC_KANAL_0, motorSpeed);   
  ledcWrite(LEDC_KANAL_1, motorSpeed);  
}

void schneller () {
  Serial.println("schneller");  
  motorSpeed+=20;   
  if (motorSpeed > 1023) {
      motorSpeed = 1023; 
  }
  ledcWrite(LEDC_KANAL_0, motorSpeed);     
  ledcWrite(LEDC_KANAL_1, motorSpeed);    
}


// ###### IR-Steuerung                                         #####

#if defined HAT_IR
const uint16_t kRecvPin = IR_RECV_PIN;          // GPIO13 = G13
const uint16_t kCaptureBufferSize = 1024;
const uint8_t  kTimeout = 50;
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);
decode_results results;                         // für die Ergebnisse

// wertet IR-Fernbedienungsbefehle aus
void checkIR (void) {
  if (irrecv.decode(&results)) {
    // Check if we got an IR message that was to big for our capture buffer.
    if (results.overflow) {
      Serial.printf(D_WARN_BUFFERFULL "\n", kCaptureBufferSize);
      return;
    }       
    //Serial.println(results.command, HEX);    
    if (results.command == 0x46) {
      vorwaerts ();   
    } else 
    if (results.command == 0x44) {
      links ();
    } else 
    if (results.command == 0x40) {
      motorenStop ();  
    } else 
    if (results.command == 0x43) {
      rechts ();    
    } else 
    if (results.command == 0x15) {
      rueckwaerts ();   
    } else 
    if (results.command == 0x42) {
      langsamer (); 
    } else 
    if (results.command == 0x4A) {
      schneller (); 
    }  
  }  
}
#endif


// ###### Sensoren                                             #####

// Sensoren
#if defined HAT_SENSOREN
//Kontaktsensor
volatile bool interruptStatus;    // 1 = true = interrupt
volatile int alteZeit = 0, debounceZeit = 500;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
//Ultraschallsensor
float entfernungUS;               // berechnete Entfernung Ultraschalls.


// überprüfen, ob Stoßstangenkontakt (Interrupt) vorliegt und ggf. Ausweichmanöver durchführen 
void checkInterrupt() {
  if (interruptStatus == true) { 
    Serial.println("interruptStatus = true");    
    portENTER_CRITICAL_ISR(&timerMux);
    interruptStatus = false;  
    portEXIT_CRITICAL_ISR(&timerMux);       
    if (motorRun) {
      ausweichenVor(0);
    }
  }  
}

// ISR-Routine 
void IRAM_ATTR isrHandler() { 
  if  ( (millis() - alteZeit) > debounceZeit) {
    portENTER_CRITICAL_ISR(&timerMux); 
    interruptStatus = true;
    portEXIT_CRITICAL_ISR(&timerMux);   
    alteZeit = millis();                      // letzte Zeit merken
  }   
}

// lesen der HCSR04 Messwerte 
void lesenHCSR04() {
  float dauer;                               // Zeitspanne des Echos  
  digitalWrite(US_TRIG_PIN, LOW);            // definierten Zustand schaffen
  delayMicroseconds(5);                      // warten auf defin. PIN-Zustand
  digitalWrite(US_TRIG_PIN, HIGH);           // Trigger-Pin für 10 Microsk. an
  delayMicroseconds(10);
  digitalWrite(US_TRIG_PIN, LOW); 
  dauer = pulseIn(US_ECHO_PIN, HIGH);        // Pulslänge in Microse. messen
  entfernungUS = dauer * 0.034 / 2;          // Entfernung berechnen
  //Serial.println("Entfernung (cm): " + String (entfernungUS));
  if (entfernungUS < 5.0) {
    Serial.println("Entfernung (cm) zu kurz: " + String (entfernungUS)); 
    ausweichenRueck(0);                      // nach rechts ausweeichen
  }
  delay(1);                                  // Selbstverwaltung ESP32
}

// Ausweichmanöver
// Hindernis bei Vorwärtsfahrt; 
// => ausweichen durch temporäre Rückwärtsbewegeung
// @ri : Ausweichrichtung: 0 = rechts, 1 = links
void ausweichenRueck(int ri) {
  Serial.println("ausweichenRueck");   
  rueckwaerts();
  delay (1000);                              // 1 sek. rueckwaerts fahren  
  if (ri == 0) {
    links();  
  } else {
    rechts ();
  }
  delay (500);                               // 0,5 sek. links fahren  
  vorwaerts();
}
// // Hindernis bei Rückwärtsfahrt; 
// => ausweichen durch temporäre Vorwärtsbewegeung
// @ri : Ausweichrichtung: 0 = rechts, 1 = links
void ausweichenVor(int ri) {
  Serial.println("ausweichenVor");   
  vorwaerts();
  delay (1000);                              // 1 sek. rueckwaerts fahren  
  if (ri == 0) {
    links();  
  } else {
    rechts ();
  }
  delay (500);                               // 0,5 sek. links fahren  
  rueckwaerts();
}

#endif


// ###### für autonomes Fahren                                 #####

#if defined HAT_AUTONOMES_FAHREN
#define SERVO_MINPW     520       // default 544
#define SERVO_MAXPW     2600      // default 2400
Servo myServo;                    // Servo-Objekt erstellen
bool rotation = true;             // Rotations-Richtung des Servoss
BH1750 luxMeter;
float messWertHelligkeit = 0;     // Messwert der Helligkeit BH1750
float messWertHelligkeitAlt = 0; 

// positionieren des Servos und lesen der Lichtmesswerte 
int lesenBH1750(int i) {
  int pos = 0;   
  myServo.write(i);                    // Servo positionieren
  delay(500);   
  messWertHelligkeit = luxMeter.readLightLevel();
  if (messWertHelligkeit > messWertHelligkeitAlt) {
    messWertHelligkeitAlt = messWertHelligkeit;
    pos = i;
  }                   
  return pos;
}

// suche nach grösster Lichtquelle
int findeLicht(bool rotation) {
  int i;
  int pos = 0, posX;

  messWertHelligkeitAlt = 0;
  if (rotation) { 
    // geht von 0 Grad bis 180 Grad in 10 Grad-Schritten   
    for (i = 0; i <= 180; i += 10) {
      posX = lesenBH1750(i);            // Servo positionieren und Helligkeissensor lesen      
      if (posX) {
        pos = posX;
      }
    }            
  } else { 
    // geht von 180 Grad bis 0 Grad in 10 Grad-Schritten   
    for (i = 180; i >= 0; i -= 10) {
      posX = lesenBH1750(i);              // Servo positionieren und Helligkeissensor lesen      
      if (posX) {
        pos = posX;
      }
    }         
  }            
  return pos;
}


// autonomes Fahren
// Servowinkel 180: links / 0: rechts
void autonomFahren() {
  rotation = !rotation;                // Rotationsrichtung ändern
  int pos = findeLicht(rotation); 
  Serial.println();   
  Serial.print  ("groesste Helligkeit in Winkel: ");    
  Serial.println(pos); 

  if (pos < 61) {
    Serial.println("rechts");
    rechts();
    vorwaerts();
    delay (1000);
    motorenStop ();
  } else {
    if (pos < 121) {
      Serial.println("geradeaus");
      vorwaerts();
      delay (1000);
      motorenStop ();      
    } else {
      Serial.println("links");
      links();
      vorwaerts();
      delay (1000);
      motorenStop ();      
    }
  }  
  delay (2000);
}
#endif


// für GPS-Anbindung
#if defined HAT_GPS
TinyGPSPlus gps;                  // TinyGPS++ Objekt
static const uint32_t GPSBaud = 9600;

// auslesen der Rohdaten und aufbereiten der GPS-Daten
void lesenGPS() {
  while (Serial2.available() > 0) { 
    // Ausgabe der Position, wenn Datensatz fehlerfrei decodiert wurde
    if (gps.encode(Serial2.read()))  {
      displayGPS();
    }    
  } 
  if (millis() > 5000 && gps.charsProcessed() < 10)   {
    Serial.println("konnte kein GPS-Modul finden: Verkabelung überprüfen");    
//    while(true) {}    
  }  
}

// nur für Ausgabe im Seriellen Monitor nötig
void displayGPS() {
  Serial.print(F("Location: ")); 
  if (gps.location.isValid()) {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())   {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())   {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  } else   {
    Serial.print(F("INVALID"));
  }
  Serial.println();
}
#endif


// ###### WiFi                                                 #####

void handleRoot() {   
  Serial.println("handleRoot");
  String htmlPage; 
  float f;
  char latChr[15], longChr [15]; ; 
  htmlPage.concat(motorHTML_Seite1);             // HTML-Seite aufbauen
  String s = (String("width: ") + String((motorSpeed*100)/1023) + String("%;"));   
  htmlPage.concat(s);  
  htmlPage.concat(motorHTML_Seite2); 
  s = "";
#if defined HAT_GPS     
  if (gps.location.isValid()) {
    f = gps.location.lat(), 6;
    dtostrf(f, 8, 6, latChr); 
    f = gps.location.lng(), 6;
    dtostrf(f, 7, 6, longChr);     
    s = (String("GPS:  Breite:  ") + String(latChr) + 
         String(" / Länge: ") + String(longChr) );   
  } else {
     s  = "keine GPS-Daten";
  }   
#endif
  htmlPage.concat(s);      
  htmlPage.concat(motorHTML_Seite3);
  webServer.send(200, "text/html", htmlPage);    // HTML-Seite senden 
}

// Auswertung einer HTML-Anfrage einem URI '/motor' 
void handleMotor() {                          
  Serial.println("handleMotor"); 
  controlMotors(webServer.arg("fahr"));   
  handleRoot(); 
}

// Auswertung einer HTML-Anfrage einem URI '/motorAI' 
void handleMotorAI() {                          
  Serial.println("handleMotorAI");    
  controlMotors(webServer.arg("fahr"));
  String s = String ((motorSpeed*100)/1023) ;   
  webServer.send(200, "text/plain", s); 
}

// Steuerung der Motoren
void controlMotors (String fahrAuftrag) {                          
  Serial.println("controlMotors");      
  if(webServer.arg("fahr") == "langsamer") { 
    Serial.println("langsamer");  
    motorSpeed-=20;
    if (motorSpeed < MINIMUMSPEED) {
      motorSpeed = MINIMUMSPEED; 
    }
    ledcWrite(LEDC_KANAL_0, motorSpeed); 
  } 
  if(webServer.arg("fahr") == "schneller") { 
    Serial.println("schneller");      
    motorSpeed+=20;   
    if (motorSpeed > 1023) {
      motorSpeed = 1023; 
    } 
    ledcWrite(LEDC_KANAL_0, motorSpeed);  
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
}

// sendet HTTP status 404 (Not Found) 
// wenn es keinen Handler fuer den URI im HTTP-Request gibt
void handleNotFound(){
  Serial.println("handleNotFound");  
  webServer.send(404, "text/plain", "404: Not found"); 
}

// ###### setup                                                #####

void setup(void){
  Serial.begin(115200);
#if defined HAT_GPS
  Serial2.begin(GPSBaud, SERIAL_8N1, RXD2, TXD2);   // für GPS
#endif             
  delay(10);
  // Motor-Control-Pins als Output öffnen
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT); 
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);   
  pinMode(ENA, OUTPUT);
  pinMode(ENA, OUTPUT);

#if defined HAT_SENSOREN  
  // Interrupt-Pins als Input öffnen und ISR-Handler aktivieren
  pinMode(INTERR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERR_PIN), isrHandler, FALLING);
  // Ultraschall-Pins als Input / Output öffnen
  pinMode(US_TRIG_PIN, OUTPUT);
  pinMode(US_ECHO_PIN, INPUT); 
#endif   

  // Motor-Control-Pins auf LOW setzen  
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);  
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);  
  ledcSetup(LEDC_KANAL_0, LEDC_FREQ, LEDC_AUFLOESUNG); 
  ledcAttachPin(ENA, LEDC_KANAL_0);
  ledcWrite(LEDC_KANAL_0, motorSpeed);           // Tastverhältnis 
  ledcSetup(LEDC_KANAL_1, LEDC_FREQ, LEDC_AUFLOESUNG); 
  ledcAttachPin(ENB, LEDC_KANAL_1);
  ledcWrite(LEDC_KANAL_1, motorSpeed);    

  if (wifiModus.equals ("STA")) {
    // WiFi-Netzwerke hinzufügen, mit denen eine Verbindung aufgebaut werden soll 
    wifiMulti.addAP(ssid1, password1); 
    wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2");
    wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");
    WiFi.mode(WIFI_STA); 
    Serial.println("WiFi Station-Modus Verbindungsaufbau ");     
    if (!WiFi.config(lclIP, gateway, subnet, primaryDNS, secondaryDNS)) {
      Serial.println("STA konnte nicht konfiguriert werden");
      while(1);    
    } 

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
  } else {
    Serial.println("WiFi Access-Point-Modus ");
    WiFi.softAP(ssidAP, passwordAP);             
    Serial.print("Access Point \"");
    Serial.print(ssidAP);
    Serial.println("\" gestartet");
    Serial.print("IP Addresse:\t");
    Serial.println(WiFi.softAPIP());               // ESP32 
    delay (500);
  }

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();

  // Call der 'handleRoot' Funktion bei einer Client-Amforderung URI "/"
  webServer.on("/", HTTP_GET, handleRoot);     
  // Call der 'handleRoot' Funktion bei einer Client-Amforderung URI "/"
  webServer.on("/motor", HTTP_GET, handleMotor);
  // Call der 'handleMotorAI' Funktion bei einer Client-Amforderung URI "/motorAI"
  webServer.on("/motorAI", HTTP_GET, handleMotorAI);    
  webServer.onNotFound(handleNotFound);          // bei einem unbekannten URI  
  webServer.begin();                             // Start des Webservers
  Serial.println("HTTP server gestartet");

#if defined HAT_IR
  irrecv.enableIRIn();                           // Start IR-Receiver
#endif

#if defined HAT_AUTONOMES_FAHREN
   myServo.attach(SERVO_PIN, LEDC_KANAL_2);       // Servo-Steuerung an SERVO_PIN
  // I2C-Bus initialisieren (nicht automatisch durch BH1750 Bibliothek)
  Wire.begin();
  luxMeter.begin();
#endif
}


// ###### loop                                                #####

void loop(void){
  ArduinoOTA.handle();  
  webServer.handleClient();                      // horchen auf HTTP-Anfragen von Clients 

#if defined HAT_IR
  checkIR();
#endif

#if defined HAT_SENSOREN
  checkInterrupt();    
  lesenHCSR04(); 
#endif

#if defined HAT_AUTONOMES_FAHREN  
  autonomFahren();
#endif  

#if defined HAT_GPS
  lesenGPS();
#endif

  delay (10);
}