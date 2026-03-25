## Sistemas Embarcados
* Desenvolvidos para aplicações específicas.
* Recursos físicos e computacionais restritos.
* Deve ser pensado em segurança, baixo consumo, etc.
* **Exemplos:**
    * Lavadoras de roupas.
    * Robôs industriais.
    * Drones.
    * Rastreadores.

<img width="1098" height="437" alt="image" src="https://github.com/user-attachments/assets/d189a566-b72b-4372-ad8c-0b8262c99412" />


---

## Programação de Sistemas Embarcados
Existem diferentes abordagens para o desenvolvimento:
* **Super Loop**
* **Máquinas de Estado**
* **Kernel - RTOS**

### Abordagem Super Loop
<img width="578" height="506" alt="image" src="https://github.com/user-attachments/assets/ecb35b6e-5540-4c77-a4a5-fb77c12c7d4f" />


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
<img width="703" height="332" alt="image" src="https://github.com/user-attachments/assets/232f758a-42a4-43b7-bed1-601b16f00050" />


* Capacidade de manutenção/extensão.
* Abstração de informações de tempo.
* Modularidade e desenvolvimento em equipes.
* Reutilização de código e facilidade de testes.
* Eficiência aumentada: aproveitamento do tempo ocioso do MCU e gerenciamento de energia.
* Tratamento de interrupção flexível e controle mais fácil dos periféricos.

---

## Aplicações Práticas do RTOS
Indicado para efetuar muitas tarefas ao mesmo tempo ou quando falhas críticas podem ocorrer caso o sistema atrase além do tempo definido.

<img width="1214" height="428" alt="image" src="https://github.com/user-attachments/assets/ee600008-bd35-493a-8433-fab62504af14" />


Em Bare Metal (sem SO), a sincronização para que uma tarefa não "trave" a outra pode ser extremamente complexa.

---

## Principais Itens de um SO
* **Kernel:** Núcleo do sistema, abstração entre software e hardware; gerencia memória RAM e processos.
* **Scheduler:** Algoritmo que decide quais tarefas serão executadas, baseando-se em prioridades ou divisão de tempo.
* **Tarefas:** "Mini programas" que dividem o código em partes com diferentes importâncias.

---

## Scheduler (Agendador)
Responsável por administrar quais tarefas obtêm o uso da CPU. Algoritmos comuns incluem: RR (Round Robin), SJF (Shortest Job First) e SRT (Shortest Remaining Time).

<img width="527" height="329" alt="image" src="https://github.com/user-attachments/assets/3d174c78-5411-4f45-9ea4-f8b4a601914c" />


---

## Formas de Trabalho do Scheduler

### 1. Preemptivo
Permite que uma tarefa em execução seja interrompida antes do fim para forçar a troca de contexto (ex: por prioridade ou Time Slicing).

<img width="644" height="391" alt="image" src="https://github.com/user-attachments/assets/274738dc-58ed-489a-8539-08470af6e07e" />


<img width="530" height="426" alt="image" src="https://github.com/user-attachments/assets/bca7ac75-373a-40db-a0eb-67837224b436" />


*Durante a troca de tarefa, é preciso salvar os registradores da tarefa atual.*

### 2. Cooperativo
As tarefas não podem ser interrompidas; elas devem cooperar e ceder o uso da CPU voluntariamente após terminarem sua parte ou forçarem a troca de contexto.

<img width="735" height="347" alt="image" src="https://github.com/user-attachments/assets/00f784c6-ddf8-4df8-91e3-89e656266624" />

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

<img width="1143" height="331" alt="image" src="https://github.com/user-attachments/assets/e70f0b55-263a-4055-9a0d-79acea73fa4b" />


---

## Estados das Tarefas
As tarefas sempre estarão em um dos quatro estados abaixo:

<img width="1138" height="120" alt="image" src="https://github.com/user-attachments/assets/16f5e3f6-b55d-4304-b2a7-101eecdd011e" />


1. **Bloqueada (Blocked):** Esperando um evento temporal (*Timeout*) ou sincronização (*Sync*). Delays no RTOS não travam o MCU, apenas bloqueiam a tarefa solicitante.
2. **Suspensa (Suspended):** Solicitada explicitamente pelas funções `vTaskSuspend()` e `vTaskResume()`.
3. **Execução (Running):** Atualmente alocada na CPU. O ESP32 pode ter até duas em execução simultânea (Dual Core).
4. **Pronta (Ready):** A tarefa está apta a executar, mas espera que o scheduler a escolha (depende da prioridade).

<img width="329" height="426" alt="image" src="https://github.com/user-attachments/assets/6ffeaa13-8fd5-483f-ba0e-a4d04740e69a" />


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
