#include <Arduino.h>
TaskHandle_t task_handle_1;
TaskHandle_t task_handle_2;
// Task Codes
void task_code_1 ( void * pvParameters ){
  Serial.print  ("Task1 ausgeführt in Kern ");
  Serial.println(xPortGetCoreID());

  for(;;){
    Serial.println("Meldung von Task1");
    delay(2000);
  } 
}
void task_code_2( void * pvParameters ){
  Serial.print  ("Task2 ausgeführt in Kern ");
  Serial.println(xPortGetCoreID());
  for(;;){
    Serial.println("Meldung von Task2");
    delay(1000);
  }
}
void setup() {
  Serial.begin(115200); 
  delay (100);
  Serial.print  ("Anzahl Tasks vor Create Tasks  ");             
  Serial.println(uxTaskGetNumberOfTasks()); 
  xTaskCreatePinnedToCore(
                    task_code_1,
                    "Task1",
                    10000,
                    NULL,
                    1,
                    &task_handle_1,
                    0);
  delay(500);
  xTaskCreatePinnedToCore(
                    task_code_2,
                    "Task2",
                    10000,
                    NULL,
                    1,
                    &task_handle_2,
                    1);
  delay(500); 
  Serial.print  ("Anzahl Tasks nach Create Tasks  ");             
  Serial.println(uxTaskGetNumberOfTasks());     
}
void loop() {
  delay(10000);  
}