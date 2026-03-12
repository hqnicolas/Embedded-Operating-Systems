// Remove a definição duplicada da classe EntradaDigital que estava aqui.
// Agora, incluímos o arquivo de cabeçalho da nossa biblioteca.
#include "entradaDigital.h"

// Criando objetos da classe entradaDigital
entradaDigital botao1(15); // Cria um objeto para o botão no pino 15
entradaDigital botao2(2); // Cria um objeto para o botão no pino 2

void setup() {
  Serial.begin(9600); // Inicia a comunicação serial para debug
  botao1.iniciar();   // Configura o pino do botão 1
  botao2.iniciar();   // Configura o pino do botão 2
}

void loop() {

  // Verifica se o botão 1 foi pressionado (com debounce)
  if (botao1.pulso()) {
    Serial.println("Botão 1 pressionado (pulso)!");
  }

}