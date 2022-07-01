#include <Arduino.h>
#include <Ps3Controller.h>    // PS3-Controller
#include <Servo.h>            // Servo-Bibliothek
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
#define SERVO_PIN       13    // GPIO13 -  Servo-Motor´
#define LEDC_KANAL_2    2     // Kanal 2; einer der 16-LEDC-Kanäle
#define SERVO_MINPW     520   // default 544
#define SERVO_MAXPW     2600  // default 2400
#define MINIMUMSPEED    510   // "Minimal-Geschindigkeit" damit der Mototr läuft
// Funktions-Prototypen
void motorenStop ();          // Motorsteuerung
void rechts();
void links();
void vorwaerts();
void rueckwaerts();
Servo myServo;                        // Servo-Objekt erstellen
int servoPos = 90;
int servoPosSich = 90;
// fuer die Motorsteuerung                              
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
//  PS3-Steuerung     
int leftStickX = 0;
void notify() {  
  if (Ps3.event.button_down.circle) {
    Serial.println("Aufnahme:");
  }  
  if (Ps3.event.button_down.up) {
    Serial.println("schneller");
    motorSpeed+=20;   
    if (motorSpeed > 1023) {
      motorSpeed = 1023; 
    } 
    ledcWrite(LEDC_KANAL_0, motorSpeed);      
  }  
  if (Ps3.event.button_down.down) {
    Serial.println("langsamer");
    motorSpeed-=20;
    if (motorSpeed < MINIMUMSPEED) {
      motorSpeed = MINIMUMSPEED; 
    }
    ledcWrite(LEDC_KANAL_0, motorSpeed);     
  }
  if (Ps3.event.button_down.l3) {
    Serial.println("Position merken - left stick button");
    servoPosSich = map(leftStickX,-128,127,0,180);  
  }       
  if (Ps3.event.button_down.r3) {
    Serial.println("Motor Start / Stop right stick button");
    motorRun =! motorRun;
    Serial.print ("Motor ");
    Serial.println (motorRun);      
    if (motorRun) {
    } else {     
      motorenStop ();
    }              
  }
  if (abs(Ps3.event.analog_changed.stick.lx) + 
      abs(Ps3.event.analog_changed.stick.ly) > 2 ){
    //Serial.print("Moved the left stick:");
    //Serial.print(" x="); 
    //Serial.print(Ps3.data.analog.stick.lx, DEC);
    //Serial.print(" y="); 
    //Serial.print(Ps3.data.analog.stick.ly, DEC);
    //Serial.println();
    leftStickX = Ps3.data.analog.stick.lx;  
  }
//                        -1 / -128
//                           -
//             -128 / -1   | O |   127 / -1
//                           -
//                        -1 / 127 
  if (abs(Ps3.event.analog_changed.stick.rx) + 
      abs(Ps3.event.analog_changed.stick.ry) > 2 ){
    //Serial.print("Moved the right stick:");
    //Serial.print(" x="); 
    //Serial.print(Ps3.data.analog.stick.rx, DEC);
    //Serial.print(" y="); 
    //Serial.print(Ps3.data.analog.stick.ry, DEC);
    int ix = Ps3.data.analog.stick.rx;
    int iy = Ps3.data.analog.stick.ry;
    if (ix == -128 && iy == -1) {        
      links ();     
    } else {
      if (ix == -1 && iy == -128) {        
         vorwaerts (); 
       } else {
         if (ix == 127 && iy == -1) {        
           rechts ();            
         } else {
           if (ix == -1 && iy == 127) {        
             rueckwaerts ();  
           }  
         } 
       }
     }
   }
} 
void onConnect(){
    Serial.println("Connected.");
}
void setup() {
  Serial.begin(115200);    
  // Motor-Control-Pins als Output öffnen
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT); 
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);   
  // Vorwärtslauf  
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);  
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);    
  ledcSetup(LEDC_KANAL_0, LEDC_FREQ, LEDC_AUFLOESUNG); 
  ledcAttachPin(ENA, LEDC_KANAL_0);
  ledcSetup(LEDC_KANAL_1, LEDC_FREQ, LEDC_AUFLOESUNG); 
  ledcAttachPin(ENB, LEDC_KANAL_1);  
  myServo.attach(SERVO_PIN, LEDC_KANAL_2); 
  Ps3.attach(notify);
  Ps3.attachOnConnect(onConnect);
  Ps3.begin("00:32:85:7e:14:b6");
  Serial.println("Start.");
}
void loop(){
  if(!Ps3.isConnected())
    return;
  if (servoPosSich != 90) {
    servoPos = servoPosSich;
  } else {
    servoPos = map(leftStickX,-128,127,0,180); 
  }
  myServo.write(servoPos);        
  delay(1000);
}