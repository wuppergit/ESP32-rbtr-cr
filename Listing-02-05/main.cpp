#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
static SemaphoreHandle_t binSemaphore = NULL;
void testSemaphore() {
  if (binSemaphore != NULL) {
    Serial.println("     binSemaphore != NULL / Semaphore vorhanden");
    if (xSemaphoreTake (binSemaphore, ( TickType_t ) 10 ) == pdTRUE ) {
      Serial.println("     Take");
      xSemaphoreGive (binSemaphore);
      Serial.println("     Give");
    } else {
       Serial.println("     Take fehlgeschlagen");
    }
  } else {
    Serial.println("     binSemaphore == NULL / keine Semaphore");
  }
  Serial.println();
}
void setup() {
  Serial.begin(115200);
  delay (100);
  Serial.println("setup");
  // Step 1: Semaphore noch nicht erstellt
  Serial.println("Semaphore noch nicht erstellt");
  testSemaphore();
  // Step 2: Semaphore erstellen
  Serial.println("Semaphore erstellen");
  binSemaphore = xSemaphoreCreateBinary();
  if (binSemaphore != NULL) {
    Serial.println("     Semaphore erstellt"); 
  } else {
    Serial.println("     Semaphore erstellen fehlgeschlagen");
    while(1);  
  }
  Serial.println();  
  // Step 3: Testzugriff (Take) auf erstellte Semaphore
  Serial.println("Testzugriff (Take) auf erstellte Semaphore");
  testSemaphore();
  // Step 4: Semaphore auslösen
  Serial.println("erstellte Semaphore auslösen (Give)");
  if (xSemaphoreGive(binSemaphore) != pdTRUE )      {
    Serial.println("     xSemaphoreGive fehlgeschlagen (!= pdTRUE)");      
  } else {
    Serial.println("     xSemaphoreGive erfolgreich    (== pdTRUE)");
  }
  Serial.println();
  // Step 5: Zugriff (Take) auf ausgelöste Semaphore
  Serial.println("Zugriff (Take) auf ausgelöste Semaphore");
  testSemaphore();
}
void loop() {
  delay(10000);
}