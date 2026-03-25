Esta é a transcrição completa do documento "Introdução ao FreeRTOS" para o formato Markdown, com espaços reservados para a inclusão das imagens originais.

---

# Introdução aos Sistemas de Tempo Real
**Disciplina:** IoT Aplicada  
**Prof. Me. Cleber Lourenço Izidoro**

---

## Sistemas Embarcados
* Desenvolvidos para aplicações específicas.
* Recursos físicos e computacionais restritos.
* Deve ser pensado em segurança, baixo consumo, etc.
* **Exemplos:**
    * Lavadoras de roupas.
    * Robôs industriais.
    * Drones.
    * Rastreadores.

> **[IMAGEM AQUI: Diagrama de blocos de um Sistema Embarcado - Entradas Digitais/Analógicas, Web Server, Periféricos e CPU ARM7]**

---

## Programação de Sistemas Embarcados
Existem diferentes abordagens para o desenvolvimento:
* **Super Loop**
* **Máquinas de Estado**
* **Kernel - RTOS**

### Abordagem Super Loop
> **[IMAGEM AQUI: Diagrama de tempo de um Superloop com Tasks e ISR (Interrupt Service Routines)]**

* **Vantagens:**
    * Fácil de desenvolver.
    * Simples e eficiente.
    * Ótimo para microcontroladores pequenos.
    * Não requer recursos adicionais para processamento.
* **Desvantagens:**
    * Não garante as restrições de tempo.
    * Uma função ou interrupção influencia no tempo das tarefas.
    * Difícil manutenção.
    * Difícil implementar novas funcionalidades na aplicação.

---

## Sistemas Operacionais RTOS
Oferecem:
* Uma camada de abstração do Hardware.
* Gerenciamento de Memória.
* Gerenciamento de Processos.
* Intermediação da comunicação entre periféricos e processos.
* Portabilidade de código.

**Definição:** Um RTOS é um sistema operacional cujos processos internos garantem a conformidade com os requisitos de tempo.

### Características Fundamentais:
* **Previsibilidade:** Qualidade de ser previsível no comportamento do agendamento das tarefas.
* **Determinismo:** Qualidade de poder produzir consistentemente os mesmos resultados sob as mesmas condições.

---

## Por que usar um RTOS?
> **[IMAGEM AQUI: Comparação entre non-OS, Bare-bones RTOS e RTOS with Tasking]**

* Capacidade de manutenção/extensão.
* Abstração de informações de tempo.
* Modularidade e desenvolvimento em equipes.
* Reutilização de código e facilidade de testes.
* Eficiência aumentada: aproveitamento do tempo ocioso do MCU e gerenciamento de energia.
* Tratamento de interrupção flexível e controle mais fácil dos periféricos.

---

## Aplicações Práticas do RTOS
Indicado para efetuar muitas tarefas ao mesmo tempo ou quando falhas críticas podem ocorrer caso o sistema atrase além do tempo definido.

> **[IMAGEM AQUI: Gráfico de execução de múltiplas tarefas (Task 1, 2, 3) parecendo ocorrer simultaneamente]**

Em Bare Metal (sem SO), a sincronização para que uma tarefa não "trave" a outra pode ser extremamente complexa.

> **[IMAGEM AQUI: Diagrama de Context Switch - Apenas uma tarefa executando por vez no processador]**

> **[IMAGEM AQUI: Comparação visual entre a estrutura Bare Metal e RTOS com a IoT Application]**

---

## Principais Itens de um SO
* **Kernel:** Núcleo do sistema, abstração entre software e hardware; gerencia memória RAM e processos.
* **Scheduler:** Algoritmo que decide quais tarefas serão executadas, baseando-se em prioridades ou divisão de tempo.
* **Tarefas:** "Mini programas" que dividem o código em partes com diferentes importâncias.

---

## Scheduler (Agendador)
Responsável por administrar quais tarefas obtêm o uso da CPU. Algoritmos comuns incluem: RR (Round Robin), SJF (Shortest Job First) e SRT (Shortest Remaining Time).

> **[IMAGEM AQUI: Diagrama do Scheduler gerenciando a fila de tarefas para a CPU]**

---

## Formas de Trabalho do Scheduler

### 1. Preemptivo
Permite que uma tarefa em execução seja interrompida antes do fim para forçar a troca de contexto (ex: por prioridade ou Time Slicing).

> **[IMAGEM AQUI: Diagrama de Context Switch - Salvando informações da Task 1 e carregando Task 2]**

> **[IMAGEM AQUI: Padrão de execução com preempção por prioridade (High, Med, Low priority tasks)]**

