#ifndef ENTRADADIGITAL_H
#define ENTRADADIGITAL_H

#include <Arduino.h>

class entradaDigital {
  private:
    int pino; 
    bool estadoAtual;
    bool estadoAnterior;
    unsigned long ultimoTempo;
    const unsigned long debounceDelay = 50; // 50ms

  public:
    entradaDigital(int pino); 
    
    void iniciar();
    bool pressionado();
    bool pulso();
};

#endif