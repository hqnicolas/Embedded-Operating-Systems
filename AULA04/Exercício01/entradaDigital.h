#ifndef ENTRADADIGITAL_H
#define ENTRADADIGITAL_H

#include <Arduino.h> // Inclui as funções básicas do Arduino como pinMode, digitalRead, millis.

class entradaDigital {
private:
  int pino;
  bool estadoAtual;     // O estado atual do pino após o debounce.
  bool estadoAnterior;  // O último estado RAW lido, usado para detectar mudanças e reiniciar o timer.
  unsigned long ultimoTempo; // O último momento em que o pino mudou de estado RAW.

  // Constante para o tempo de debounce em milissegundos.
  // Um valor comum é 50ms, mas pode ser ajustado conforme a necessidade do hardware.
  static const unsigned long debounceDelay = 50;

public:
  // Construtor: Inicializa o objeto com o pino especificado.
  entradaDigital(int p);

  // Método para configurar o pino como entrada com pull-down.
  void iniciar();

  // Método para retornar o estado atual (debounced) do botão.
  // Retorna HIGH se o botão está pressionado.
  bool pressionado();

  // Método que detecta um "pulso" (borda de subida) do botão após o debounce.
  // Retorna true apenas no instante em que o botão é pressionado (e estabilizado).
  bool pulso();
};

#endif