#include <Arduino.h>
#include <Servo.h>               // Servo-Bibliothek
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/mcpwm.h"
#define SERVO_MIN_PULSEWIDTH_US    (1000) // Minimum Pulsweite in Microsekunden
#define SERVO_MAX_PULSEWIDTH_US    (2000) // Maximum Pulsweite in Microsekunden
#define SERVO_MAX_DEGREE           (90)   // Maximum-Winkel in Rotations-Grad des Servos
#define SERVO_PIN                  (13)   // GPIO13 für den Servo
uint32_t winkelUS, count;
static uint32_t calcPulsweite (uint32_t degree_of_rotation) {
    uint32_t cal_pulsewidth = 0;
    cal_pulsewidth = (SERVO_MIN_PULSEWIDTH_US + (((SERVO_MAX_PULSEWIDTH_US - SERVO_MIN_PULSEWIDTH_US) * (degree_of_rotation)) / (SERVO_MAX_DEGREE)));
    return cal_pulsewidth;
}
void setup() {
  Serial.begin(115200);
  // initialisieren des GPIOs für mcpwm
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, SERVO_PIN);
  // mcpwm-Konfiguration, muss zwischen gpio_init und mcpwm_init sein
  mcpwm_config_t pwm_config;
  pwm_config.frequency = 50;   // Frequenz
  pwm_config.cmpr_a = 0;       // duty Zyklus PWMxA = 0
  pwm_config.cmpr_b = 0;       // duty Zyklus PWMxB = 0    
  pwm_config.counter_mode = MCPWM_UP_COUNTER;
  pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
  // initialisieren mcpwm-Unit
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);  
}
void loop() {
  for (count = 0; count < SERVO_MAX_DEGREE; count++) {
    // Pulweite in Mikrosekunden berechnen
    winkelUS = calcPulsweite(count);
    // positionieren 
    mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, winkelUS);
    // etwas für das Psositioniren des Servos warten
    vTaskDelay(10); 
  }
}