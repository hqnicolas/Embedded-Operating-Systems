# Exercício 01

## Objetivo
Implementar a comunicação entre duas tarefas no FreeRTOS utilizando uma fila para transportar leituras analógicas de um potenciômetro.

## Descrição
O programa foi desenvolvido para um ESP32 com leitura analógica no **GPIO 34**.

Uma tarefa faz a leitura contínua do potenciômetro e envia os valores para uma fila em memória. Outra tarefa recebe esses valores e mostra o resultado no monitor serial de forma proporcional à posição do potenciômetro.

## Requisitos Atendidos
- Leitura analógica no **POT1** conectado ao **GPIO 34**
- Resolução de **12 bits**
- Referência de **3,3 V**
- Comunicação entre tarefas usando **fila**
- Fila com capacidade para **5 valores**
- Bloqueio da tarefa emissora quando a fila está cheia, usando `portMAX_DELAY`

## Estrutura da Solução

### Task emissora
- Lê o valor analógico com `analogRead()`
- Envia o valor para a fila com `xQueueSend()`
- Fica bloqueada automaticamente se a fila atingir 5 elementos e a receptora ainda não tiver consumido dados

### Task receptora
- Recebe os valores com `xQueueReceive()`
- Exibe no monitor serial:
- Valor bruto do ADC
- Tensão equivalente em volts
- Percentual aproximado da posição do potenciômetro

## Arquivo Principal
O código da atividade está em [sketch.ino](/c:/Users/nicolas.337911/Downloads/Embedded-Operating-Systems/AULA02/Exercício01/sketch.ino).

## Exemplo de saída serial
```text
ADC: 0 | Tensao: 0.00 V | Potenciometro: 0.0%
ADC: 2048 | Tensao: 1.65 V | Potenciometro: 50.0%
ADC: 4095 | Tensao: 3.30 V | Potenciometro: 100.0%
```

## Observação
Os arquivos `entradaDigital.h` e `entradaDigital.cpp` permaneceram na pasta, mas não são utilizados nesta implementação.
