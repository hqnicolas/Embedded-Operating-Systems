#include "sensor.h"
#include "led.h"

const int pinoA = 18;
const int pinoB = 19;
const int pinoC = 0;
const int pinoD = 4;
const int pinoE = 16;
const int pinoF = 5;
const int pinoG = 17;

const int SENSOR_INTERNO_PIN = 14;
const int SENSOR_EXTERNO_PIN = 12;
const int LED_ABRE_PORTAO_PIN = 26;
const int LED_FECHA_PORTAO_PIN = 27;

const byte numeros[10][7] = {
  {1, 1, 1, 1, 1, 1, 0},
  {0, 1, 1, 0, 0, 0, 0},
  {1, 1, 0, 1, 1, 0, 1},
  {1, 1, 1, 1, 0, 0, 1},
  {0, 1, 1, 0, 0, 1, 1},
  {1, 0, 1, 1, 0, 1, 1},
  {1, 0, 1, 1, 1, 1, 1},
  {1, 1, 1, 0, 0, 0, 0},
  {1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 0, 1, 1}
};

Sensor sensorInterno(SENSOR_INTERNO_PIN);
Sensor sensorExterno(SENSOR_EXTERNO_PIN);
LED ledAbrirPortao(LED_ABRE_PORTAO_PIN);
LED ledFecharPortao(LED_FECHA_PORTAO_PIN);

const int TOTAL_VAGAS = 5;
int TemVaga = TOTAL_VAGAS;

enum EstadoPortao {
  FECHADO,
  ABERTO_ENTRADA,
  ABERTO_SAIDA
};

EstadoPortao estadoAtualPortao = FECHADO;

enum EstadoSistema {
  AGUARDANDO,
  ABRINDO_ENTRADA,
  CARRO_ENTRANDO,
  FECHANDO_ENTRADA,
  ABRINDO_SAIDA,
  CARRO_SAINDO,
  FECHANDO_SAIDA
};

EstadoSistema estadoAtualSistema = AGUARDANDO;

void atualizarDisplay(int n) {
  if (n < 0) n = 0;
  if (n > 9) n = 9;

  digitalWrite(pinoA, numeros[n][0]);
  digitalWrite(pinoB, numeros[n][1]);
  digitalWrite(pinoC, numeros[n][2]);
  digitalWrite(pinoD, numeros[n][3]);
  digitalWrite(pinoE, numeros[n][4]);
  digitalWrite(pinoF, numeros[n][5]);
  digitalWrite(pinoG, numeros[n][6]);
}

void abrirPortaoEntrada() {
  ledAbrirPortao.ligar();
  ledFecharPortao.desligar();
  estadoAtualPortao = ABERTO_ENTRADA;
}

void abrirPortaoSaida() {
  ledAbrirPortao.ligar();
  ledFecharPortao.desligar();
  estadoAtualPortao = ABERTO_SAIDA;
}

void fecharPortao() {
  ledAbrirPortao.desligar();
  ledFecharPortao.ligar();
  estadoAtualPortao = FECHADO;
}

void setup() {
  pinMode(pinoA, OUTPUT);
  pinMode(pinoB, OUTPUT);
  pinMode(pinoC, OUTPUT);
  pinMode(pinoD, OUTPUT);
  pinMode(pinoE, OUTPUT);
  pinMode(pinoF, OUTPUT);
  pinMode(pinoG, OUTPUT);
  atualizarDisplay(TemVaga);
  estadoAtualPortao = FECHADO;
}

void loop() {
  bool carroExterno = sensorExterno.readState();
  bool carroInterno = sensorInterno.readState();

  switch (estadoAtualSistema) {

    case AGUARDANDO:
      if (carroExterno && TemVaga > 0) {
        abrirPortaoEntrada();
        estadoAtualSistema = ABRINDO_ENTRADA;
      }
      else if (carroInterno && !carroExterno) {
        abrirPortaoSaida();
        estadoAtualSistema = ABRINDO_SAIDA;
      }
      break;

    case ABRINDO_ENTRADA:
      if (carroInterno) {
        estadoAtualSistema = CARRO_ENTRANDO;
      }
      break;

    case CARRO_ENTRANDO:
      if (!carroInterno) {
        estadoAtualSistema = FECHANDO_ENTRADA;
      }
      break;

    case FECHANDO_ENTRADA:
      if (!carroExterno) {
        TemVaga--;
        fecharPortao();
        estadoAtualSistema = AGUARDANDO;
      }
      break;

    case ABRINDO_SAIDA:
      if (carroExterno) {
        estadoAtualSistema = CARRO_SAINDO;
      }
      break;

    case CARRO_SAINDO:
      if (!carroExterno) {
        estadoAtualSistema = FECHANDO_SAIDA;
      }
      break;

    case FECHANDO_SAIDA:
      if (!carroInterno) {
        TemVaga++;
        fecharPortao();
        estadoAtualSistema = AGUARDANDO;
      }
      break;
  }

  atualizarDisplay(TemVaga);
  delay(100);
}