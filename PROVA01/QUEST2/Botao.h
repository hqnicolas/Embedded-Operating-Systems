#ifndef BOTAO_H
#define BOTAO_H

#include <Arduino.h>

class Botao {
  private:
    int pin;
    bool lastState;
  public:
    Botao(int p);
    void begin();
    bool pressed();
};

#endif