*Durante a troca de tarefa, é preciso salvar os registradores da tarefa atual.*

### 2. Cooperativo
As tarefas não podem ser interrompidas; elas devem cooperar e ceder o uso da CPU voluntariamente após terminarem sua parte ou forçarem a troca de contexto.

> **[IMAGEM AQUI: Padrão de execução do Scheduler Cooperativo]**

---

## Algoritmos de Escalonamento

### Round Robin (RR)
Comum em sistemas multitarefa, foca na equidade.
* **Funcionamento:** Cada tarefa recebe uma fatia de tempo fixa (**Quantum**). Ao acabar, vai para o fim da fila.
* **Preemptivo:** Sim.
* **Vantagem:** Evita a "inanição" (*starvation*).
* **Desvantagem:** Trocas de contexto frequentes podem gastar muito tempo de processamento se o quantum for curto.

### Shortest Job First (SJF)
Prioriza processos que terminam rápido.
* **Funcionamento:** Escolhe a tarefa com o menor tempo total estimado de execução.
* **Preemptivo:** Não (forma clássica).
* **Desvantagem:** Pode causar *starvation* de tarefas longas.

### Shortest Remaining Time (SRT)
Versão preemptiva do SJF (ou SRTF).
* **Funcionamento:** Avalia constantemente se uma nova tarefa que chegou é mais curta que a atual. Se for, ela a expulsa.
* **Vantagem:** Excelente para processos curtos e críticos esporádicos.

---

## O FreeRTOS
Kernel de tempo real para dispositivos embarcados portado para mais de 35 plataformas. Distribuído sob a **Licença MIT**.

### Características:
* Pequeno o suficiente para microcontroladores.
* Kernel preemptivo ou colaborativo.
* Temporizadores de software eficientes.
* Escrito em C, facilmente portável.
* Possui API fácil de usar e é *Royalty Free*.

### Amazon FreeRTOS (a:FreeRTOS)
Extensão com bibliotecas para IoT, permitindo conexão segura com os serviços de nuvem da AWS.

---

## Estados das Tarefas
As tarefas sempre estarão em um dos quatro estados abaixo:

> **[IMAGEM AQUI: Ciclo de vida das tarefas - Pronta, Execução, Bloqueada e Suspensa]**

1. **Bloqueada (Blocked):** Esperando um evento temporal (*Timeout*) ou sincronização (*Sync*). Delays no RTOS não travam o MCU, apenas bloqueiam a tarefa solicitante.
2. **Suspensa (Suspended):** Solicitada explicitamente pelas funções `vTaskSuspend()` e `vTaskResume()`.
3. **Execução (Running):** Atualmente alocada na CPU. O ESP32 pode ter até duas em execução simultânea (Dual Core).
4. **Pronta (Ready):** A tarefa está apta a executar, mas espera que o scheduler a escolha (depende da prioridade).

---

## Convenções e Guia de Estilo
* **Variáveis:** unsigned (`u`), char (`c`), short (`s`), long (`l`), ponteiros (`p`).
* **Funções:** Privadas (`prv`), API prefixada com o tipo de retorno (ex: `v` para void) e nome do arquivo de origem (ex: `vTaskDelete` de `tasks.c`).
* **Estilo:** Tab = 4 espaços; comentários até coluna 80; sem comentários estilo C++ (`//`).

---

## Erros Comuns (Versão Arduino)
O LED interno (`LED_BUILTIN`) é usado para diagnósticos:
* **Stack Overflow:** LED pisca lentamente. Pilha da tarefa excedida.
* **Falha de Heap:** LED pisca rapidamente. Falha na alocação dinâmica de memória.

---

## FreeRTOS API - Principais Funções
Documentação oficial: https://www.freertos.org

| Categoria | Funções |
| :--- | :--- |
| **Controle do Kernel** | `vTaskStartScheduler()`, `vTaskEndScheduler()`, `vTaskSuspendAll()`, `xTaskResumeAll()` |
| **Criação/Deleção** | `xTaskCreate()`, `vTaskDelete()` |
| **Controle de Task** | `vTaskDelay()`, `vTaskDelayUntil()`, `vTaskPrioritySet()`, `vTaskSuspend()`, `vTaskResume()` |
| **Filas (Queues)** | `xQueueCreate()`, `xQueueSend()`, `xQueueReceive()`, `xQueuePeek()` |
| **Semáforos** | `vSemaphoreCreateBinary()`, `xSemaphoreCreateMutex()`, `xSemaphoreTake()`, `xSemaphoreGive()` |

---

**Fim do Documento**