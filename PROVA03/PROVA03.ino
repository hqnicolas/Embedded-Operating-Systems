/*
   ###########################
   # GABRIEL BADA
   # NICOLAS BORBA PEREIRA
   ###########################
*/

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"

static const uint8_t led = 4;
static const uint8_t valvula = 0;
static const uint8_t valvulaAcionada = LOW;
static const uint8_t valvulaDesligada = HIGH;
static const uint8_t emergencia = 2;
static const uint8_t ldr = 39;
static const uint8_t pot = 34;

static const uint8_t a = 18;
static const uint8_t b = 5;
static const uint8_t c = 21;
static const uint8_t d = 3;
static const uint8_t e = 1;
static const uint8_t f = 23;
static const uint8_t g = 22;
static const uint8_t ponto = 19;
static const uint8_t display_dezena = 16;
static const uint8_t display_unidade = 17;

static const uint16_t minimoCalibrado = 820;
static const uint16_t maximoCalibrado = 2335;
static const uint8_t limitePressao = 50;
static const uint8_t limiteLdrAcionado = 50;
static const TickType_t intervaloLeitura = pdMS_TO_TICKS(100);
static const TickType_t intervaloBotaoPressionado = pdMS_TO_TICKS(20);
static const TickType_t intervaloVerificacaoAlarme = pdMS_TO_TICKS(20);
static const uint8_t amostrasDebounceEmergencia = 4;

#define ALARME_PRESSAO     (1 << 0)
#define ALARME_LDR         (1 << 1)
#define ALARME_EMERGENCIA  (1 << 2)
#define TODOS_ALARMES      (ALARME_PRESSAO | ALARME_LDR | ALARME_EMERGENCIA)

const byte digitos7seg[10][8] = {
  {1, 1, 1, 1, 1, 1, 0, 0},
  {0, 1, 1, 0, 0, 0, 0, 0},
  {1, 1, 0, 1, 1, 0, 1, 0},
  {1, 1, 1, 1, 0, 0, 1, 0},
  {0, 1, 1, 0, 0, 1, 1, 0},
  {1, 0, 1, 1, 0, 1, 1, 0},
  {1, 0, 1, 1, 1, 1, 1, 0},
  {1, 1, 1, 0, 0, 0, 0, 0},
  {1, 1, 1, 1, 1, 1, 1, 0},
  {1, 1, 1, 1, 0, 1, 1, 0}
};

const uint8_t pinosSegmentos[8] = {
  a, b, c, d, e, f, g, ponto
};

struct DadosDisplay {
  uint8_t valor;
};

QueueHandle_t filaDisplay = NULL;
TimerHandle_t timerLED = NULL;
EventGroupHandle_t grupoAlarmes = NULL;
TaskHandle_t taskAlarmeHandle = NULL;
TaskHandle_t taskBotaoEmergenciaHandle = NULL;

volatile uint8_t valorDisplayAtual = 0;

void TaskPressao(void *pvParameters);
void TaskLDR(void *pvParameters);
void TaskBotaoEmergencia(void *pvParameters);
void TaskAlarme(void *pvParameters);
void TaskDisplay(void *pvParameters);
void callbackTimerLED(TimerHandle_t xTimer);
void IRAM_ATTR interrupcaoEmergencia();

uint8_t converterPressao(uint16_t leituraDaPorta);
uint8_t converterLuminosidade(uint16_t leituraDaPorta);
void atualizarDisplay(uint8_t valor);
void notificarTaskAlarme();
void apagarDisplays();
void escreverDigito(uint8_t digito, bool pontoDecimal);
void exibirNumero(uint8_t numero);
TickType_t periodoDoAlarme(EventBits_t bits);
void aplicarSaidasDeAlarme(EventBits_t bits);
void aplicarValvula(bool acionada);

