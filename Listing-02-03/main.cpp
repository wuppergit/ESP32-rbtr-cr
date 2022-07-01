#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#define LED_PIN        GPIO_NUM_2
void setup() {
  // LED-GPIO aus GPIO-Matrix auswählen
  gpio_pad_select_gpio(LED_PIN);
  // LED-GPIO als Ouput öffnen
  gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
}
void loop() {
  // GPIO auf LOW setzen
  gpio_set_level(LED_PIN, 0);
  // 1000 mSek warten
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  // GPIO auf HIGH setzen
  gpio_set_level(LED_PIN, 1);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}