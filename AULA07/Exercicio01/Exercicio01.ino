#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

static const uint8_t POT_PIN = 34;
static const uint8_t FILA_TAMANHO = 5;
static const uint16_t ADC_MAXIMO = 4095;
static const float TENSAO_REFERENCIA = 3.3f;

QueueHandle_t filaPotenciometro;
TaskHandle_t taskLeituraHandle;
TaskHandle_t taskSerialHandle;

void taskLeituraPotenciometro(void *pvParameters);
void taskSerial(void *pvParameters);

void setup() {
  Serial.begin(115200);
  delay(500);

  analogReadResolution(12);
  pinMode(POT_PIN, INPUT);

  filaPotenciometro = xQueueCreate(FILA_TAMANHO, sizeof(uint16_t));

  if (filaPotenciometro == NULL) {
    Serial.println("Erro: nao foi possivel criar a fila.");
    while (true) {
      delay(1000);
    }
  }

  BaseType_t tarefaLeituraCriada = xTaskCreate(
    taskLeituraPotenciometro,
    "LeituraPot",
    configMINIMAL_STACK_SIZE + 1024,
    NULL,
    1,
    &taskLeituraHandle
  );

  BaseType_t tarefaSerialCriada = xTaskCreate(
    taskSerial,
    "SerialPot",
    configMINIMAL_STACK_SIZE + 1024,
    NULL,
    1,
    &taskSerialHandle
  );

  if (tarefaLeituraCriada != pdPASS || tarefaSerialCriada != pdPASS) {
    Serial.println("Erro: nao foi possivel criar as tasks.");
    while (true) {
      delay(1000);
    }
  }

  Serial.println("Sistema iniciado.");
  Serial.println("Leitura do potenciometro no GPIO 34 usando fila de 5 posicoes.");
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}

void taskLeituraPotenciometro(void *pvParameters) {
  (void) pvParameters;

  while (true) {
    uint16_t leituraADC = analogRead(POT_PIN);

    // Bloqueia a task emissora se a fila estiver cheia.
    xQueueSend(filaPotenciometro, &leituraADC, portMAX_DELAY);

    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void taskSerial(void *pvParameters) {
  (void) pvParameters;

  uint16_t leituraRecebida = 0;

  while (true) {
    if (xQueueReceive(filaPotenciometro, &leituraRecebida, portMAX_DELAY) == pdTRUE) {
      float tensao = (static_cast<float>(leituraRecebida) * TENSAO_REFERENCIA) / ADC_MAXIMO;
      float percentual = (static_cast<float>(leituraRecebida) * 100.0f) / ADC_MAXIMO;

      Serial.print("ADC: ");
      Serial.print(leituraRecebida);
      Serial.print(" | Tensao: ");
      Serial.print(tensao, 2);
      Serial.print(" V | Potenciometro: ");
      Serial.print(percentual, 1);
      Serial.println("%");

      vTaskDelay(pdMS_TO_TICKS(500));
    }
  }
}
