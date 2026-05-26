# PROVA04 - Estacao de trabalho Inteligente IoT

## Visao Geral

Este projeto define uma **estacao de trabalho inteligente** baseada em `ESP32`, unindo os conteudos de **Sistemas Operacionais Embarcados** e **Internet das Coisas** em uma proposta unica, pratica e proxima da realidade.

A solucao monitora o ambiente de trabalho, acompanha o ciclo de foco, automatiza a iluminacao local e publica os dados para um dashboard remoto. O sistema usa `FreeRTOS` para organizar a execucao concorrente e `MQTT + Node-RED + HTTP WebServer` como pilha oficial de IoT.

## Problema Resolvido

Durante longos periodos de trabalho ou trabalho, o usuario pode ficar em um ambiente com:

- pouca iluminacao;
- temperatura alta;
- umidade inadequada;
- falta de acompanhamento do tempo de foco e pausa;
- ausencia de monitoramento remoto do estado da bancada.

O projeto resolve esse problema transformando uma mesa comum em uma estacao conectada que mede o ambiente, toma decisoes locais e permite supervisao remota em tempo real.

## Objetivo do Projeto

Desenvolver uma aplicacao embarcada com `ESP32` capaz de:

- ler `temperatura`, `umidade` e `luminosidade`;
- exibir o estado local em `display de 7 segmentos`;
- receber comandos por `botoes`;
- acionar uma luminaria por `rele`;
- publicar telemetria por `MQTT`;
- mostrar dados e comandos em um dashboard `Node-RED`;
- disponibilizar um `HTTP WebServer` para consulta local.

## Comportamento Funcional

- Em `FOCUS`, o display mostra o tempo restante do ciclo.
- Em `IDLE` ou `PAUSE`, o display alterna entre temperatura e luminosidade.
- Se a luminosidade cair abaixo do limite configurado e o modo estiver em `FOCUS`, a luminaria e ligada automaticamente pelo `rele`.
- O usuario pode atuar por botoes locais ou por comando remoto via `MQTT`.
- O `Node-RED` exibe sensores, modo atual, alerta ambiental e estado da luminaria.

Valores padrao adotados:

- limite inicial de luminosidade: `40%`;
- alerta de temperatura: acima de `30 C`;
- alerta de umidade: abaixo de `30%`;
- ciclo padrao: `25 min` de foco e `5 min` de pausa;
- para demonstracao, os tempos podem ser reduzidos sem mudar a arquitetura.

## Hardware Base

O projeto assume o kit completo ja usado nas aulas:

| Componente | Funcao | GPIO base |
| --- | --- | --- |
| `ESP32` | processamento principal | - |
| `DHT11` | temperatura e umidade | `33` |
| `LDR` | leitura analogica de luminosidade | `39` |
| `Rele` | acionamento da luminaria | `13` |
| `LED 1` | indicacao de foco | `4` |
| `LED 2` | indicacao de pausa | `0` |
| `LED 3` | status Wi-Fi/MQTT | `2` |
| `LED 4` | alerta ambiental | `15` |
| `Botao 1` | iniciar ou pausar ciclo | `4` |
| `Botao 2` | reset do ciclo | `0` |
| `Botao 3` | alternar exibicao no display | `2` |
| `Botao 4` | override manual da luminaria | `15` |
| `Display 7 segmentos` | exibicao local | pinagem consolidada das aulas |

Pinagem do display:

- `SEG_A = 18`
- `SEG_B = 5`
- `SEG_C = 21`
- `SEG_D = 3`
- `SEG_E = 1`
- `SEG_F = 23`
- `SEG_G = 22`
- `SEG_DP = 19`
- `DISPLAY_1 = 16`
- `DISPLAY_2 = 17`

## Arquitetura FreeRTOS

O firmware deve ser organizado com os seguintes blocos minimos:

| Bloco | Responsabilidade |
| --- | --- |
| `TaskSensores` | ler `DHT11` e `LDR`, montar snapshot e encaminhar dados |
| `TaskControle` | ler botoes, alternar modos, aplicar regras locais e override manual |
| `TaskDisplay` | multiplexar o display e escolher o valor exibido |
| `TaskIoT` | manter Wi-Fi, `MQTT`, `HTTP`, publicar telemetria e receber comandos |
| `Queue` | transportar snapshot de sensores e estado entre as tasks |
| `Software Timer` | controlar ciclo de foco e pausa e/ou periodicidade de publicacao |
| `Event Group` | sinalizar `WIFI_OK`, `MQTT_OK`, `FOCUS` e `ALERT` |

