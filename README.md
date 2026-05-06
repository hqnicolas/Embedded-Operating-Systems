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
- [Exercicio01](./AULA05/Exercicio01) – Leitura analógica e controle de tasks com suspensão/retomada

### AULA06 - Comunicação entre Tasks
- [README](./AULA06/README.md) – Passagem de parâmetros para tasks usando ponteiros (`void *`) e `xTaskCreate`

### AULA07 - Filas no FreeRTOS
- [README](./AULA07/README.md) – Introdução e funções de manipulação de filas
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

---