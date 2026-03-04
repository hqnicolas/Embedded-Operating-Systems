#include "Sinalizador.h"

Sinalizador::Sinalizador(int pino, unsigned long tempoCurto, 
                         unsigned long tempoLongo, unsigned long intervalo) {
  _pino = pino;
  _tempoCurto = tempoCurto;
  _tempoLongo = tempoLongo;
  _intervalo = intervalo;
}

void Sinalizador::begin() {
  pinMode(_pino, OUTPUT);
}

void Sinalizador::pulso(unsigned long tempoOn, unsigned long tempoOff) {
  digitalWrite(_pino, HIGH);
  delay(tempoOn);
  digitalWrite(_pino, LOW);
  delay(tempoOff);
}

void Sinalizador::statusOK() {
  pulso(_tempoCurto, _intervalo);
}

void Sinalizador::atencao(int ciclos) {
  for (int i = 0; i < ciclos; i++) {
    for (int j = 0; j < 3; j++) {
      pulso(_tempoCurto, _tempoCurto);
    }
    delay(_intervalo);
  }
}

void Sinalizador::emergencia() {
  // 3 longos
  for (int i = 0; i < 3; i++) {
    pulso(_tempoLongo, _tempoCurto);
  }

  // 3 curtos
  for (int i = 0; i < 3; i++) {
    pulso(_tempoCurto, _tempoCurto);
  }

  // 3 longos
  for (int i = 0; i < 3; i++) {
    pulso(_tempoLongo, _intervalo);
  }
}