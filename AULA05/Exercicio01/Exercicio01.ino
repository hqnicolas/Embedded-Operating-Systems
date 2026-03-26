#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED 2
#define POT1 34
#define POT2 35

TaskHandle_t taskLED;
TaskHandle_t taskSerial;
TaskHandle_t taskControle;

int estadoAtual = -1;

void tarefaLED(void *pvParameters) {
  while (true) {
    digitalWrite(LED, HIGH);
    vTaskDelay(pdMS_TO_TICKS(500));
    digitalWrite(LED, LOW);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void tarefaSerial(void *pvParameters) {
  int contador = 0;

  while (true) {
    contador++;
    Serial.print("Contador: ");
    Serial.println(contador);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void tarefaControle(void *pvParameters) {
  while (true) {
    int valorPOT2 = analogRead(POT2);

    int tempo = map(valorPOT2, 0, 4095, 100, 1000);

    digitalWrite(LED, HIGH);
    vTaskDelay(pdMS_TO_TICKS(tempo));
    digitalWrite(LED, LOW);
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

      vTaskSuspend(taskLED);
      vTaskSuspend(taskSerial);
      vTaskSuspend(taskControle);

      digitalWrite(LED, LOW);

      switch (estadoAtual) {
        case 0:
          Serial.println("Estado: Tudo suspenso");
          break;

        case 1:
          Serial.println("Estado: LED 500ms");
          vTaskResume(taskLED);
          break;

        case 2:
          Serial.println("Estado: Contador Serial");
          vTaskResume(taskSerial);
          break;

        case 3:
          Serial.println("Estado: Controle POT2");
          vTaskResume(taskControle);
          break;
      }
    }

    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(LED, OUTPUT);

  xTaskCreate(tarefaLED, "Task LED", 2048, NULL, 1, &taskLED);
  xTaskCreate(tarefaSerial, "Task Serial", 2048, NULL, 1, &taskSerial);
  xTaskCreate(tarefaControle, "Task Controle", 2048, NULL, 1, &taskControle);
  xTaskCreate(tarefaPrincipal, "Task Principal", 2048, NULL, 2, NULL);

  vTaskSuspend(taskLED);
  vTaskSuspend(taskSerial);
  vTaskSuspend(taskControle);
}

void loop() {
}