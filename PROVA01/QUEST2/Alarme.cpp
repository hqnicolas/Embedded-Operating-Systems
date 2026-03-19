#include "Alarme.h"

Alarme::Alarme(int p) : pin(p), lastTime(0), state(false) {}

void Alarme::begin() {
  pinMode(pin, OUTPUT);
}

void Alarme::update() {
  if (millis() - lastTime >= 300) {
    lastTime = millis();
    state = !state;
    digitalWrite(pin, state);
  }
}