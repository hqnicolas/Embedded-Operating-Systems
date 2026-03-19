#include "led.h"

LED::LED(int pin) : Pino(pin) {
  pinMode(Pino, OUTPUT);
  digitalWrite(Pino, LOW);
}

void LED::ligar() {
  digitalWrite(Pino, HIGH);
}

void LED::desligar() {
  digitalWrite(Pino, LOW);
}

void LED::toggle() {
  digitalWrite(Pino, !digitalRead(Pino));
}