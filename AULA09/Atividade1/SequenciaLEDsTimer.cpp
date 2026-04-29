#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

#define LED1 4
#define LED2 0
#define LED3 2
#define LED4 15

TimerHandle_t xTimerSequencia;
volatile int passoAtual = 0;

void apagaTodos();
void callbackSequencia(TimerHandle_t xTimer);

void setup() {
  Serial.begin(115200);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);

  apagaTodos();

  xTimerSequencia = xTimerCreate(
    "TimerSeq",
    pdMS_TO_TICKS(250),
    pdTRUE,
    (void *)0,
    callbackSequencia
  );

  if (xTimerSequencia == NULL) {
    Serial.println("Erro ao criar o software timer.");
    while (1) {
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }

  xTimerStart(xTimerSequencia, 0);
  Serial.println("Sequencia de LEDs iniciada.");
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}

void apagaTodos() {
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);
}

void callbackSequencia(TimerHandle_t xTimer) {
  switch (passoAtual) {
    case 0:
      digitalWrite(LED1, HIGH);
      Serial.println("LED1 ligado");
      break;

    case 1:
      digitalWrite(LED2, HIGH);
      Serial.println("LED2 ligado");
      break;

    case 2:
      digitalWrite(LED3, HIGH);
      Serial.println("LED3 ligado");
      break;

    case 3:
      digitalWrite(LED4, HIGH);
      Serial.println("LED4 ligado");
      break;

    default:
      apagaTodos();
      Serial.println("Todos os LEDs apagados");
      passoAtual = -1;
      break;
  }

  passoAtual++;
}