Separacao de responsabilidades:

- aquisicao de dados: `TaskSensores`
- logica de negocio: `TaskControle`
- interface local: `TaskDisplay`
- comunicacao externa: `TaskIoT`

## Interfaces Publicas

### Topicos MQTT

| Funcao | Topico |
| --- | --- |
| temperatura | `satc/gX/telemetry/temperature` |
| umidade | `satc/gX/telemetry/humidity` |
| luminosidade | `satc/gX/telemetry/luminosity` |
| modo atual | `satc/gX/status/mode` |
| alerta ambiental | `satc/gX/status/alert` |
| comando da luminaria | `satc/gX/cmd/light` |

`gX` deve ser substituido pelo numero real do grupo.

### Rotas HTTP

| Rota | Funcao |
| --- | --- |
| `/` | resumo humano do sistema |
| `/status` | estado atual para teste rapido |
| `/config` | opcional para ajustes locais simples |

## Fluxo Macro do Sistema

```text
Sensores e botoes
    -> ESP32
    -> Tasks FreeRTOS
    -> Queue / Timer / Event Group
    -> Display / LEDs / Rele
    -> MQTT
    -> Node-RED
    -> MQTT
    -> ESP32
```

Resumo do fluxo operacional:

1. O `ESP32` coleta `temperatura`, `umidade` e `luminosidade`.
2. O `FreeRTOS` distribui leitura, controle, exibicao e comunicacao em tarefas separadas.
3. O sistema publica telemetria via `MQTT`.
4. O `Node-RED` mostra os dados em dashboard e pode enviar comandos remotos.
5. O `ESP32` recebe os comandos, atualiza rele, LEDs e display.
6. O `HTTP WebServer` oferece uma visao local do mesmo estado exibido remotamente.

## Requisitos da Prova Atendidos

### Requisitos IoT

- envio de dados para a nuvem ou broker por `MQTT`;
- dashboard em tempo real com `Node-RED`;
- interface local por `HTTP WebServer`;
- controle remoto da luminaria por `MQTT`.

### Requisitos de Sistemas Embarcados

- uso de `tasks` concorrentes;
- uso de `software timers`;
- uso de `queues`;
- uso de `event groups` como mecanismo de sincronizacao.

### Requisitos de Hardware Minimo

- pelo menos `1` entrada analogica: `LDR`;
- pelo menos `2` entradas digitais: botoes;
- `1` display de `7 segmentos`;
- pelo menos `2` saidas digitais: `rele` e `LEDs`.

### Requisitos de Engenharia

- separacao clara entre leitura, processamento, exibicao e comunicacao;
- arquitetura multiprocessada com `FreeRTOS`;
- integracao coerente com `MQTT`, `Node-RED` e `HTTP`;
- tema conectado a um problema real do cotidiano.

## Estrutura de Documentacao

- [CONCEITO.md](./CONCEITO.md): conceito do projeto, objetivo, problema e justificativa
- [FLUXOGRAMA_HARDWARE.md](./FLUXOGRAMA_HARDWARE.md): fluxo do hardware e dos blocos do sistema
- [DEFINICOES_IOT.md](./DEFINICOES_IOT.md): definicoes essenciais de IoT usadas no projeto

## Cenarios de Validacao

- leitura correta de `temperatura`, `umidade` e `luminosidade`;
- alteracao de modo pelos botoes com reflexo no display;
- acionamento automatico da luminaria quando a luz ambiente cair no modo `FOCUS`;
- acionamento remoto da luminaria pelo `Node-RED`;
- consistencia entre dashboard `MQTT` e pagina `HTTP`;
- presenca explicita de `tasks`, `queue`, `software timer` e `event group`.

## Referencias de Aula

Conteudos mais proximos desta proposta:

- `Embedded-Operating-Systems/AULA07`: filas no FreeRTOS
- `Embedded-Operating-Systems/AULA09`: software timers
- `Embedded-Operating-Systems/AULA10`: event groups e task notifications
- `Internet-of-Things/AULA07`: MQTT com ESP32
- `Internet-of-Things/AULA08`: `MQTT + Node-RED + HTTP WebServer`
