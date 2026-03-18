#include "sensor.h"

Sensor::Sensor(int pin) : Pino(pin), ultimoEstado(LOW), EstadoAtual(LOW), ultimoDebounce(0), TempoDebounce(50) {
  pinMode(Pino, INPUT);
}

bool Sensor::readState() {
  bool reading = digitalRead(Pino);
  if (reading != ultimoEstado) {
    ultimoDebounce = millis();
  }

  if ((millis() - ultimoDebounce) > TempoDebounce) {
    if (reading != EstadoAtual) {
      EstadoAtual = reading;
    }
  }
  ultimoEstado = reading;
  return EstadoAtual;
}