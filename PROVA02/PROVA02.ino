#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

static const uint8_t led = 4;
static const uint8_t menu = 0;
static const uint8_t incrementa = 2;
static const uint8_t decrementa = 15;
static const uint8_t rele = 13;
static const uint8_t ldr = 39;
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
static const uint16_t ldr_maximo = 4095;
static const uint8_t setpoint = 50;
static const uint8_t histerese = 5;
static const TickType_t debounce = pdMS_TO_TICKS(50);

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

struct DadosLuminosidade {
  uint16_t leituraADC;
  uint8_t luminosidadePercentual;
};

struct DadosDisplay {
  uint8_t valor;
  bool modoProgramacao;
};

QueueHandle_t filaControle;
QueueHandle_t filaDisplay;

volatile uint8_t valorAtualDisplay = 0;
volatile bool pontoDecimalAtivo = false;

void taskLeituraLdr(void *pvParameters);
void taskControleMenu(void *pvParameters);
void taskDisplay(void *pvParameters);

bool botaoPressionado(uint8_t pinoBotao);
void publicarDisplay(uint8_t valor, bool modoProgramacao);
void apagarDisplays();
void escreverDigito(uint8_t digito, bool pontoDecimal);
void exibirNumeroMux(uint8_t numero, bool pontoDecimal);

void setup() {
  analogReadResolution(12);

  pinMode(ldr, INPUT);
  pinMode(led, OUTPUT);
  pinMode(rele, OUTPUT);
  pinMode(menu, INPUT_PULLUP);
  pinMode(incrementa, INPUT_PULLUP);
  pinMode(decrementa, INPUT_PULLUP);

  digitalWrite(led, LOW);
  digitalWrite(rele, LOW);

  for (uint8_t i = 0; i < 8; i++) {
    pinMode(pinosSegmentos[i], OUTPUT);
    digitalWrite(pinosSegmentos[i], LOW);
  }

  pinMode(display_dezena, OUTPUT);
  pinMode(display_unidade, OUTPUT);
  apagarDisplays();

  filaControle = xQueueCreate(1, sizeof(DadosLuminosidade));
  filaDisplay = xQueueCreate(1, sizeof(DadosDisplay));

  if (filaControle == NULL || filaDisplay == NULL) {
    while (true) {
      delay(1000);
    }
  }

  publicarDisplay(setpoint, false);

  if (xTaskCreate(taskLeituraLdr, "LeituraLDR", 2048, NULL, 1, NULL) != pdPASS ||
      xTaskCreate(taskControleMenu, "ControleMenu", 4096, NULL, 2, NULL) != pdPASS ||
      xTaskCreate(taskDisplay, "Display", 2048, NULL, 2, NULL) != pdPASS) {
    while (true) {
      delay(1000);
    }
  }
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}

void taskLeituraLdr(void *pvParameters) {
  (void) pvParameters;

  while (true) {
    DadosLuminosidade amostra;
    amostra.leituraADC = analogRead(ldr);
    amostra.luminosidadePercentual = static_cast<uint8_t>(map(amostra.leituraADC, 0, ldr_maximo, 0, 100));

    xQueueOverwrite(filaControle, &amostra);
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void taskControleMenu(void *pvParameters) {
  (void) pvParameters;

  DadosLuminosidade amostraAtual = {0, 0};
  uint8_t setpoint = setpoint;
  bool modoProgramacao = false;
  bool releLigado = false;

  publicarDisplay(0, false);

  while (true) {
    DadosLuminosidade novaAmostra;
    if (xQueueReceive(filaControle, &novaAmostra, pdMS_TO_TICKS(20)) == pdTRUE) {
      amostraAtual = novaAmostra;
    }

    if (botaoPressionado(menu)) {
      modoProgramacao = !modoProgramacao;
      digitalWrite(led, modoProgramacao ? HIGH : LOW);
      publicarDisplay(modoProgramacao ? setpoint : amostraAtual.luminosidadePercentual, modoProgramacao);
    }

    if (modoProgramacao) {
      if (botaoPressionado(incrementa) && setpoint < 99) {
        setpoint++;
        publicarDisplay(setpoint, true);
      }

      if (botaoPressionado(decrementa) && setpoint > 0) {
        setpoint--;
        publicarDisplay(setpoint, true);
      }
    } else {
      if (!releLigado && amostraAtual.luminosidadePercentual <= max(0, static_cast<int>(setpoint) - histerese)) {
        releLigado = true;
        digitalWrite(rele, HIGH);
      } else if (releLigado && amostraAtual.luminosidadePercentual >= min(100, static_cast<int>(setpoint) + histerese)) {
        releLigado = false;
        digitalWrite(rele, LOW);
      }

      publicarDisplay(amostraAtual.luminosidadePercentual, false);
    }

    vTaskDelay(pdMS_TO_TICKS(120));
  }
}

void taskDisplay(void *pvParameters) {
  (void) pvParameters;
  DadosDisplay dadosRecebidos;

  while (true) {
    if (xQueueReceive(filaDisplay, &dadosRecebidos, 0) == pdTRUE) {
      valorAtualDisplay = dadosRecebidos.valor;
      pontoDecimalAtivo = dadosRecebidos.modoProgramacao;
    }

    exibirNumeroMux(valorAtualDisplay, pontoDecimalAtivo);
  }
}

bool botaoPressionado(uint8_t pinoBotao) {
  if (digitalRead(pinoBotao) == LOW) {
    vTaskDelay(debounce);

    if (digitalRead(pinoBotao) == LOW) {
      while (digitalRead(pinoBotao) == LOW) {
        vTaskDelay(pdMS_TO_TICKS(10));
      }

      return true;
    }
  }

  return false;
}

void publicarDisplay(uint8_t valor, bool modoProgramacao) {
  DadosDisplay dados = {static_cast<uint8_t>(constrain(valor, 0, 99)), modoProgramacao};
  xQueueOverwrite(filaDisplay, &dados);
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

void exibirNumeroMux(uint8_t numero, bool pontoDecimal) {
  uint8_t valor = constrain(numero, 0, 99);
  uint8_t dezena = valor / 10;
  uint8_t unidade = valor % 10;

  apagarDisplays();
  escreverDigito(dezena, false);
  digitalWrite(display_dezena, HIGH);
  vTaskDelay(pdMS_TO_TICKS(2));

  apagarDisplays();
  escreverDigito(unidade, pontoDecimal);
  digitalWrite(display_unidade, HIGH);
  vTaskDelay(pdMS_TO_TICKS(2));
}
