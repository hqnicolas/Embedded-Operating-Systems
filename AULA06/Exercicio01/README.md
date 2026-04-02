# AULA06 - Exercício 01: Controle de Display de 7 Segmentos com FreeRTOS

## Descrição da Atividade

Este exercício consiste em criar um programa para controlar um display de 7 segmentos utilizando FreeRTOS. A contagem numérica no display será iniciada e parada por um pulso de uma chave (botão). A contagem será automática, com um intervalo de tempo fixo entre cada incremento.

O programa deve permitir a passagem de parâmetros para a task na sua inicialização, garantindo flexibilidade e modularidade.

## Funcionalidades Principais

*   **Controle de Display 7 Segmentos:** Exibição de números de 0 a 9.
*   **Start/Stop por Botão:** Um único botão atua como toggle para iniciar e pausar a contagem. Um pulso liga, outro pulso para.
*   **Contagem Automática:** Incremento automático do contador em um intervalo de tempo configurável.
*   **Contagem Cíclica:** A contagem reinicia de 0 ao atingir a contagem máxima definida.
*   **Passagem de Parâmetros para a Task:**
    *   Número do pino de start/stop (do botão).
    *   Contagem Máxima (0-9).
    *   Tempo de intervalo entre os incrementos (em milissegundos).

## Conceitos Utilizados

*   **FreeRTOS Tasks:** Criação e gerenciamento de uma task dedicada para o controle do display e da lógica do botão.
*   **Passagem de Parâmetros:** Utilização de uma estrutura (struct) para agrupar e passar múltiplos parâmetros para a task no momento de sua criação.
*   **Entrada Digital:** Leitura do estado de um botão com `INPUT_PULLDOWN`.
*   **Saída Digital:** Controle dos segmentos do display para formar os números.
*   **Debounce:** Lógica para evitar múltiplos acionamentos do botão com um único clique.


## Estrutura do Código (Exemplo Conceitual)

A seguir, apresentamos um esboço da estrutura do código que implementaria as funcionalidades descritas, baseando-se no código inicial fornecido e nos exemplos de FreeRTOS

```cpp
const int pinoA = 18;
const int pinoB = 19;
const int pinoC = 0;
const int pinoD = 4;
const int pinoE = 16;
const int pinoF = 5;
const int pinoG = 17;

const int btnAumenta = 14;
const int btnDiminui = 12;

int contador = 0;

const byte numeros[10][7] = {
  {1, 1, 1, 1, 1, 1, 0}, 
  {0, 1, 1, 0, 0, 0, 0},
  {1, 1, 0, 1, 1, 0, 1}, 
  {1, 1, 1, 1, 0, 0, 1}, 
  {0, 1, 1, 0, 0, 1, 1},
  {1, 0, 1, 1, 0, 1, 1}, 
  {1, 0, 1, 1, 1, 1, 1},
  {1, 1, 1, 0, 0, 0, 0}, 
  {1, 1, 1, 1, 1, 1, 1}, 
  {1, 1, 1, 1, 0, 1, 1} 
};

void atualizarDisplay(int n) {
  digitalWrite(pinoA, numeros[n][0]);
  digitalWrite(pinoB, numeros[n][1]);
  digitalWrite(pinoC, numeros[n][2]);
  digitalWrite(pinoD, numeros[n][3]);
  digitalWrite(pinoE, numeros[n][4]);
  digitalWrite(pinoF, numeros[n][5]);
  digitalWrite(pinoG, numeros[n][6]);
}


void setup() {
  // Configura segmentos como saída
  int pinos[] = {pinoA, pinoB, pinoC, pinoD, pinoE, pinoF, pinoG};
  for(int i=0; i<7; i++) pinMode(pinos[i], OUTPUT);

  pinMode(btnAumenta, INPUT_PULLDOWN);
  pinMode(btnDiminui, INPUT_PULLDOWN);

  atualizarDisplay(contador);
}
```