#include "Botao.h"

Botao::Botao(int p) : pin(p), lastState(LOW) {}

void Botao::begin() {
  pinMode(pin, INPUT_PULLDOWN);
}

bool Botao::pressed() {
  bool current = digitalRead(pin);
  if (lastState == LOW && current == HIGH) {
    lastState = current;
    return true;
  }
  lastState = current;
  return false;
}