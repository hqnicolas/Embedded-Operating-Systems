#ifndef LED_H
#define LED_H

#include <Arduino.h>

class LED {
public:
  LED(int pin);
  void ligar();
  void desligar();
  void toggle();

private:
  int Pino;
};

#endif