void setup() {
  pinMode(ldr, INPUT);
  pinMode(pot, INPUT);
  pinMode(led, OUTPUT);

  pinMode(valvula, INPUT_PULLUP);
  digitalWrite(valvula, valvulaDesligada);
  pinMode(valvula, OUTPUT);

  pinMode(emergencia, INPUT_PULLUP);

  digitalWrite(led, LOW);
  aplicarValvula(false);

  for (uint8_t i = 0; i < 8; i++) {
    pinMode(pinosSegmentos[i], OUTPUT);
    digitalWrite(pinosSegmentos[i], LOW);
  }

  pinMode(display_dezena, OUTPUT);
  pinMode(display_unidade, OUTPUT);
  apagarDisplays();

  filaDisplay = xQueueCreate(1, sizeof(DadosDisplay));
  grupoAlarmes = xEventGroupCreate();
  timerLED = xTimerCreate("TimerLED", pdMS_TO_TICKS(250), pdTRUE, NULL, callbackTimerLED);

  if (filaDisplay == NULL || grupoAlarmes == NULL || timerLED == NULL) {
    while (true) {
      delay(1000);
    }
  }

  atualizarDisplay(0);

  if (xTaskCreate(TaskAlarme, "TaskAlarme", 2048, NULL, 3, &taskAlarmeHandle) != pdPASS ||
      xTaskCreate(TaskDisplay, "TaskDisplay", 2048, NULL, 2, NULL) != pdPASS ||
      xTaskCreate(TaskPressao, "TaskPressao", 2048, NULL, 2, NULL) != pdPASS ||
      xTaskCreate(TaskLDR, "TaskLDR", 2048, NULL, 2, NULL) != pdPASS ||
      xTaskCreate(TaskBotaoEmergencia, "TaskEmergencia", 2048, NULL, 3, &taskBotaoEmergenciaHandle) != pdPASS) {
    while (true) {
      delay(1000);
    }
  }

  attachInterrupt(digitalPinToInterrupt(emergencia), interrupcaoEmergencia, FALLING);
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}

void TaskPressao(void *pvParameters) {
  (void) pvParameters;
  bool alarmePressaoAtivo = false;

  while (true) {
    uint8_t pressao = converterPressao(analogRead(pot));
    bool novoEstado = (pressao > limitePressao);

    atualizarDisplay(pressao);

    if (novoEstado != alarmePressaoAtivo) {
      if (novoEstado) {
        xEventGroupSetBits(grupoAlarmes, ALARME_PRESSAO);
      } else {
        xEventGroupClearBits(grupoAlarmes, ALARME_PRESSAO);
      }

      alarmePressaoAtivo = novoEstado;
      notificarTaskAlarme();
    }

    vTaskDelay(intervaloLeitura);
  }
}

void TaskLDR(void *pvParameters) {
  (void) pvParameters;
  bool alarmeLdrAtivo = false;

  while (true) {
    uint8_t luminosidade = converterLuminosidade(analogRead(ldr));
    bool novoEstado = (luminosidade >= limiteLdrAcionado);

    if (novoEstado != alarmeLdrAtivo) {
      if (novoEstado) {
        xEventGroupSetBits(grupoAlarmes, ALARME_LDR);
      } else {
        xEventGroupClearBits(grupoAlarmes, ALARME_LDR);
      }

      alarmeLdrAtivo = novoEstado;
      notificarTaskAlarme();
    }

    vTaskDelay(intervaloLeitura);
  }
}

void TaskBotaoEmergencia(void *pvParameters) {
  (void) pvParameters;
  bool alarmeEmergenciaAtivo = false;
  bool leituraAnterior = (digitalRead(emergencia) == LOW);
  uint8_t amostrasIguais = 0;

  while (true) {
    ulTaskNotifyTake(pdTRUE, intervaloBotaoPressionado);

    bool botaoPressionado = (digitalRead(emergencia) == LOW);

    if (botaoPressionado == leituraAnterior) {
      if (amostrasIguais < amostrasDebounceEmergencia) {
        amostrasIguais++;
      }
    } else {
      leituraAnterior = botaoPressionado;
      amostrasIguais = 1;
    }

    if (amostrasIguais >= amostrasDebounceEmergencia && botaoPressionado != alarmeEmergenciaAtivo) {
      if (botaoPressionado) {
        xEventGroupSetBits(grupoAlarmes, ALARME_EMERGENCIA);
      } else {
        xEventGroupClearBits(grupoAlarmes, ALARME_EMERGENCIA);
      }

      alarmeEmergenciaAtivo = botaoPressionado;
      notificarTaskAlarme();
    }
  }
}

