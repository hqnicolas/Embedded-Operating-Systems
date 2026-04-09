#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

static const uint8_t POTENCIOMETRO = 34;
static const uint8_t SEG_A = 18;
static const uint8_t SEG_B = 5;
static const uint8_t SEG_C = 21;
static const uint8_t SEG_D = 3;
static const uint8_t SEG_E = 1;
static const uint8_t SEG_F = 23;
static const uint8_t SEG_G = 22;
static const uint8_t SEG_DP = 19;
static const uint8_t DISPLAY_DEZENA = 16;
static const uint8_t DISPLAY_UNIDADE = 17;

static const uint8_t TAMANHO_FILA = 5;
static const uint16_t ADC_MAXIMO = 4095;
static const float TENSAO_REFERENCIA = 3.3f;

const byte digitos7seg[10][8] = {
  {1, 1, 1, 1, 1, 1, 0, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1, 0}, // 2
  {1, 1, 1, 1, 0, 0, 1, 0}, // 3
  {0, 1, 1, 0, 0, 1, 1, 0}, // 4
  {1, 0, 1, 1, 0, 1, 1, 0}, // 5
  {1, 0, 1, 1, 1, 1, 1, 0}, // 6
  {1, 1, 1, 0, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1, 0}, // 8
  {1, 1, 1, 1, 0, 1, 1, 0}  // 9
};

const uint8_t pinosSegmentos[8] = {
  SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_DP
};

QueueHandle_t filaPotenciometro;
volatile uint8_t valorDisplay = 0;

void taskLeituraPotenciometro(void *pvParameters);
void taskProcessamento(void *pvParameters);
void taskDisplay(void *pvParameters);

void apagarDisplays() {
  digitalWrite(DISPLAY_DEZENA, LOW);
  digitalWrite(DISPLAY_UNIDADE, LOW);
}

void escreverDigito(uint8_t digito) {
  for (uint8_t i = 0; i < 8; i++) {
    digitalWrite(pinosSegmentos[i], digitos7seg[digito][i]);
  }
}

void exibirNumeroMux(uint8_t numero) {
  uint8_t dezena = numero / 10;
  uint8_t unidade = numero % 10;

  apagarDisplays();
  escreverDigito(dezena);
  digitalWrite(DISPLAY_DEZENA, HIGH);
  vTaskDelay(pdMS_TO_TICKS(2));

  apagarDisplays();
  escreverDigito(unidade);
  digitalWrite(DISPLAY_UNIDADE, HIGH);
  vTaskDelay(pdMS_TO_TICKS(2));
}

void setup() {
  Serial.begin(115200);
  delay(500);

  analogReadResolution(12);
  pinMode(POTENCIOMETRO, INPUT);

  for (uint8_t i = 0; i < 8; i++) {
    pinMode(pinosSegmentos[i], OUTPUT);
    digitalWrite(pinosSegmentos[i], LOW);
  }

  pinMode(DISPLAY_DEZENA, OUTPUT);
  pinMode(DISPLAY_UNIDADE, OUTPUT);
  apagarDisplays();

  filaPotenciometro = xQueueCreate(TAMANHO_FILA, sizeof(uint16_t));

  if (filaPotenciometro == NULL) {
    Serial.println("Erro: nao foi possivel criar a fila.");
    while (true) {
      delay(1000);
    }
  }

  if (xTaskCreate(taskLeituraPotenciometro, "LeituraPot", 2048, NULL, 1, NULL) != pdPASS ||
      xTaskCreate(taskProcessamento, "ProcessaFila", 3072, NULL, 1, NULL) != pdPASS ||
      xTaskCreate(taskDisplay, "DisplayMux", 2048, NULL, 2, NULL) != pdPASS) {
    Serial.println("Erro: nao foi possivel criar as tasks.");
    while (true) {
      delay(1000);
    }
  }

  Serial.println("Sistema iniciado.");
  Serial.println("Fila de 5 posicoes, serial e display de 7 segmentos com multiplexacao.");
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}

void taskLeituraPotenciometro(void *pvParameters) {
  (void) pvParameters;

  while (true) {
    uint16_t leituraADC = analogRead(POTENCIOMETRO);
    xQueueSend(filaPotenciometro, &leituraADC, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(150));
  }
}

void taskProcessamento(void *pvParameters) {
  (void) pvParameters;
  uint16_t leituraRecebida = 0;

  while (true) {
    if (xQueueReceive(filaPotenciometro, &leituraRecebida, portMAX_DELAY) == pdTRUE) {
      float tensao = (static_cast<float>(leituraRecebida) * TENSAO_REFERENCIA) / ADC_MAXIMO;
      float percentual = (static_cast<float>(leituraRecebida) * 100.0f) / ADC_MAXIMO;

      valorDisplay = map(leituraRecebida, 0, ADC_MAXIMO, 0, 99);

      Serial.print("ADC: ");
      Serial.print(leituraRecebida);
      Serial.print(" | Tensao: ");
      Serial.print(tensao, 2);
      Serial.print(" V | Potenciometro: ");
      Serial.print(percentual, 1);
      Serial.print("% | Display: ");
      Serial.println(valorDisplay);

      vTaskDelay(pdMS_TO_TICKS(350));
    }
  }
}

void taskDisplay(void *pvParameters) {
  (void) pvParameters;

  while (true) {
    exibirNumeroMux(valorDisplay);
  }
}
