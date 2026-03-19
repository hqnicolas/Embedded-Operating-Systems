#ifndef ALARME_H
#define ALARME_H

#include <Arduino.h>

class Alarme {
  private:
    int pin;
    unsigned long lastTime;
    bool state;
  public:
    Alarme(int p);
    void begin();
    void update();
};

#endif