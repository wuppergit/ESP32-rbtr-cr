#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#define BUTTON_PIN 32
TaskHandle_t task_handle_1 = NULL;
void task_code_1(void * pvParameters) {
  while (1) {
    if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) != 0) {
      Serial.println("Button gedrückt");
    }
  }
}
void IRAM_ATTR isrHandler() {  
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 200) {
    vTaskNotifyGiveFromISR(task_handle_1, NULL);
  }
  last_interrupt_time = interrupt_time;
}
void setup() {
  Serial.begin(115200);
  Serial.println("start - mit Butten ISR-auslösen ");
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), isrHandler, RISING);
  xTaskCreate(task_code_1, "Task 1", 2048, NULL, 1, &task_handle_1);
}
void loop() {
}