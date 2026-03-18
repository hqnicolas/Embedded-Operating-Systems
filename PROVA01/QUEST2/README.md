# Sistema de Dosagem e Mistura Química com ESP32

## Visão Geral

Este projeto descreve a lógica de programação para um sistema de dosagem e mistura de matérias-primas utilizando a plataforma ESP32. O sistema controla a sequência de abertura de três válvulas de descarga de tanques de matéria-prima (V1, V2, V3) para um tanque de mistura. Um sensor de nível máximo no tanque de mistura garante que o mesmo não transborde, ativando um alarme visual e interrompendo o processo em caso de emergência.

## Requisitos Funcionais

O sistema deve operar da seguinte forma:

1.  **Início do Ciclo:** Ao pressionar o botão `Start`:
    *   A válvula V1 deve ser aberta.
    *   Após 2 segundos, a V1 deve ser fechada.
    *   Aguardar 1 segundo.
    *   A válvula V2 deve ser aberta.
    *   Após 2.5 segundos, a V2 deve ser fechada.
    *   A válvula V3 deve ser aberta.
    *   Após 2 segundos, a V3 deve ser fechada, encerrando o ciclo normal.
2.  **Controle de Tempo:** Todos os tempos de acionamento das válvulas e intervalos devem ser controlados sem o uso da função `delay()`, utilizando timers para garantir uma execução não bloqueante.
3.  **Monitoramento de Nível:**
    *   Um sensor de nível máximo no tanque de mistura deve ser monitorado continuamente.
    *   Caso o nível máximo seja atingido em qualquer ponto do ciclo, todas as válvulas (V1, V2, V3) devem ser imediatamente fechadas.
    *   Um alarme visual (LED) deve ser acionado, piscando intermitentemente, indicando a condição de transbordamento ou nível crítico.

## Pinagem do Hardware (ESP32)

As seguintes conexões de hardware devem ser realizadas com os pinos do ESP32, configurados como `ENABLE UP` onde aplicável:

*   **Botão Start:** `GPIO 14` (Entrada, `ENABLE UP`)
*   **Sensor de Nível Máximo:** `GPIO 12` (Entrada, `ENABLE UP`)
*   **Válvula 1 (V1):** `GPIO 19` (Saída, `ENABLE UP`)
*   **Válvula 2 (V2):** `GPIO 18` (Saída, `ENABLE UP`)
*   **Válvula 3 (V3):** `GPIO 5` (Saída, `ENABLE UP`)
*   **Alarme Visual (LED):** `GPIO 17` (Saída, `ENABLE UP`)

## Lógica de Programação

A lógica de programação deverá implementar uma máquina de estados ou um sistema baseado em timers para gerenciar a sequência das válvulas e a interrupção por falha de nível, garantindo que a execução seja assíncrona e responsiva.