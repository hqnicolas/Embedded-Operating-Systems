#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

#define BOTAO1 4
#define LED2 0
#define LED3 2
#define LED4 15

TimerHandle_t xTimerLed2;
TimerHandle_t xTimerLed3;
TimerHandle_t xTimerLed4;

bool timersAtivos = false;
bool ultimoEstadoBotao = HIGH;

void callbackLed2(TimerHandle_t xTimer);
void callbackLed3(TimerHandle_t xTimer);
void callbackLed4(TimerHandle_t xTimer);
void alternaTimersPeriodicos();
void trataBotao();

void setup() {
  Serial.begin(115200);

  pinMode(BOTAO1, INPUT_PULLUP);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);

  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);

  xTimerLed2 = xTimerCreate(
    "TimerLED2",
    pdMS_TO_TICKS(3000),
    pdFALSE,
    (void *)0,
    callbackLed2
  );

  xTimerLed3 = xTimerCreate(
    "TimerLED3",
    pdMS_TO_TICKS(500),
    pdTRUE,
    (void *)0,
    callbackLed3
  );

  xTimerLed4 = xTimerCreate(
    "TimerLED4",
    pdMS_TO_TICKS(1000),
    pdTRUE,
    (void *)0,
    callbackLed4
  );

  if (xTimerLed2 == NULL || xTimerLed3 == NULL || xTimerLed4 == NULL) {
    Serial.println("Erro ao criar os software timers.");
    while (1) {
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }

  Serial.println("Pressione BOTAO1 para iniciar/parar os timers periodicos.");
  Serial.println("A cada clique, o LED2 acende por 3 segundos usando timer one-shot.");
}

void loop() {
  trataBotao();
  vTaskDelay(pdMS_TO_TICKS(20));
}

void trataBotao() {
  bool estadoAtual = digitalRead(BOTAO1);

  if (ultimoEstadoBotao == HIGH && estadoAtual == LOW) {
    vTaskDelay(pdMS_TO_TICKS(50));

    if (digitalRead(BOTAO1) == LOW) {
      digitalWrite(LED2, HIGH);
      xTimerReset(xTimerLed2, 0);
      alternaTimersPeriodicos();

      while (digitalRead(BOTAO1) == LOW) {
        vTaskDelay(pdMS_TO_TICKS(10));
      }
    }
  }

  ultimoEstadoBotao = estadoAtual;
}

void alternaTimersPeriodicos() {
  if (timersAtivos) {
    xTimerStop(xTimerLed3, 0);
    xTimerStop(xTimerLed4, 0);

    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);

    timersAtivos = false;
    Serial.println("Timers auto reload parados.");
  } else {
    xTimerStart(xTimerLed3, 0);
    xTimerStart(xTimerLed4, 0);

    timersAtivos = true;
    Serial.println("Timers auto reload iniciados.");
  }
}

void callbackLed2(TimerHandle_t xTimer) {
  digitalWrite(LED2, LOW);
  Serial.println("Timer one-shot do LED2 finalizado.");
}

void callbackLed3(TimerHandle_t xTimer) {
  digitalWrite(LED3, !digitalRead(LED3));
}

void callbackLed4(TimerHandle_t xTimer) {
  digitalWrite(LED4, !digitalRead(LED4));
}
