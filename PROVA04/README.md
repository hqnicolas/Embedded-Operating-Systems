# PROVA04 - Estacao de Trabalho Inteligente IoT

## Visao geral

Este projeto define uma **estacao de trabalho inteligente** baseada em `ESP32`, unindo os conteudos de **Sistemas Operacionais Embarcados** e **Internet das Coisas** em uma proposta unica, pratica e proxima da realidade.

A solucao monitora o ambiente de trabalho, acompanha o ciclo de foco, automatiza a iluminacao local e publica os dados para um dashboard remoto. O sistema usa `FreeRTOS` para organizar a execucao concorrente e `MQTT + Node-RED + HTTP WebServer` como pilha oficial de IoT.

## Problema resolvido

Durante longos periodos de estudo ou trabalho, o usuario pode ficar em um ambiente com:

- pouca iluminacao;
- temperatura alta;
- umidade inadequada;
- falta de acompanhamento do tempo de foco e pausa;
- ausencia de monitoramento remoto do estado da bancada.

O projeto resolve esse problema transformando uma mesa comum em uma estacao conectada que mede o ambiente, toma decisoes locais e permite supervisao remota em tempo real.

## Objetivo do projeto

Desenvolver uma aplicacao embarcada com `ESP32` capaz de:

- ler `temperatura`, `umidade` e `luminosidade`;
- exibir o estado local em `display de 7 segmentos`;
- receber comandos por `botoes`;
- acionar uma luminaria por `rele`;
- publicar telemetria por `MQTT`;
- mostrar dados e comandos em um dashboard `Node-RED`;
- disponibilizar um `HTTP WebServer` para consulta local.

## Comportamento funcional

- Em `FOCO`, o display mostra o tempo restante do ciclo.
- Em `OCIOSO` ou `PAUSA`, o display mostra sensores de acordo com o modo de exibicao.
- Se a luminosidade cair abaixo do limite configurado e o modo estiver em `FOCO`, a luminaria e ligada automaticamente pelo `rele`.
- O usuario pode atuar por botoes locais ou por comando remoto via `MQTT`.
- O `Node-RED` exibe sensores, foco, alerta e estado da luminaria.

Valores padrao adotados:

- limite inicial de luminosidade: `40%`;
- alerta de temperatura: acima de `30 C`;
- alerta de umidade: abaixo de `30%`;
- ciclo padrao: `25 s` de foco e `5 s` de pausa;
- para demonstracao, os tempos podem ser alterados sem mudar a arquitetura.

## Hardware base

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
| `Botao 4` | controle manual da luminaria | `15` |
| `Display 7 segmentos` | exibicao local | pinagem consolidada das aulas |

## Arquitetura FreeRTOS

O firmware e organizado com os seguintes blocos:

| Bloco | Responsabilidade |
| --- | --- |
| `tarefaSensores` | ler `DHT11` e `LDR`, montar `LeituraSensores` e encaminhar dados |
| `tarefaControle` | ler botoes, alternar modos, aplicar regras locais e controle manual |
| `tarefaDisplay` | multiplexar o display e escolher o valor exibido |
| `tarefaIoT` | manter Wi-Fi, `MQTT`, `HTTP`, publicar telemetria e receber comandos |
| `Queue` | transportar leituras e comandos entre as tarefas |
| `Software Timer` | controlar amostragem, ciclo e periodicidade de publicacao |
| `Event Group` | sinalizar `BIT_WIFI_OK`, `BIT_MQTT_OK`, `BIT_FOCO_ATIVO` e `BIT_ALERTA_ATIVO` |

Separacao de responsabilidades:

- aquisicao de dados: `tarefaSensores`
- logica de negocio: `tarefaControle`
- interface local: `tarefaDisplay`
- comunicacao externa: `tarefaIoT`

## Interfaces publicas

### Topicos MQTT

| Funcao | Topico |
| --- | --- |
| temperatura | `satc/gX/telemetria/temperatura` |
| umidade | `satc/gX/telemetria/umidade` |
| luminosidade | `satc/gX/telemetria/luminosidade` |
| estado de foco | `satc/gX/estado/foco` |
| alerta ambiental | `satc/gX/estado/alerta` |
| comando da luminaria | `satc/gX/comando/luz` |

`gX` deve ser substituido pelo numero real do grupo.

### Rotas HTTP

| Rota | Funcao |
| --- | --- |
| `/` | resumo humano do sistema |
| `/estado` | estado atual em JSON |
| `/configuracao` | ajustes locais simples |

Exemplo:

```text
/configuracao?luminosidade=45&umidade=35&foco=30&pausa=10
```

### Payloads de comando

- `LIGAR`
- `DESLIGAR`

## Conectividade Wi-Fi

Na inicializacao e em toda reconexao, o firmware segue esta ordem:

1. tenta a rede comum `Nicolas`;
2. se falhar, tenta a rede enterprise `SATC 2.4`;
3. se as duas falharem, aguarda o intervalo configurado e reinicia o ciclo pela rede primaria.

## Fluxo macro do sistema

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

## Requisitos da prova atendidos

### Requisitos IoT

- envio de dados para broker por `MQTT`;
- dashboard em tempo real com `Node-RED`;
- interface local por `HTTP WebServer`;
- controle remoto da luminaria por `MQTT`.

### Requisitos de sistemas embarcados

- uso de `tasks` concorrentes;
- uso de `software timers`;
- uso de `queues`;
- uso de `event groups` como mecanismo de sincronizacao.

### Requisitos de hardware minimo

- pelo menos `1` entrada analogica: `LDR`;
- pelo menos `2` entradas digitais: botoes;
- `1` display de `7 segmentos`;
- pelo menos `2` saidas digitais: `rele` e `LEDs`.

## Estrutura de documentacao

- [CONCEITO.md](./CONCEITO.md): conceito do projeto, objetivo, problema e justificativa
- [FLUXOGRAMA_HARDWARE.md](./FLUXOGRAMA_HARDWARE.md): fluxo do hardware e dos blocos do sistema
- [DEFINICOES_IOT.md](./DEFINICOES_IOT.md): definicoes essenciais de IoT usadas no projeto
- [ESP32/README.md](./ESP32/README.md): guia tecnico do firmware e da validacao
