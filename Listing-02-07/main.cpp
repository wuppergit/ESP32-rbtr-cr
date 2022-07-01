#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
QueueHandle_t  q = NULL;
void producer_task_code(void *pvParameter){
  unsigned long counter=1;
  if (q == NULL){
    Serial.println("Producer_task: Queue steht nicht bereit");
    return;
  }
  delay(50);
  Serial.print  ("start producer_task-Task in Kern ");
  Serial.println(xPortGetCoreID());
  delay(50);
  while(1){
    Serial.print  ("von Producer an Queue gesendeter Wert :     ");
    Serial.println(counter);    
    xQueueSend(q,(void *)&counter,(TickType_t )0); // add the counter value to the queue
    counter++;
    delay(1000);
  }
}
void consumer_task_code(void *pvParameter) {
  unsigned long counter;
  if (q == NULL){
    Serial.println("Consumer_task: Queue steht nicht bereit");   
    return;
  }
  delay(50);
  Serial.print  ("start consumer_task-Task in Kern ");
  Serial.println(xPortGetCoreID());
  delay(50); 
  while(1){
    xQueueReceive(q,&counter,(TickType_t )(1000/portTICK_PERIOD_MS)); 
    Serial.print  ("von Consumer über Queue empfangener Wert :  ");    
    Serial.println(counter);    
    delay(500);
  }
} 
void setup() {
  Serial.begin(115200);
  Serial.println("setup");
  q=xQueueCreate(20,sizeof(unsigned long));
  if(q != NULL){
    Serial.println("Queue erstellt");
    delay(1000);
    xTaskCreate(&producer_task_code,"producer_task",2048,NULL,5,NULL);
    Serial.println("producer task  gestartet");
    delay(10);
    xTaskCreate(&consumer_task_code,"consumer_task",2048,NULL,5,NULL);
    Serial.println("consumer task  gestartet");
  } else {
    Serial.println("Queue creation fehlgeschlagen");
  }
  Serial.println();
} 
void loop() {
  delay(10000); 
}