/*Bibliotecas FreeRTOS */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

/*mapeamento de pinos*/ 
#define LED1 2
#define LED2 4

#define butao1 14
#define butao2 12


/* Variáveis para armazenamento do handle das tasks*/
TaskHandle_t task1Handle = NULL;
TimerHandle_t xTimer1, xTimer2;


/*protítipos das Tasks*/
void vTask1(void *pvParameters);
void callBackTimer1(TimerHandle_t xTimer);
void callBackTimer2(TimerHandle_t xTimer);


void setup() {
  Serial.begin(9600);
    
  xTaskCreate(vTask1,"TASK1",configMINIMAL_STACK_SIZE,NULL,1,&task1Handle);
  xTimer1 = xTimerCreate("TIMER1",pdMS_TO_TICKS(1000),pdTRUE,0,callBackTimer1);
  xTimer2 = xTimerCreate("TIMER2",pdMS_TO_TICKS(2000),pdFALSE,0,callBackTimer2);
  pinMode(LED2,OUTPUT);
      pinMode(LED1,OUTPUT);
  pinMode(butao1, INPUT_PULLDOWN);
  pinMode(butao2, INPUT_PULLDOWN);
  xTimerStart(xTimer1,0);
}

void loop() {
  vTaskDelay(3000);
}


void vTask1(void *pvParameters)
{
    while (1)
    {
      if (digitalRead(butao1)){
        xTimerStop(xTimer1, pdMS_TO_TICKS(2000));
      }
      if (digitalRead(butao2)){
        xTimerStart(xTimer2, pdMS_TO_TICKS(5000));
      }
      vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void callBackTimer2(TimerHandle_t xTimer)
{
    digitalWrite(LED1,!digitalRead(LED1));
}
void callBackTimer1(TimerHandle_t xTimer)
{
    digitalWrite(LED2,!digitalRead(LED2));
}
