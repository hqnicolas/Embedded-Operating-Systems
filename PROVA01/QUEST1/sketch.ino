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
  {1,1,1,1,1,1,0},
  {0,1,1,0,0,0,0},
  {1,1,0,1,1,0,1},
  {1,1,1,1,0,0,1},
  {0,1,1,0,0,1,1},
  {1,0,1,1,0,1,1},
  {1,0,1,1,1,1,1},
  {1,1,1,0,0,0,0},
  {1,1,1,1,1,1,1},
  {1,1,1,1,0,1,1}
};

Sensor sensorInterno(SENSOR_INTERNO_PIN);
Sensor sensorExterno(SENSOR_EXTERNO_PIN);
LED ledAbrirPortao(LED_ABRE_PORTAO_PIN);
LED ledFecharPortao(LED_FECHA_PORTAO_PIN);

const int TOTAL_VAGAS = 5;
int vagasDisponiveis = TOTAL_VAGAS;

enum EstadoSistema {
  AGUARDANDO,
  ENTRANDO_1,
  ENTRANDO_2,
  SAINDO_1,
  SAINDO_2
};

EstadoSistema estado = AGUARDANDO;

void atualizarDisplay(int n) {
  n = constrain(n, 0, 9);

  digitalWrite(pinoA, numeros[n][0]);
  digitalWrite(pinoB, numeros[n][1]);
  digitalWrite(pinoC, numeros[n][2]);
  digitalWrite(pinoD, numeros[n][3]);
  digitalWrite(pinoE, numeros[n][4]);
  digitalWrite(pinoF, numeros[n][5]);
  digitalWrite(pinoG, numeros[n][6]);
}

void abrirPortao() {
  ledAbrirPortao.ligar();
  ledFecharPortao.desligar();
}

void fecharPortao() {
  ledAbrirPortao.desligar();
  ledFecharPortao.ligar();
}

void setup() {
  pinMode(pinoA, OUTPUT);
  pinMode(pinoB, OUTPUT);
  pinMode(pinoC, OUTPUT);
  pinMode(pinoD, OUTPUT);
  pinMode(pinoE, OUTPUT);
  pinMode(pinoF, OUTPUT);
  pinMode(pinoG, OUTPUT);

  pinMode(SENSOR_INTERNO_PIN, INPUT);
  pinMode(SENSOR_EXTERNO_PIN, INPUT);

  atualizarDisplay(vagasDisponiveis);
  fecharPortao();
}

void loop() {
  bool ext = sensorExterno.readState();
  bool intl = sensorInterno.readState();

  switch (estado) {

    case AGUARDANDO:
      if (ext && vagasDisponiveis > 0) {
        abrirPortao();
        estado = ENTRANDO_1;
      }
      else if (intl) {
        abrirPortao();
        estado = SAINDO_1;
      }
      break;

    case ENTRANDO_1:
      if (intl) {
        estado = ENTRANDO_2;
      }
      break;

    case ENTRANDO_2:
      if (!ext && !intl) {
        vagasDisponiveis--;
        vagasDisponiveis = constrain(vagasDisponiveis, 0, TOTAL_VAGAS);
        fecharPortao();
        estado = AGUARDANDO;
      }
      break;

    case SAINDO_1:
      if (ext) {
        estado = SAINDO_2;
      }
      break;

    case SAINDO_2:
      if (!ext && !intl) {
        vagasDisponiveis++;
        vagasDisponiveis = constrain(vagasDisponiveis, 0, TOTAL_VAGAS);
        fecharPortao();
        estado = AGUARDANDO;
      }
      break;
  }

  atualizarDisplay(vagasDisponiveis);
  delay(50);
}