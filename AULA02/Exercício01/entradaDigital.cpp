#include "entradaDigital.h"

entradaDigital::entradaDigital(int p) {
  pino = p;
  estadoAtual = false;
  estadoAnterior = false;
  ultimoTempo = 0;
}

void entradaDigital::iniciar() {
  pinMode(pino, INPUT_PULLDOWN);
}

bool entradaDigital::pressionado() {
  return digitalRead(pino) == HIGH;
}

bool entradaDigital::pulso() {
  bool leitura = digitalRead(pino);

  if (leitura != estadoAnterior) {
    ultimoTempo = millis(); // reinicia tempo de debounce
  }

  if ((millis() - ultimoTempo) > debounceDelay) {
    if (leitura != estadoAtual) {
      estadoAtual = leitura;

      if (estadoAtual == HIGH) {
        estadoAnterior = leitura;
        return true; // detecta borda de subida
      }
    }
  }

  estadoAnterior = leitura;
  return false;
}