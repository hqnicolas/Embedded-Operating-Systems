#include "Val.h"
#include "Botao.h"
#include "Alarme.h"
#include "SensorDeNivel.h"

#define BTN_START 14
#define NIVEL_PIN 12
#define V1_PIN 19
#define V2_PIN 18
#define V3_PIN 5
#define LED_ALARME 17

Val v1(V1_PIN);
Val v2(V2_PIN);
Val v3(V3_PIN);
Botao startBtn(BTN_START);
Alarme Alarme(LED_ALARME);
SensorDeNivel nivel(NIVEL_PIN);

enum State {
  PARADO,
  V1_ON,
  PERA_1,
  V2_ON,
  V3_ON,
  FEITO,
  ERROR
};

State estado = PARADO;

unsigned long Quando = 0;

void stopAll() {
  v1.close();
  v2.close();
  v3.close();
}

void setup() {
  v1.begin();
  v2.begin();
  v3.begin();
  startBtn.begin();
  Alarme.begin();
  nivel.begin();
}


void loop() {
  if (nivel.isFull()) {
    stopAll();
    estado = ERROR;
  }

  if (estado == ERROR) {
    Alarme.update();
    return;
  }

  unsigned long now = millis();

  switch (estado) {

    case PARADO:
      if (startBtn.pressed()) {
        v1.open();
        Quando = now;
        estado = V1_ON;
      }
      break;

    case V1_ON:
      if (now - Quando >= 2000) {
        v1.close();
        Quando = now;
        estado = PERA_1;
      }
      break;

    case PERA_1:
      if (now - Quando >= 1000) {
        v2.open();
        Quando = now;
        estado = V2_ON;
      }
      break;

    case V2_ON:
      if (now - Quando >= 2500) {
        v2.close();
        v3.open();
        Quando = now;
        estado = V3_ON;
      }
      break;

    case V3_ON:
      if (now - Quando >= 2000) {
        v3.close();
        estado = FEITO;
      }
      break;

    case FEITO:
      estado = PARADO;
      break;
  }
}