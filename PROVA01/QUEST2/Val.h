#ifndef VAL_H
#define VAL_H

#include <Arduino.h>

class Val {
  private:
    int pin;
  public:
    Val(int p);
    void begin();
    void open();
    void close();
};

#endif