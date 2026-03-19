#include "SensorDeNivel.h"

SensorDeNivel::SensorDeNivel(int p) : pin(p) {}

void SensorDeNivel::begin() {
  pinMode(pin, INPUT_PULLDOWN);
}

bool SensorDeNivel::isFull() {
  return digitalRead(pin) == HIGH;
}