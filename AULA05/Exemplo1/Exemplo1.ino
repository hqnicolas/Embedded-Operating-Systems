/*Bibliotecas FreeRTOS */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
/*Quando for usar fila inclui fila*/ 
/*Quando for usar semaforo inclui semaforo*/ 

/*mapeamento de pinos*/ 
#define LED 2

/* Variáveis para armazenamento do handle das tasks*/
TaskHandle_t taks1Handle = NULL;
TaskHandle_t taks2Handle = NULL;

/*protítipos das Tasks*/
void vTask1(void *pvParameters);
void vTask2(void *pvParameters);

/*função setup*/
void setup() {
  Serial.begin(9600); //configura comunicação serial com baudrate de 9600
  /*criação das tasks*/
  xTaskCreate(vTask1,"TASK1",configMINIMAL_STACK_SIZE,NULL,1,&taks1Handle);
  /*isso aloca a task na memoria*/ 
  xTaskCreate(vTask2,"TASK2",configMINIMAL_STACK_SIZE+1024,NULL,2,&taks1Handle);
  /*Parâmetros: pvTaskCode: vTask1 - ponteiro para função que será chamada
                pcName: "TASK1" - label que indica qual a task (depuração)
                usStackDepth: configMINIMAL_STACK - tamanho da pilha a ser usada
                pvParameters: Ponteiro que servirá de parâmetro para a tarefa que está sendo criada
                uxPriority: A prioridade na qual a tarefa criada será executada
                pvCreatedTask: Usado para passar um identificador para a tarefa criada.*/
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
    pinMode(LED,OUTPUT);

    while (1)
    {
      digitalWrite(LED,!digitalRead(LED));
      vTaskDelay(pdMS_TO_TICKS(200));
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
