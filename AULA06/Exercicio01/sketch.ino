#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Pinos do display de 7 segmentos
const int pinoA = 18;
const int pinoB = 19;
const int pinoC = 0;
const int pinoD = 4;
const int pinoE = 16;
const int pinoF = 5;
const int pinoG = 17;

// Estrutura para passagem de parâmetros
typedef struct {
  int pinoBotao;      // Pino do botão start/stop
  int contagemMax;    // Contagem máxima (0-9)
  int tempoIntervalo; // Tempo de intervalo em ms
} ParametrosTask;

// Tabela de números para display (ânodo comum)
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
  
  // Configura segmentos como saída
  int pinos[] = {pinoA, pinoB, pinoC, pinoD, pinoE, pinoF, pinoG};
  for (int i = 0; i < 7; i++) {
    pinMode(pinos[i], OUTPUT);
  }
  
  // Display inicial em 0
  atualizarDisplay(0);

  // Cria estrutura com parâmetros
  ParametrosTask params = {
    .pinoBotao = 14,       // Pino do botão start/stop
    .contagemMax = 9,      // Contagem máxima (0-9)
    .tempoIntervalo = 500  // Intervalo de 500ms
  };

  // Cria task passando os parâmetros
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
    
    // Detecção de pulso (borda de descida)
    if (estadoAnteriorBtn == HIGH && estadoAtualBtn == LOW) {
      contando = !contando; // Alterna estado
      Serial.print("Contagem: ");
      Serial.println(contando ? "INICIADA" : "PAUSADA");
    }
    
    estadoAnteriorBtn = estadoAtualBtn;
    
    if (contando) {
      atualizarDisplay(contador);
      contador++;
      
      // Reinicia ao atingir contagem máxima
      if (contador > contagemMax) {
        contador = 0;
      }
    }
    
    vTaskDelay(pdMS_TO_TICKS(tempo));
  }
}