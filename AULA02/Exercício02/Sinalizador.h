#ifndef SINALIZADOR_H
#define SINALIZADOR_H

#include <Arduino.h>

class Sinalizador {
  private:
    int _pino;
    unsigned long _tempoCurto;
    unsigned long _tempoLongo;
    unsigned long _intervalo;

    void pulso(unsigned long tempoOn, unsigned long tempoOff);

  public:
    Sinalizador(int pino, unsigned long tempoCurto, 
                unsigned long tempoLongo, unsigned long intervalo);

    void begin();
    void statusOK();
    void atencao(int ciclos);
    void emergencia();
};

#endif