#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#define  LED  2
static SemaphoreHandle_t binSemaphore;
// LedFastTask
void LedFast_task_code (void *parameters) {
  while (1) {
    Serial.println("LedFastTask  Wait");
    xSemaphoreTake(binSemaphore,portMAX_DELAY);
    Serial.println("LedFastTask  Take");
    for (int i = 1; i < 10; i++) {
      digitalWrite(LED,HIGH);
      delay(100);
      digitalWrite(LED,LOW);
      delay(100);
    }
    xSemaphoreGive(binSemaphore);
    Serial.println("LedFastTask  Give\n");
    delay(2000);
  }
}
// LedSlowTask
void LedSlow_task_code (void *parameters) {
  while (1) {
    Serial.println("                 LedSlowTask   Wait");
    xSemaphoreTake(binSemaphore,portMAX_DELAY);
    Serial.println("                 LedSlowTask   Take");
    for (int i = 1; i < 8; i++) {
      digitalWrite(LED,HIGH);
      delay(300);
      digitalWrite(LED,LOW);
      delay(300);
    }
    xSemaphoreGive(binSemaphore);
    Serial.println("                 LedSlowTask   Give\n");     
    delay(1000);
  }
}
void setup() {
  Serial.begin(115200);
  delay (1000);                    // warten auf Bereitschaft Serial Port
  Serial.println("setup");
  pinMode(LED ,OUTPUT);
  
  // Create semaphores before starting tasks
  binSemaphore = xSemaphoreCreateBinary();
 if (binSemaphore != NULL) {
   Serial.println("Semaphore erstellt");
 } else {
   while(1);  
 }  
  // Start LedFast task
  xTaskCreatePinnedToCore (
                  LedFast_task_code,
                  "LedFstTask",
                  1024,
                  NULL,
                  1,
                  NULL,
                  1);
  delay (100);
  // Start LedSlow task
  xTaskCreatePinnedToCore (
                  LedSlow_task_code,
                  "LedSlowTask",
                  1024,
                  NULL,
                  1,
                  NULL,
                  1);
  delay (100);
  // Semaphore auslösen
  xSemaphoreGive(binSemaphore);
}
void loop() {
  delay(10000);
}