void TaskAlarme(void *pvParameters) {
  (void) pvParameters;
  EventBits_t ultimoEstadoAplicado = TODOS_ALARMES;

  while (true) {
    ulTaskNotifyTake(pdTRUE, intervaloVerificacaoAlarme);

    EventBits_t bitsAtivos = xEventGroupGetBits(grupoAlarmes) & TODOS_ALARMES;

    if (bitsAtivos != ultimoEstadoAplicado) {
      aplicarSaidasDeAlarme(bitsAtivos);
      ultimoEstadoAplicado = bitsAtivos;
    }
  }
}

void TaskDisplay(void *pvParameters) {
  (void) pvParameters;
  DadosDisplay dadosRecebidos;

  while (true) {
    if (xQueueReceive(filaDisplay, &dadosRecebidos, 0) == pdTRUE) {
      valorDisplayAtual = dadosRecebidos.valor;
    }

    exibirNumero(valorDisplayAtual);
  }
}

void callbackTimerLED(TimerHandle_t xTimer) {
  (void) xTimer;
  digitalWrite(led, !digitalRead(led));
}

void IRAM_ATTR interrupcaoEmergencia() {
  BaseType_t tarefaAcordada = pdFALSE;

  if (taskBotaoEmergenciaHandle != NULL) {
    vTaskNotifyGiveFromISR(taskBotaoEmergenciaHandle, &tarefaAcordada);

    if (tarefaAcordada == pdTRUE) {
      portYIELD_FROM_ISR();
    }
  }
}

uint8_t converterPressao(uint16_t leituraDaPorta) {
  uint16_t leituraCalibrada = constrain(leituraDaPorta, 0, 4095);
  return static_cast<uint8_t>(map(leituraCalibrada, 0, 4095, 0, 99));
}

uint8_t converterLuminosidade(uint16_t leituraDaPorta) {
  uint16_t leituraCalibrada = constrain(leituraDaPorta, minimoCalibrado, maximoCalibrado);
  return static_cast<uint8_t>(map(leituraCalibrada, minimoCalibrado, maximoCalibrado, 0, 99));
}

void atualizarDisplay(uint8_t valor) {
  DadosDisplay dados = {static_cast<uint8_t>(constrain(valor, 0, 99))};
  xQueueOverwrite(filaDisplay, &dados);
}

void notificarTaskAlarme() {
  if (taskAlarmeHandle != NULL) {
    xTaskNotifyGive(taskAlarmeHandle);
  }
}

void aplicarSaidasDeAlarme(EventBits_t bits) {
  if ((bits & TODOS_ALARMES) == 0) {
    xTimerStop(timerLED, 0);
    digitalWrite(led, LOW);
    aplicarValvula(false);
    return;
  }

  aplicarValvula(true);
  digitalWrite(led, LOW);
  xTimerChangePeriod(timerLED, periodoDoAlarme(bits), 0);
  xTimerStart(timerLED, 0);
}

void aplicarValvula(bool acionada) {
  digitalWrite(valvula, acionada ? valvulaAcionada : valvulaDesligada);
}

void apagarDisplays() {
  digitalWrite(display_dezena, LOW);
  digitalWrite(display_unidade, LOW);
}

void escreverDigito(uint8_t digito, bool pontoDecimal) {
  for (uint8_t i = 0; i < 8; i++) {
    digitalWrite(pinosSegmentos[i], digitos7seg[digito][i]);
  }

  digitalWrite(ponto, pontoDecimal ? HIGH : LOW);
}

void exibirNumero(uint8_t numero) {
  uint8_t valor = constrain(numero, 0, 99);
  uint8_t dezena = valor / 10;
  uint8_t unidade = valor % 10;

  apagarDisplays();
  escreverDigito(dezena, false);
  digitalWrite(display_dezena, HIGH);
  vTaskDelay(pdMS_TO_TICKS(2));

  apagarDisplays();
  escreverDigito(unidade, false);
  digitalWrite(display_unidade, HIGH);
  vTaskDelay(pdMS_TO_TICKS(2));
}

TickType_t periodoDoAlarme(EventBits_t bits) {
  if ((bits & ALARME_EMERGENCIA) != 0) {
    return pdMS_TO_TICKS(1000);
  }

  if ((bits & ALARME_LDR) != 0) {
    return pdMS_TO_TICKS(500);
  }

  return pdMS_TO_TICKS(250);
}
