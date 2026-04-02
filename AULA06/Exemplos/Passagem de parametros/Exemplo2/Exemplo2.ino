/*Bibliotecas FreeRTOS */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*mapeamento de pinos*/ 
#define LED1 2
#define LED2 4

typedef struct{
  int pino;
  int tempo;
}TaskParams_t;

static TaskParams_t params1 = {
  .pino = LED1,
  .tempo = 371
}

/* Variáveis para armazenamento do handle das tasks*/
TaskHandle_t taks1Handle = NULL;
TaskHandle_t taks2Handle = NULL;

/*protótipos das Tasks*/
void vTask1(void *pvParameters);
void vTask2(void *pvParameters);

/*função setup*/
void setup() {
  Serial.begin(9600); //configura comunicação serial com baudrate de 9600
  /*criação das tasks*/
  xTaskCreate(vTask1,"TASK1",configMINIMAL_STACK_SIZE,&params1,1,&taks1Handle);
  xTaskCreate(vTask1,"TASK2",configMINIMAL_STACK_SIZE+1024,(void*)LED2,2,&taks1Handle);

}

/*função loop*/
void loop() {
  vTaskDelay(3000); //libera a CPU por 3 seg
}

/*
vTask1 
inverte LED em intervalos de 200 ms
*/
void vTask1(void *pvParameters)
{
    int pin;
    int tempo;

    TaskParams_t *parameters = (TaskParams_t*) pvParameters;
    pin = parameters->pino;
    tempo = parameters->tempo;

    pinMode(pin,OUTPUT);

    while (1)
    {
      digitalWrite(pin,!digitalRead(pin));
      vTaskDelay(pdMS_TO_TICKS(tempo));
    }
}

/*
vTask2 
imprime valor de contagem a cada 1 seg
*/
void vTask2(void *pvParameters)
{
  int cont = 0;

  while (1)
  {
    Serial.println("Task 2: " + String(cont++));
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
