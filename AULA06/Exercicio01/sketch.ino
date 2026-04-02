#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

const int pinoA = 18;
const int pinoB = 19;
const int pinoC = 0;
const int pinoD = 4;
const int pinoE = 16;
const int pinoF = 5;
const int pinoG = 17;

typedef struct {
  int pinoBotao;
  int contagemMax;
  int tempoIntervalo;
} ParametrosTask;

const byte numeros[10][7] = {
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}  // 9
};

TaskHandle_t taskDisplayHandle = NULL;

void atualizarDisplay(int n);
void vTaskDisplay(void *pvParameters);

void setup() {
  Serial.begin(9600);
  
  int pinos[] = {pinoA, pinoB, pinoC, pinoD, pinoE, pinoF, pinoG};
  for (int i = 0; i < 7; i++) {
    pinMode(pinos[i], OUTPUT);
  }
  
  atualizarDisplay(0);

  ParametrosTask params = {
    .pinoBotao = 14,
    .contagemMax = 9,
    .tempoIntervalo = 500
  };

  xTaskCreate(vTaskDisplay, "TaskDisplay", 2048, (void*)&params, 1, &taskDisplayHandle);
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(3000));
}

void atualizarDisplay(int n) {
  if (n >= 0 && n <= 9) {
    digitalWrite(pinoA, numeros[n][0]);
    digitalWrite(pinoB, numeros[n][1]);
    digitalWrite(pinoC, numeros[n][2]);
    digitalWrite(pinoD, numeros[n][3]);
    digitalWrite(pinoE, numeros[n][4]);
    digitalWrite(pinoF, numeros[n][5]);
    digitalWrite(pinoG, numeros[n][6]);
  }
}

void vTaskDisplay(void *pvParameters) {
  ParametrosTask *params = (ParametrosTask*)pvParameters;
  
  int pinoBtn = params->pinoBotao;
  int contagemMax = params->contagemMax;
  int tempo = params->tempoIntervalo;
  
  pinMode(pinoBtn, INPUT_PULLDOWN);
  
  int contador = 0;
  bool contando = false;
  bool estadoAnteriorBtn = HIGH;
  
  while (1) {
    bool estadoAtualBtn = !digitalRead(pinoBtn);
    
    if (estadoAnteriorBtn == HIGH && estadoAtualBtn == LOW) {
      contando = !contando;
      Serial.print("Contagem: ");
      Serial.println(contando ? "INICIADA" : "PAUSADA");
    }
    
    estadoAnteriorBtn = estadoAtualBtn;
    
    if (contando) {
      atualizarDisplay(contador);
      contador++;
      
      if (contador > contagemMax) {
        contador = 0;
      }
    }
    
    vTaskDelay(pdMS_TO_TICKS(tempo));
  }
}