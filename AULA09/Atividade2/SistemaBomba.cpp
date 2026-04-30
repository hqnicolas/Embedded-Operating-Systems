#include "SistemaBomba.h"

static const uint8_t SEGMENTOS_POR_DIGITO = 8;
static const uint16_t CONTAGEM_INICIAL = 59;
static const uint16_t INTERVALO_NORMAL_MS = 1000;
static const uint16_t INTERVALO_RAPIDO_MS = 500;
static const uint16_t TEMPO_STATUS_MS = 3000;
static const uint16_t LIMIAR_OBSTRUCAO = 2200;
static const uint8_t TAMANHO_FILA_EVENTOS = 10;
static const uint8_t RGB_ATIVO = LOW;
static const uint8_t RGB_DESLIGADO = HIGH;

static const byte numeros[10][8] = {
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

static const uint8_t pinosSegmentos[SEGMENTOS_POR_DIGITO] = {
  SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_DP
};

static QueueHandle_t filaEventos = NULL;
static SemaphoreHandle_t semBotaoCancela = NULL;
static SemaphoreHandle_t semBotaoAcelera = NULL;
static SemaphoreHandle_t semBotaoExplode = NULL;
static SemaphoreHandle_t mutexEstado = NULL;

static TimerHandle_t timerContagem = NULL;
static TimerHandle_t timerStatus = NULL;

static volatile EstadoSistema estadoAtual = ESTADO_AGUARDANDO;
static volatile uint8_t valorDisplay = CONTAGEM_INICIAL;
static volatile uint16_t intervaloAtualMs = INTERVALO_NORMAL_MS;

static void taskMonitorLdr(void *pvParameters);
static void taskBotoes(void *pvParameters);
static void taskControle(void *pvParameters);
static void taskDisplay(void *pvParameters);

static void callbackTimerContagem(TimerHandle_t xTimer);
static void callbackTimerStatus(TimerHandle_t xTimer);

static void IRAM_ATTR isrBotaoCancela();
static void IRAM_ATTR isrBotaoAcelera();
static void IRAM_ATTR isrBotaoExplode();

static void configurarPinos();
static void apagarDisplays();
static void escreverDigito(uint8_t digito);
static void exibirNumeroMux(uint8_t numero);
static void setDisplaySeguro(uint8_t valor);
static uint8_t getDisplaySeguro();
static EstadoSistema getEstadoSeguro();
static void setEstadoSeguro(EstadoSistema novoEstado);
static void ligarLedExplosao();
static void ligarLedSalvo();
static void desligarStatus();
static void iniciarContagem();
static void cancelarContagem();
static void explodirBomba();
static void enviarEvento(EventoSistema evento);

void inicializarSistemaBomba() {
  configurarPinos();
  analogReadResolution(12);

  filaEventos = xQueueCreate(TAMANHO_FILA_EVENTOS, sizeof(EventoSistema));
  semBotaoCancela = xSemaphoreCreateBinary();
  semBotaoAcelera = xSemaphoreCreateBinary();
  semBotaoExplode = xSemaphoreCreateBinary();
  mutexEstado = xSemaphoreCreateMutex();

  timerContagem = xTimerCreate(
    "TimerContagem",
    pdMS_TO_TICKS(INTERVALO_NORMAL_MS),
    pdTRUE,
    (void *)0,
    callbackTimerContagem
  );

  timerStatus = xTimerCreate(
    "TimerStatus",
    pdMS_TO_TICKS(TEMPO_STATUS_MS),
    pdFALSE,
    (void *)0,
    callbackTimerStatus
  );

  if (filaEventos == NULL || semBotaoCancela == NULL || semBotaoAcelera == NULL ||
      semBotaoExplode == NULL || mutexEstado == NULL || timerContagem == NULL ||
      timerStatus == NULL) {
    while (true) {
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }

  attachInterrupt(digitalPinToInterrupt(BOTAO1), isrBotaoCancela, FALLING);
  attachInterrupt(digitalPinToInterrupt(BOTAO2), isrBotaoAcelera, FALLING);
  attachInterrupt(digitalPinToInterrupt(BOTAO3), isrBotaoExplode, FALLING);

  if (xTaskCreate(taskMonitorLdr, "TaskLDR", 3072, NULL, 1, NULL) != pdPASS ||
      xTaskCreate(taskBotoes, "TaskBotoes", 3072, NULL, 2, NULL) != pdPASS ||
      xTaskCreate(taskControle, "TaskControle", 4096, NULL, 3, NULL) != pdPASS ||
      xTaskCreate(taskDisplay, "TaskDisplay", 2048, NULL, 2, NULL) != pdPASS) {
    while (true) {
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }
}

void loopSistemaBomba() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}

static void configurarPinos() {
  pinMode(BOTAO1, INPUT_PULLUP);
  pinMode(BOTAO2, INPUT_PULLUP);
  pinMode(BOTAO3, INPUT_PULLUP);
  pinMode(LDR, INPUT);

  pinMode(RGB_RED, OUTPUT);
  pinMode(RGB_BLUE, OUTPUT);

  // LED RGB com anodo comum: acende em LOW e apaga em HIGH.
  digitalWrite(RGB_RED, RGB_DESLIGADO);
  digitalWrite(RGB_BLUE, RGB_DESLIGADO);

  for (uint8_t i = 0; i < SEGMENTOS_POR_DIGITO; i++) {
    pinMode(pinosSegmentos[i], OUTPUT);
    digitalWrite(pinosSegmentos[i], LOW);
  }

  pinMode(DISPLAY_DEZENA, OUTPUT);
  pinMode(DISPLAY_UNIDADE, OUTPUT);
  apagarDisplays();
}

static void taskMonitorLdr(void *pvParameters) {
  (void) pvParameters;
  bool obstrucaoJaDetectada = false;

  while (true) {
    uint16_t leituraLdr = analogRead(LDR);

    if (leituraLdr > LIMIAR_OBSTRUCAO && !obstrucaoJaDetectada) {
      enviarEvento(EVENTO_LDR_ESCURO);
      obstrucaoJaDetectada = true;
    }

    if (leituraLdr <= LIMIAR_OBSTRUCAO) {
      obstrucaoJaDetectada = false;
    }

    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

static void taskBotoes(void *pvParameters) {
  (void) pvParameters;

  while (true) {
    if (xSemaphoreTake(semBotaoCancela, pdMS_TO_TICKS(20)) == pdTRUE) {
      enviarEvento(EVENTO_BOTAO_CANCELA);
    }

    if (xSemaphoreTake(semBotaoAcelera, 0) == pdTRUE) {
      enviarEvento(EVENTO_BOTAO_ACELERA);
    }

    if (xSemaphoreTake(semBotaoExplode, 0) == pdTRUE) {
      enviarEvento(EVENTO_BOTAO_EXPLODE);
    }
  }
}

static void taskControle(void *pvParameters) {
  (void) pvParameters;
  EventoSistema eventoRecebido;
  uint8_t segundosRestantes = CONTAGEM_INICIAL;

  while (true) {
    if (xQueueReceive(filaEventos, &eventoRecebido, portMAX_DELAY) == pdTRUE) {
      switch (eventoRecebido) {
        case EVENTO_LDR_ESCURO:
          if (getEstadoSeguro() == ESTADO_AGUARDANDO) {
            segundosRestantes = CONTAGEM_INICIAL;
            setDisplaySeguro(segundosRestantes);
            iniciarContagem();
          }
          break;

        case EVENTO_BOTAO_CANCELA:
          if (getEstadoSeguro() == ESTADO_CONTAGEM) {
            cancelarContagem();
            segundosRestantes = CONTAGEM_INICIAL;
            setDisplaySeguro(segundosRestantes);
          }
          break;

        case EVENTO_BOTAO_ACELERA:
          if (getEstadoSeguro() == ESTADO_CONTAGEM && intervaloAtualMs != INTERVALO_RAPIDO_MS) {
            intervaloAtualMs = INTERVALO_RAPIDO_MS;
            xTimerChangePeriod(timerContagem, pdMS_TO_TICKS(intervaloAtualMs), 0);
          }
          break;

        case EVENTO_BOTAO_EXPLODE:
          if (getEstadoSeguro() == ESTADO_CONTAGEM) {
            explodirBomba();
          }
          break;

        case EVENTO_TIMER_TICK:
          if (getEstadoSeguro() == ESTADO_CONTAGEM) {
            if (segundosRestantes > 0) {
              segundosRestantes--;
              setDisplaySeguro(segundosRestantes);
            }

            if (segundosRestantes == 0) {
              explodirBomba();
              segundosRestantes = CONTAGEM_INICIAL;
            }
          }
          break;

        case EVENTO_LIMPA_STATUS:
          desligarStatus();

          if (getEstadoSeguro() != ESTADO_CONTAGEM) {
            setEstadoSeguro(ESTADO_AGUARDANDO);
            setDisplaySeguro(CONTAGEM_INICIAL);
          }
          break;
      }
    }
  }
}

static void taskDisplay(void *pvParameters) {
  (void) pvParameters;

  while (true) {
    exibirNumeroMux(getDisplaySeguro());
  }
}

static void callbackTimerContagem(TimerHandle_t xTimer) {
  (void) xTimer;
  enviarEvento(EVENTO_TIMER_TICK);
}

static void callbackTimerStatus(TimerHandle_t xTimer) {
  (void) xTimer;
  enviarEvento(EVENTO_LIMPA_STATUS);
}

static void IRAM_ATTR isrBotaoCancela() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR(semBotaoCancela, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void IRAM_ATTR isrBotaoAcelera() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR(semBotaoAcelera, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void IRAM_ATTR isrBotaoExplode() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR(semBotaoExplode, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void apagarDisplays() {
  digitalWrite(DISPLAY_DEZENA, LOW);
  digitalWrite(DISPLAY_UNIDADE, LOW);
}

static void escreverDigito(uint8_t digito) {
  for (uint8_t i = 0; i < SEGMENTOS_POR_DIGITO; i++) {
    digitalWrite(pinosSegmentos[i], numeros[digito][i]);
  }
}

static void exibirNumeroMux(uint8_t numero) {
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

static void setDisplaySeguro(uint8_t valor) {
  xSemaphoreTake(mutexEstado, portMAX_DELAY);
  valorDisplay = valor;
  xSemaphoreGive(mutexEstado);
}

static uint8_t getDisplaySeguro() {
  uint8_t valor;
  xSemaphoreTake(mutexEstado, portMAX_DELAY);
  valor = valorDisplay;
  xSemaphoreGive(mutexEstado);
  return valor;
}

static EstadoSistema getEstadoSeguro() {
  EstadoSistema estado;
  xSemaphoreTake(mutexEstado, portMAX_DELAY);
  estado = estadoAtual;
  xSemaphoreGive(mutexEstado);
  return estado;
}

static void setEstadoSeguro(EstadoSistema novoEstado) {
  xSemaphoreTake(mutexEstado, portMAX_DELAY);
  estadoAtual = novoEstado;
  xSemaphoreGive(mutexEstado);
}

static void ligarLedExplosao() {
  digitalWrite(RGB_BLUE, RGB_DESLIGADO);
  digitalWrite(RGB_RED, RGB_ATIVO);
}

static void ligarLedSalvo() {
  digitalWrite(RGB_RED, RGB_DESLIGADO);
  digitalWrite(RGB_BLUE, RGB_ATIVO);
}

static void desligarStatus() {
  digitalWrite(RGB_RED, RGB_DESLIGADO);
  digitalWrite(RGB_BLUE, RGB_DESLIGADO);
}

static void iniciarContagem() {
  intervaloAtualMs = INTERVALO_NORMAL_MS;
  xTimerChangePeriod(timerContagem, pdMS_TO_TICKS(intervaloAtualMs), 0);
  xTimerStart(timerContagem, 0);
  setEstadoSeguro(ESTADO_CONTAGEM);
  desligarStatus();
}

static void cancelarContagem() {
  xTimerStop(timerContagem, 0);
  setEstadoSeguro(ESTADO_CANCELADO);
  ligarLedSalvo();
  xTimerReset(timerStatus, 0);
}

static void explodirBomba() {
  xTimerStop(timerContagem, 0);
  setEstadoSeguro(ESTADO_EXPLODIU);
  ligarLedExplosao();
  xTimerReset(timerStatus, 0);
}

static void enviarEvento(EventoSistema evento) {
  if (filaEventos != NULL) {
    xQueueSend(filaEventos, &evento, 0);
  }
}
