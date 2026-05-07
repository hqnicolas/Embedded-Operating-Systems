/* Inclusao das bibliotecas do FreeRTOS */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#define BOTAO1 4
#define S1 0
#define S2 2
#define MOTOR 15

/* Mapeamento dos bits do grupo de eventos */
#define BIT_START (1 << 0)
#define BIT_S1    (1 << 1)
#define BIT_S2    (1 << 2)
#define BIT_MOTOR (1 << 3)
#define BIT_FIM   (1 << 4)

TaskHandle_t xTaskBotaoHandle;
TaskHandle_t xTaskControleHandle;
EventGroupHandle_t xEventos;

volatile bool cicloEmExecucao = false;

void vTaskBotao(void *pvParameters);
void vTaskControle(void *pvParameters);
void vTaskS1(void *pvParameters);
void vTaskS2(void *pvParameters);
void vTaskMotor(void *pvParameters);

void setup() {
  Serial.begin(115200);

  pinMode(BOTAO1, INPUT_PULLUP);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(MOTOR, OUTPUT);

  digitalWrite(S1, LOW);
  digitalWrite(S2, LOW);
  digitalWrite(MOTOR, LOW);

  xEventos = xEventGroupCreate();

  if (xEventos == NULL) {
    Serial.println("Falha ao criar o Event Group.");
    while (true) {
      delay(1000);
    }
  }

  xTaskCreate(vTaskBotao, "TASK_BOTAO", configMINIMAL_STACK_SIZE + 1024, NULL, 3, &xTaskBotaoHandle);
  xTaskCreate(vTaskControle, "TASK_CONTROLE", configMINIMAL_STACK_SIZE + 2048, NULL, 2, &xTaskControleHandle);
  xTaskCreate(vTaskS1, "TASK_S1", configMINIMAL_STACK_SIZE + 1024, NULL, 1, NULL);
  xTaskCreate(vTaskS2, "TASK_S2", configMINIMAL_STACK_SIZE + 1024, NULL, 1, NULL);
  xTaskCreate(vTaskMotor, "TASK_MOTOR", configMINIMAL_STACK_SIZE + 1024, NULL, 1, NULL);

  Serial.println("Sistema pronto. Pressione o botao Start para iniciar o ciclo.");
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}

void vTaskBotao(void *pvParameters) {
  bool estadoAnterior = HIGH;

  while (true) {
    bool estadoAtual = digitalRead(BOTAO1);

    if (estadoAnterior == HIGH && estadoAtual == LOW) {
      vTaskDelay(pdMS_TO_TICKS(30));

      if (digitalRead(BOTAO1) == LOW) {
        Serial.println("Botao pressionado.");

        if (!cicloEmExecucao) {
          Serial.println("Solicitando inicio do ciclo.");
          xTaskNotifyGive(xTaskControleHandle);
        } else {
          Serial.println("Botao ignorado: ciclo em execucao.");
        }
      }
    }

    while (digitalRead(BOTAO1) == LOW) {
      vTaskDelay(pdMS_TO_TICKS(10));
    }

    estadoAnterior = digitalRead(BOTAO1);
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

void vTaskControle(void *pvParameters) {
  while (true) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    if (cicloEmExecucao) {
      continue;
    }

    cicloEmExecucao = true;
    xEventGroupClearBits(xEventos, BIT_START | BIT_S1 | BIT_S2 | BIT_MOTOR | BIT_FIM);

    Serial.println("Ciclo iniciado.");
    xEventGroupSetBits(xEventos, BIT_START);

    vTaskDelay(pdMS_TO_TICKS(5000));
    Serial.println("Liberando S1.");
    xEventGroupSetBits(xEventos, BIT_S1);

    vTaskDelay(pdMS_TO_TICKS(4000));
    Serial.println("Liberando S2.");
    xEventGroupSetBits(xEventos, BIT_S2);

    vTaskDelay(pdMS_TO_TICKS(6000));
    Serial.println("Liberando motor.");
    xEventGroupSetBits(xEventos, BIT_MOTOR);

    xEventGroupWaitBits(xEventos, BIT_FIM, pdTRUE, pdTRUE, portMAX_DELAY);
    cicloEmExecucao = false;
    Serial.println("Ciclo finalizado.");
  }
}

void vTaskS1(void *pvParameters) {
  while (true) {
    xEventGroupWaitBits(xEventos, BIT_S1, pdTRUE, pdTRUE, portMAX_DELAY);

    Serial.println("S1 ligada.");
    digitalWrite(S1, HIGH);
    vTaskDelay(pdMS_TO_TICKS(3000));
    digitalWrite(S1, LOW);
    Serial.println("S1 desligada.");
  }
}

void vTaskS2(void *pvParameters) {
  while (true) {
    xEventGroupWaitBits(xEventos, BIT_S2, pdTRUE, pdTRUE, portMAX_DELAY);

    Serial.println("S2 ligada.");
    digitalWrite(S2, HIGH);
    vTaskDelay(pdMS_TO_TICKS(5000));
    digitalWrite(S2, LOW);
    Serial.println("S2 desligada.");
  }
}

void vTaskMotor(void *pvParameters) {
  while (true) {
    xEventGroupWaitBits(xEventos, BIT_MOTOR, pdTRUE, pdTRUE, portMAX_DELAY);

    Serial.println("Motor ligado.");
    digitalWrite(MOTOR, HIGH);
    vTaskDelay(pdMS_TO_TICKS(10000));
    digitalWrite(MOTOR, LOW);
    Serial.println("Motor desligado.");

    xEventGroupSetBits(xEventos, BIT_FIM);
  }
}
