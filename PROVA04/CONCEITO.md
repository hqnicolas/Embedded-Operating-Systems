# Conceito - Estacao de Trabalho Inteligente IoT

## Visao do Projeto

A **estacao de trabalho inteligente** e um sistema embarcado conectado que melhora as condicoes de trabalho ou trabalho em uma mesa individual. O projeto combina monitoramento ambiental, automacao local e supervisao remota em tempo real.

Em vez de apenas ler sensores, a proposta organiza o sistema como uma pequena solucao IoT completa:

- mede o ambiente;
- processa os dados localmente;
- apresenta informacoes no proprio hardware;
- publica telemetria para a rede;
- recebe comandos remotos;
- automatiza a iluminacao de apoio.

## Problema

Mesas de trabalho comuns normalmente nao acompanham o estado do ambiente nem ajudam o usuario a manter um ciclo de foco organizado. Isso gera alguns problemas simples, mas reais:

- iluminacao insuficiente durante o trabalho;
- desconforto termico sem percepcao imediata;
- baixa umidade do ar;
- falta de controle visual sobre o tempo de foco e pausa;
- ausencia de acompanhamento remoto do estado da bancada.

## Objetivo

Criar um sistema com `ESP32` que monitore o ambiente da mesa, indique estados localmente e permita integracao com servicos IoT para visualizacao e controle remoto.

## Como o Projeto Funciona

### Coleta de dados

O `ESP32` realiza a leitura de:

- `temperatura`;
- `umidade`;
- `luminosidade`.

Esses dados representam as principais condicoes do ambiente de trabalho.

### Processamento local

Com base nas leituras e nos botoes, o sistema decide:

- se o modo atual esta em `FOCUS`, `PAUSE` ou `IDLE`;
- se ha condicao de alerta ambiental;
- se a luminaria deve ser ligada automaticamente;
- o que deve ser mostrado no display.

### Interface local

O usuario acompanha o funcionamento por:

- `display de 7 segmentos`;
- `LEDs de status`;
- `botoes` de controle;
- acionamento fisico da luminaria por `rele`.

### Interface remota

Os dados sao enviados por `MQTT` para o `Node-RED`, que funciona como dashboard e camada de automacao. O sistema tambem expoe um `HTTP WebServer` para consulta local do estado atual.

## Justificativa Tecnica

Este tema foi escolhido porque aproveita muito bem o conteudo das duas disciplinas.

### Relacao com Sistemas Operacionais Embarcados

O projeto exige:

- multitarefa com `FreeRTOS`;
- comunicacao entre tarefas com `Queue`;
- temporizacao com `Software Timer`;
- sincronizacao com `Event Group`.

Isso permite separar responsabilidades de forma clara entre leitura, controle, exibicao e comunicacao.

### Relacao com IoT

O projeto exige:

- conexao de rede com `ESP32`;
- telemetria por `MQTT`;
- dashboard em `Node-RED`;
- monitoramento por `HTTP`;
- controle remoto da luminaria.

Ou seja, a proposta nao fica restrita ao hardware local: ela completa o ciclo de uma aplicacao IoT real.

## Por que este Projeto e Bom para a PROVA04

- usa um tema coerente com o cotidiano;
- cumpre o hardware minimo exigido;
- reaproveita a pinagem e os modulos ja vistos em aula;
- facilita demonstracao pratica;
- mostra integracao clara entre embarcados e IoT;
- permite explicar bem a arquitetura durante a apresentacao.

## Resultado Esperado

Ao final, a dupla deve apresentar uma mesa de trabalho conectada capaz de:

- monitorar o ambiente em tempo real;
- orientar o usuario durante o ciclo de foco;
- acionar a luminaria automaticamente quando necessario;
- receber comandos remotos;
- mostrar o mesmo estado localmente e no dashboard.
