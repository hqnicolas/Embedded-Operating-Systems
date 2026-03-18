# Sistema de Estacionamento Inteligente para ESP32

## Descrição do Projeto
Este projeto implementa um sistema de controle de acesso para um estacionamento de carros com um único portão de entrada e saída, utilizando um ESP32. O sistema gerencia o fluxo de veículos, exibe o número de vagas disponíveis em um display de 7 segmentos e controla a abertura/fechamento do portão com base na ocupação e nos sensores de presença de carros.

## Funcionalidades
*   **Controle de Portão Automático:**
    *   O portão abre para a entrada quando um carro é detectado pelo sensor externo.
    *   O portão fecha para a entrada somente após o carro sair do sensor interno.
    *   O portão abre para a saída quando um carro é detectado pelo sensor interno (querendo sair).
    *   O portão fecha para a saída somente após o carro sair do sensor externo.
*   **Contagem de Vagas:**
    *   O sistema monitora e atualiza o número de vagas disponíveis.
    *   Inicia com 5 vagas disponíveis.
    *   Decrementa uma vaga quando um carro entra (passa pelo sensor externo e depois pelo interno).
    *   Incrementa uma vaga quando um carro sai (passa pelo sensor interno e depois pelo externo).
*   **Indicação Luminosa:**
    *   LEDs indicam o estado de abertura (`ABRE`) ou fechamento (`FECHA`) do portão.
*   **Display de 7 Segmentos:**
    *   Exibe o número atual de vagas disponíveis.
*   **Restrição de Entrada:**
    *   Quando o número de vagas chega a zero, o portão não abrirá mais para entrada de carros.

## Hardware Utilizado
*   **Microcontrolador:** ESP32
*   **Sensores de Carro:**
    *   Sensor de carro aguardando para entrar (Sensor Externo)
    *   Sensor de carro entrando (Sensor Interno)
*   **Display:** Display de 7 segmentos
*   **Indicadores Visuais:** LEDs para `ABRE` e `FECHA`
*   **Portão Automático:** Considerado um hardware interno independente, com entradas para "abrir" e "fechar".

## Pinagem do ESP32

### Sensores de Carro
*   **Sensor Interno:** `GPIO14` (HIGH = carro detectado, LOW = sem carro)
*   **Sensor Externo:** `GPIO12` (HIGH = carro detectado, LOW = sem carro)

### LEDs de Indicação do Portão
*   **LED ABRIR Portão:** `GPIO26`
*   **LED FECHAR Portão:** `GPIO27`

### Display de 7 Segmentos
| Segmento | Pino ESP32 |
| :------- | :--------- |
| A        | `GPIO18`   |
| B        | `GPIO19`   |
| C        | `GPIO0`    |
| D        | `GPIO4`    |
| E        | `GPIO16`   |
| F        | `GPIO5`    |
| G        | `GPIO17`   |