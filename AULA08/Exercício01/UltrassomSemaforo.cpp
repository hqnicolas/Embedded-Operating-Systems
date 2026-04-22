#include <Arduino.h>
#include <Ultrasonic.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define LED1 4
#define LED2 0
#define LED3 2
#define LED4 15
#define BOTAO1 4
#define BOTAO2 0
#define BOTAO3 2
#define BOTAO4 15
#define RGB_RED 25
#define RGB_GREEN 26
#define RGB_BLUE 27
#define RELE 13
#define DHT11_PIN 33
#define LDR 39
#define POTENCIOMETRO 34
#define ULTRASSONIC_TRIG 32
#define ULTRASSONIC_ECHO 35

SemaphoreHandle_t semaforoUltrassom;
TaskHandle_t taskUltrassomHandle;

Ultrasonic ultrasonic(ULTRASSONIC_TRIG, ULTRASSONIC_ECHO);

void vTaskUltrassom(void *pvParameters);
void IRAM_ATTR ISR_Botao4();

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(LED3, OUTPUT);
  pinMode(BOTAO4, INPUT_PULLUP);

  semaforoUltrassom = xSemaphoreCreateBinary();

  if (semaforoUltrassom == NULL) {
    Serial.println("Erro: nao foi possivel criar o semaforo.");
    while (true) {
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }

  attachInterrupt(digitalPinToInterrupt(BOTAO4), ISR_Botao4, FALLING);

  xTaskCreate(
    vTaskUltrassom,
    "TaskUltrassom",
    configMINIMAL_STACK_SIZE + 2048,
    NULL,
    2,
    &taskUltrassomHandle
  );

  Serial.println("Sistema iniciado.");
  Serial.println("Pressione o BOTAO4 para medir a distancia no ultrassom.");
}

void loop() {
  digitalWrite(LED3, !digitalRead(LED3));
  vTaskDelay(pdMS_TO_TICKS(300));
}

void IRAM_ATTR ISR_Botao4() {
  BaseType_t xHighPriorityTaskWoken = pdFALSE;

  xSemaphoreGiveFromISR(semaforoUltrassom, &xHighPriorityTaskWoken);

  if (xHighPriorityTaskWoken == pdTRUE) {
    portYIELD_FROM_ISR();
  }
}

void vTaskUltrassom(void *pvParameters) {
  (void) pvParameters;

  while (true) {
    xSemaphoreTake(semaforoUltrassom, portMAX_DELAY);

    int distancia = ultrasonic.read();

    if (distancia < 0) {
      Serial.println("Leitura invalida do ultrassom.");
    } else {
      Serial.print("Distancia medida: ");
      Serial.print(distancia);
      Serial.println(" cm");
    }

    vTaskDelay(pdMS_TO_TICKS(250));
  }
}
