/*inclusão das Biblioteca do FreeRTOS*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED 2
#define BT 0

TaskHandle_t xTaskTrataBTHandle;

void vTaskTrataBT(void *pvParameters);

void callBackBT(void){
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  vTaskNotifyGiveFromISR(xTaskTrataBTHandle,&xHigherPriorityTaskWoken);

}

void setup() {
  Serial.begin(9600);
  pinMode(LED,OUTPUT);
  pinMode(BT,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BT),callBackBT,FALLING);

  xTaskCreate(vTaskTrataBT,"TASK BT",configMINIMAL_STACK_SIZE,NULL,1,&xTaskTrataBTHandle);
  
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}


void vTaskTrataBT(void *pvParameters){
  uint32_t quantidade = 0;
  while (1)
  {
    quantidade = ulTaskNotifyTake(pdFALSE,portMAX_DELAY);

    Serial.println("Tratando ISR botao: " + String(quantidade));
    digitalWrite(LED,!digitalRead(LED));
    delay(2000);
  }
}