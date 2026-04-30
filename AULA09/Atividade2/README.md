# Atividade 2 - Sistema de Bomba com FreeRTOS

## Objetivo

Desenvolver uma aplicacao usando **FreeRTOS** para simular um sistema de contagem regressiva de fogos, integrando sensores, botoes, display de 7 segmentos, LEDs e os recursos estudados ao longo da disciplina.

## Descricao geral

Um sensor `LDR` deve detectar quando o ambiente estiver escuro. Quando isso acontecer, o sistema deve iniciar uma contagem regressiva de **59 segundos** para o disparo dos fogos.

Durante a contagem:

* o tempo restante deve ser exibido no display
* um botao deve permitir cancelar o disparo
* outro botao deve acelerar a contagem
* outro botao deve forcar a explosao imediatamente
* o LED RGB deve indicar visualmente o estado do sistema

## Requisitos funcionais

### 1. Disparo automatico com LDR

* O `LDR` ativa os fogos
* Quanto mais escuro o ambiente, o sistema deve iniciar a contagem
* O intervalo inicial da contagem deve ser de `1 segundo`
* O display deve mostrar a contagem regressiva iniciando em `59 segundos`

### 2. Botao de cancelamento

* O botao no pino `4` desativa os fogos
* Esse botao representa a acao de salvar antes da explosao

### 3. Botao de aceleracao

* O botao no pino `0` acelera o timer
* Ao ser acionado, o intervalo da contagem deve passar de `1 segundo` para `0,5 segundo`

### 4. Botao de explosao imediata

* O botao no pino `2` aciona imediatamente a explosao dos fogos

### 5. Indicacao por LED RGB

A explosao e o estado do sistema devem ser representados no LED RGB:

* `Verde`: os fogos foram acionados
* `Azul`: o sistema foi desativado a tempo

## Hardware utilizado

```c
#define BOTAO1 4
#define BOTAO2 0
#define BOTAO3 2
#define RGB_RED 25
#define RGB_BLUE 27
#define LDR 39

static const uint8_t SEG_A = 18;
static const uint8_t SEG_B = 5;
static const uint8_t SEG_C = 21;
static const uint8_t SEG_D = 3;
static const uint8_t SEG_E = 1;
static const uint8_t SEG_F = 23;
static const uint8_t SEG_G = 22;
static const uint8_t SEG_DP = 19;
static const uint8_t DISPLAY_DEZENA = 16;
static const uint8_t DISPLAY_UNIDADE = 17;
```

## Tabela dos numeros do display

```c
const byte numeros[10][7] = {
  {1,1,1,1,1,1,0},
  {0,1,1,0,0,0,0},
  {1,1,0,1,1,0,1},
  {1,1,1,1,0,0,1},
  {0,1,1,0,0,1,1},
  {1,0,1,1,0,1,1},
  {1,0,1,1,1,1,1},
  {1,1,1,0,0,0,0},
  {1,1,1,1,1,1,1},
  {1,1,1,1,0,1,1}
};
```

## Integracao com as aulas anteriores

Quando este enunciado diz para **"usar todas as ferramentas possiveis de timer e de task semaforos"**, a ideia e integrar os principais conceitos estudados **das aulas 1 ate 8**, junto com os **software timers da aula 9**.

Isso significa que a atividade pode combinar, quando fizer sentido:

* criacao e gerenciamento de **tasks**
* sincronizacao com **semaforos**
* protecao de recursos compartilhados com **mutex**
* comunicacao entre tarefas com **filas**
* leitura de entradas digitais e analogicas
* controle de display e saídas digitais
* uso de **software timers** `one-shot` e `auto reload`

## Sugestao de organizacao

Uma forma de estruturar a atividade e separar responsabilidades em tarefas, por exemplo:

* uma task para leitura do `LDR`
* uma task para leitura dos botoes
* uma task para atualizacao do display
* uma task para controlar o estado geral dos fogos
* um ou mais software timers para a contagem regressiva

## Desafio da atividade

Montar uma solucao em FreeRTOS que seja organizada, concorrente e segura, aproveitando o maximo possivel dos recursos apresentados ao longo das aulas anteriores.
