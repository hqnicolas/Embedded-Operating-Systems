#include "Val.h"

Val::Val(int p) : pin(p) {}

void Val::begin() {
  pinMode(pin, OUTPUT);
  close();
}

void Val::open() {
  digitalWrite(pin, HIGH);
}

void Val::close() {
  digitalWrite(pin, LOW);
}