#ifndef SISTEMA_FOGOS_H
#define SISTEMA_FOGOS_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#define BOTAO1 4
#define BOTAO2 0
#define BOTAO3 2
#define RGB_RED 25
#define RGB_BLUE 27
#define LDR 39

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

enum EstadoSistema {
  ESTADO_AGUARDANDO = 0,
  ESTADO_CONTAGEM,
  ESTADO_CANCELADO,
  ESTADO_EXPLODIU
};

enum EventoSistema {
  EVENTO_LDR_ESCURO = 0,
  EVENTO_BOTAO_CANCELA,
  EVENTO_BOTAO_ACELERA,
  EVENTO_BOTAO_EXPLODE,
  EVENTO_TIMER_TICK,
  EVENTO_LIMPA_STATUS
};

void inicializarSistemaFogos();
void loopSistemaFogos();

#endif
