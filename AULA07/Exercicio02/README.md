# Exercício 02

## Objetivo
Expandir o exercício anterior com FreeRTOS para que o valor lido do potenciômetro também seja mostrado em um display de 7 segmentos com dois dígitos, exibindo números de **0 a 99**.

## Descrição
Uma task realiza a leitura do potenciômetro no **GPIO 34** e envia o valor para uma fila com capacidade para **5 posições**.  
Outra task recebe o valor da fila, calcula a informação proporcional da leitura e:

- mostra os dados no monitor serial;
- converte a leitura para a faixa de **0 a 99**;
- atualiza o valor que será exibido no display.

Uma terceira task mantém o display aceso por **multiplexação**, alternando rapidamente entre os dois transistores dos dígitos.

## Hardware Utilizado
- ESP32
- Potenciômetro no **GPIO 34**
- Display de 7 segmentos com **2 dígitos**
- Multiplexação usando os transistores dos pinos:
- `DISPLAY1 = GPIO 16`
- `DISPLAY2 = GPIO 17`

## Mapeamento dos Segmentos
- `A = GPIO 18`
- `B = GPIO 5`
- `C = GPIO 21`
- `D = GPIO 3`
- `E = GPIO 1`
- `F = GPIO 23`
- `G = GPIO 22`
- `DP = GPIO 19`

## Funcionamento

### Task 1 - Leitura do potenciômetro
- Faz a leitura analógica em 12 bits
- Envia o valor para a fila usando `xQueueSend()`
- Fica bloqueada com `portMAX_DELAY` se a fila estiver cheia

### Task 2 - Processamento
- Recebe o valor com `xQueueReceive()`
- Calcula:
- valor bruto do ADC
- tensão em volts
- percentual da leitura
- valor proporcional entre `0` e `99`
- Envia as informações para a serial

### Task 3 - Display
- Atualiza continuamente o display de 7 segmentos
- Faz o mux entre os dois transistores
- Mostra dezena e unidade separadamente, em alta velocidade

## Requisito de Multiplexação
Como os dois dígitos compartilham os segmentos, é necessário ativar apenas um dígito por vez:

1. escreve a dezena nos segmentos;
2. ativa o transistor do primeiro display;
3. desativa;
4. escreve a unidade;
5. ativa o transistor do segundo display;
6. repete continuamente.

Esse processo cria a impressão visual de que os dois dígitos estão acesos ao mesmo tempo.

## Exemplo de saída serial
```text
ADC: 0 | Tensao: 0.00 V | Potenciometro: 0.0% | Display: 0
ADC: 2048 | Tensao: 1.65 V | Potenciometro: 50.0% | Display: 49
ADC: 4095 | Tensao: 3.30 V | Potenciometro: 100.0% | Display: 99
```