#include "entradaDigital.h"

entradaDigital::entradaDigital(int p) {
  pino = p;
  estadoAtual = false;      // Inicializa o estado debounced como desligado.
  estadoAnterior = false;   // Inicializa o último estado RAW lido como desligado.
  ultimoTempo = 0;          // Inicializa o timer de debounce.
}

void entradaDigital::iniciar() {
  pinMode(pino, INPUT_PULLDOWN); // Configura o pino como entrada com pull-down.
}

bool entradaDigital::pressionado() {
  // Retorna o estado RAW do pino. Para um debounce completo, o ideal seria retornar 'estadoAtual'.
  // Contudo, a lógica de 'pulso()' já se encarrega do debounce.
  return digitalRead(pino) == HIGH;
}

bool entradaDigital::pulso() {
  bool leitura = digitalRead(pino); // Lê o estado RAW atual do pino.

  // Se a leitura RAW mudou em relação à leitura RAW anterior,
  // reinicia o timer de debounce, pois uma nova transição pode estar começando.
  if (leitura != estadoAnterior) {
    ultimoTempo = millis(); 
  }

  // Verifica se o tempo de debounce passou desde a última mudança RAW.
  if ((millis() - ultimoTempo) > debounceDelay) {
    // Se o tempo passou e a leitura RAW atual é diferente do estado debounced atual,
    // significa que o estado se estabilizou em um novo valor.
    if (leitura != estadoAtual) {
      estadoAtual = leitura; // Atualiza o estado debounced.

      // Se o novo estado debounced é HIGH (botão foi pressionado),
      // retorna true para indicar um pulso.
      if (estadoAtual == HIGH) {
        return true; // Detecta a borda de subida (botão pressionado).
      }
    }
  }

  // Atualiza o estado RAW anterior para a próxima iteração.
  estadoAnterior = leitura;
  return false; // Nenhum pulso detectado.
}