# Atividade 1 - Sequencia de LEDs com Software Timer

## Objetivo

Criar uma sequencia de acionamento de 4 LEDs usando um **software timer** do FreeRTOS.

## Hardware utilizado

```c
#define LED1 4
#define LED2 0
#define LED3 2
#define LED4 15
```

## Sequencia desejada

1. Ligar o `LED1`
2. Aguardar `250 ms`
3. Ligar o `LED2`
4. Aguardar `250 ms`
5. Ligar o `LED3`
6. Aguardar `250 ms`
7. Ligar o `LED4`
8. Ao chegar no ultimo LED, apagar todos

## Observacao sobre a implementacao

Nesta atividade, a selecao de qual LED sera ligado pode ser feita de duas formas:

* usando uma fila para informar a uma task qual LED deve ser acionado
* usando uma variavel global somadora para controlar o passo atual da sequencia

Para simplificar, o exemplo desta pasta usa uma **variavel global** como contador de estado.

## Arquivo de exemplo

O exemplo foi criado em [SequenciaLEDsTimer.cpp](./SequenciaLEDsTimer.cpp).

## Logica do exemplo

* Um timer periodico executa a cada `250 ms`
* A cada disparo, um novo LED e ligado
* Os LEDs permanecem acesos de forma acumulativa
* Quando o `LED4` ja tiver sido ligado, todos os LEDs sao apagados
* Em seguida, a sequencia recomeca do inicio

## Resultado esperado

A placa deve apresentar o seguinte comportamento ciclico:

```text
LED1
LED1 + LED2
LED1 + LED2 + LED3
LED1 + LED2 + LED3 + LED4
todos apagados
reinicia a sequencia
```
