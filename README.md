# Sistemas Operacionais Embarcados

Repository contendo exercícios e atividades práticas com microcontroladores ESP32/Arduino, abordando conceitos de programação embarcada, I/O digital/analógico, e desenvolvimento orientado a objetos.

---

## Conteúdo

### AULA01 - Atividades com ESP32 / Arduino
- [Exercício01](./AULA01/Exercício01) – Controle de LED RGB
- [Exercício02](./AULA01/Exercício02) – Contador com Display de 7 Segmentos

### AULA02 - IoT Aplicada: Classes, Objetos e Bibliotecas
- [Exercício01](./AULA02/Exercício01) – Entradas Digitais e Filtro Bounce
- [Exercício02](./AULA02/Exercício02) – Classe Sinalizador: Controle de LED com biblioteca customizada

### AULA03 - Saída Analógica
- [saidaAnalogica.ino](./AULA03/saidaAnalogica.ino) – Exemplos de saída PWM e modulação analógica

### AULA04 - Timers e Interrupções
- [Exercício01](./AULA04/Exercício01) – Entrada Digital com Debouncing
- [Exercício02](./AULA04/Exercício02) – Exemplo de Timer 2
- [Exercício03](./AULA04/Exercício03) – Exemplo de Timer
- [Material de Apoio](./AULA04/MATERIAL_DE_APOIO.md)

### AULA05 - FreeRTOS no ESP32
- [Exemplo1](./AULA05/Exemplo1) – Criação de tasks no FreeRTOS
- [Exemplo2](./AULA05/Exemplo2) – Execução de múltiplas tasks com controle de LED
- [Exemplo3](./AULA05/Exemplo3) – Controle de tasks com leitura analógica e prioridade
- [Exercicio01](./AULA05/Exercicio01) – Leitura analógica e controle de tasks com suspensão/retomada
- [Exercicio02](./AULA05/Exercicio02) – Controle de tarefas com lógica condicional e múltiplas saídas

### AULA06 - Comunicação entre Tasks
- [README](./AULA06/README.md) – Passagem de parâmetros para tasks usando ponteiros (`void *`) e `xTaskCreate`
- [Exemplos](./AULA06/Exemplos) – Exemplos práticos de leitura analógica e passagem de parâmetros

### AULA07 - Filas no FreeRTOS
- [README](./AULA07/README.md) – Introdução e funções de manipulação de filas
- [Exemplo1](./AULA07/Exemplo1) – Exemplo básico de uso de filas
- [Exemplo2](./AULA07/Exemplo2) – Comunicação entre tarefas com fila
- [Exemplo3](./AULA07/Exemplo3) – Exemplo adicional de uso de filas com FreeRTOS
- [Exercicio01](./AULA07/Exercicio01) – Comunicação entre tarefas com fila
- [Exercicio02](./AULA07/Exercicio02) – Display de 7 segmentos com multiplexação e fila

### AULA08 - Semáforos no FreeRTOS
- [README](./AULA08/README.md) – Introdução aos semáforos, mutex e semáforo contador
- [Exemplo1](./AULA08/Exemplo1) – Exemplo com mutex
- [Exemplo2](./AULA08/Exemplo2) – Exemplo com semáforo binário
- [Exemplo3](./AULA08/Exemplo3) – Exemplo com semáforo contador
- [Exercício01](./AULA08/Exercício01) – Leitura do ultrassom acionada por botão com semáforo

### AULA09 - Software Timers no FreeRTOS
- [README](./AULA09/README.md) – Introdução a software timers, daemon task e funções de controle
- [Exemplo1](./AULA09/Exemplo1) – Timer one-shot e auto reload com LEDs e botão
- [Exemplo2](./AULA09/Exemplo2) – Exemplo prático adicional com software timer
- [Atividade1](./AULA09/Atividade1) – Sequência de LEDs usando timer
- [Atividade2](./AULA09/Atividade2) – Sistema de bomba com temporização

### AULA10 - Event Groups e Task Notifications
- [README](./AULA10/README.md) – Conceitos de event groups, flags e task notifications
- [Exemplo1](./AULA10/Exemplo1) – Exemplo com task notifications
- [Exemplo2](./AULA10/Exemplo2) – Exemplo com event groups

### PROVA01 - Avaliações com FreeRTOS
- [QUEST1](./PROVA01/QUEST1) – Controle de entrada analógica e leitura de sensores
- [QUEST2](./PROVA01/QUEST2) – Sistema de alarme com sensores e atuadores

### PROVA02 - Controle de Luminosidade com ESP32
- [README](./PROVA02/README.md) – Descrição do projeto com LDR, setpoint e controle de relé

### PROVA03 - Avaliação FreeRTOS
- [PROVA03.md](./PROVA03/PROVA03.md) – Enunciado e requisitos da prova
- [PROVA03.ino](./PROVA03/PROVA03.ino) – Implementação da solução em ESP32

### PROVA04 - Estação de Trabalho Inteligente IoT
- [README](./PROVA04/README.md) – Visão geral do projeto integrado com IoT
- [CONCEITO.md](./PROVA04/CONCEITO.md) – Conceito e justificativa do projeto
- [FLUXOGRAMA_HARDWARE.md](./PROVA04/FLUXOGRAMA_HARDWARE.md) – Fluxo do hardware e dos blocos do sistema
- [DEFINICOES_IOT.md](./PROVA04/DEFINICOES_IOT.md) – Definições essenciais de IoT
- [ESP32](./PROVA04/ESP32) – Firmware principal em ESP32
- [IMG](./PROVA04/IMG) – Imagens de apoio e documentação visual

---
