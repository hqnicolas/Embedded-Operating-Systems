#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED_STATUS 21
#define LED_PISCA 19
#define LED_ANALOGICO 18
#define POT1 36
#define POT2 39

TaskHandle_t taskPisca;
TaskHandle_t taskAnalogica;
TaskHandle_t taskControle;

int estadoAtual = -1;

void tarefaPisca(void *pvParameters) {
  while (true) {
    digitalWrite(LED_PISCA, HIGH);
    vTaskDelay(pdMS_TO_TICKS(500));
    digitalWrite(LED_PISCA, LOW);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void tarefaAnalogica(void *pvParameters) {
  while (true) {
    int valorPOT2 = analogRead(POT2);
    int brilho = map(valorPOT2, 0, 4095, 0, 255);
    analogWrite(LED_ANALOGICO, brilho);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void tarefaControle(void *pvParameters) {
  while (true) {
    int valorPOT2 = analogRead(POT2);
    int tempo = map(valorPOT2, 0, 4095, 100, 1000);
    
    digitalWrite(LED_STATUS, HIGH);
    vTaskDelay(pdMS_TO_TICKS(tempo));
    digitalWrite(LED_STATUS, LOW);
    vTaskDelay(pdMS_TO_TICKS(tempo));
  }
}

void tarefaPrincipal(void *pvParameters) {
  while (true) {
    int valorPOT1 = analogRead(POT1);

    int novoEstado;

    if (valorPOT1 < 500) {
      novoEstado = 0;
    } else if (valorPOT1 < 1000) {
      novoEstado = 1;
    } else if (valorPOT1 < 2000) {
      novoEstado = 2;
    } else {
      novoEstado = 3;
    }

    if (novoEstado != estadoAtual) {
      estadoAtual = novoEstado;
      vTaskSuspend(taskPisca);
      vTaskSuspend(taskAnalogica);
      vTaskSuspend(taskControle);
      digitalWrite(LED_PISCA, LOW);
      digitalWrite(LED_ANALOGICO, LOW);
      digitalWrite(LED_STATUS, LOW);

      switch (estadoAtual) {
        case 0:
          Serial.println("Estado 0: Tudo suspenso");
          break;

        case 1:
          Serial.println("Estado 1: LED Pisca (500ms)");
          vTaskResume(taskPisca);
          break;

        case 2:
          Serial.println("Estado 2: LED Analógico (POT2 controla brilho)");
          vTaskResume(taskAnalogica);
          break;

        case 3:
          Serial.println("Estado 3: LED Status (POT2 controla velocidade)");
          vTaskResume(taskControle);
          break;
      }
    }

    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(LED_STATUS, OUTPUT);
  pinMode(LED_PISCA, OUTPUT);
  pinMode(LED_ANALOGICO, OUTPUT);

  xTaskCreate(tarefaPisca, "Task Pisca", 2048, NULL, 1, &taskPisca);
  xTaskCreate(tarefaAnalogica, "Task Analogica", 2048, NULL, 1, &taskAnalogica);
  xTaskCreate(tarefaControle, "Task Controle", 2048, NULL, 1, &taskControle);
  xTaskCreate(tarefaPrincipal, "Task Principal", 2048, NULL, 2, NULL);
  vTaskSuspend(taskPisca);
  vTaskSuspend(taskAnalogica);
  vTaskSuspend(taskControle);
}

void loop() {
}