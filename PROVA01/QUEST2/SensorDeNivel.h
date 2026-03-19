#ifndef SENSOR_DE_NIVEL_H
#define SENSOR_DE_NIVEL_H

#include <Arduino.h>

class SensorDeNivel {
  private:
    int pin;
  public:
    SensorDeNivel(int p);
    void begin();
    bool isFull();
};

#endif