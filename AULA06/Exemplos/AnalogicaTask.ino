#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define ledStatus 21
#define ledMedio  19
#define ledAnalog 18

volatile bool rodarTaskMedio = true;
volatile bool rodarTaskSerial = true;
volatile bool rodarTaskAnalogica = true;

TaskHandle_t task1Handle = NULL;
TaskHandle_t task2Handle = NULL;
TaskHandle_t task3Handle = NULL;
TaskHandle_t task4Handle = NULL;
TaskHandle_t task5Handle = NULL;

/* Protótipos das Tasks */
void vTaskControle(void *pvParameters);
void vTaskStatus(void *pvParameters);
void vTaskLedMedio(void *pvParameters);
void vTaskSerial(void *pvParameters);
void vTaskLedAnalogica(void *pvParameters);

void setup() {
  Serial.begin(9600);
  
  xTaskCreate(vTaskControle, "TASK0", 2048, NULL, 1, &task1Handle);
  xTaskCreate(vTaskStatus, "TASK1", 1024, NULL, 2, &task2Handle);
  xTaskCreate(vTaskLedMedio, "TASK2", 1024, NULL, 10, &task3Handle);
  xTaskCreate(vTaskSerial, "TASK3", 2048, NULL, 15, &task4Handle);
  xTaskCreate(vTaskLedAnalogica, "TASK4", 1024, NULL, 20, &task5Handle);
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(3000));
}

void vTaskControle(void *pvParameters) {
  int valorAnalog = 0;

  while (1) {
    valorAnalog = analogRead(36);
    
    Serial.print("Analog: ");
    Serial.println(valorAnalog);
    
    if (valorAnalog < 500) {
      rodarTaskMedio = false;
      rodarTaskSerial = false;
      rodarTaskAnalogica = false;
    }
    else if (valorAnalog >= 500 && valorAnalog < 1000) {
      rodarTaskMedio = true;
      rodarTaskSerial = false;
      rodarTaskAnalogica = false;
    }
    else if (valorAnalog >= 1000 && valorAnalog < 2000) {
      rodarTaskMedio = true;
      rodarTaskSerial = true;
      rodarTaskAnalogica = false;
    }
    else if (valorAnalog >= 2000) {
      rodarTaskMedio = true;
      rodarTaskSerial = true;
      rodarTaskAnalogica = true;
    }
    
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void vTaskStatus(void *pvParameters) {
    pinMode(ledStatus, OUTPUT);

    while (1) {
      digitalWrite(ledStatus, !digitalRead(ledStatus));
      vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void vTaskLedMedio(void *pvParameters) {
    pinMode(ledMedio, OUTPUT);

    while (1) {
      if (rodarTaskMedio) {
        digitalWrite(ledMedio, !digitalRead(ledMedio));
      } else {
        digitalWrite(ledMedio, LOW); 
      }
      vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void vTaskSerial(void *pvParameters) {
  int cont = 0;

  while (1) {
    if (rodarTaskSerial) {
      Serial.print("Contadorium: ");
      Serial.println(cont++);
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void vTaskLedAnalogica(void *pvParameters) {
  int analog2 = 0;
  int tempo = 0;
  pinMode(ledAnalog, OUTPUT); 
  
  while (1) {
      if (rodarTaskAnalogica) {
        analog2 = analogRead(39);
        tempo = map(analog2, 0, 4095, 100, 1000);
        digitalWrite(ledAnalog, !digitalRead(ledAnalog));
        vTaskDelay(pdMS_TO_TICKS(tempo));
      } else {
        digitalWrite(ledAnalog, LOW); 
        vTaskDelay(pdMS_TO_TICKS(100)); 
      }
  }
}