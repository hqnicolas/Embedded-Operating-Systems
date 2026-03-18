#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>

class Sensor {
public:
  Sensor(int pin);
  bool readState();

private:
  int Pino;
  bool ultimoEstado;
  bool EstadoAtual;
  unsigned long ultimoDebounce;
  unsigned long TempoDebounce;
};

#endif