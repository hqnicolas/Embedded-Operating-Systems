## O que são Software Timers?
* Os software timers são usados para agendar a execução de uma função em um tempo definido no futuro ou periodicamente em uma frequência fixa. 
* A função executada pelo terminal é chamada de função *call-back*. 
* Quando implementados, estão sob o controle do Kernel do FreeRTOS. 
* Não precisam de suporte de hardware e não estão relacionados a temporizadores ou contadores de hardware físicos. 
* É um recurso opcional do FreeRTOS. 

---

## Configuração Básica
* Para habilitar a funcionalidade é necessário incluir o arquivo `timers.c`. 
* É preciso configurar o timer no arquivo `FreeRTOSConfig.h`. 
* **Passo A:** Habilitar a constante `configUSE_TIMERS`. 
* **Passo B:** Configurar a prioridade do timer em `configTIMER_TASK_PRIORITY`. 
* **Passo C:** Configurar o tamanho da fila em `configTIMER_QUEUE_LENGTH`. 
* **Passo D:** Configurar o tamanho da *Stack* (pilha) do timer em `configTIMER_TASK_STACK_DEPTH`. 

---

## Características de Execução
* Assim como o *hardware timer*, o *software timer* não utiliza nenhum processamento da CPU enquanto está apenas ativo (contando). 
* O *software timer* não usa o *tick* do hardware e não executa as funções de *call-back* em contexto de interrupção. 
* Seu funcionamento pode ser comparado ao de uma tarefa comum do RTOS. 
* Todos os comandos dos timers são enviados à tarefa "Timer Service" (ou *Daemon Task*) por meio de uma fila (*queue*). 

---

## Vantagens
* Não precisa de suporte de hardware, sendo sua gestão de responsabilidade exclusiva do FreeRTOS. 
* Podem ser criados diversos timers, limitados apenas pela quantidade de memória disponível no sistema. 
* Apresentam fácil implementação no código. 

## Desvantagens
* A latência varia de acordo com a prioridade da tarefa "Timer Service" e a frequência de *tick* do FreeRTOS. 
* O sistema pode perder comandos se usado excessivamente durante um curto período de tempo, já que a comunicação com a tarefa "Timer Service" é feita por uma fila com tamanho limitado. 
* Possui uma limitação de frequência na faixa de 1kHz. 

---

## Tipos de Configuração
* **One-shot:** Executa o timer apenas uma vez. Se o usuário quiser uma nova execução, ela deve ser reiniciada por software. 
* **Auto reload:** O timer é reiniciado automaticamente e executa infinitamente. 

---

## Timer Service Task (Daemon Task)
* É iniciada automaticamente junto ao *scheduler* (escalonador), desde que a funcionalidade de *software timer* esteja habilitada no FreeRTOS. 
* É responsável por receber e executar os comandos sobre os timers e também por invocar a função de *call-back*. 
* Funciona de maneira idêntica a uma tarefa padrão do RTOS. 

---

## Funcionamento e API

<img width="951" height="531" alt="image" src="https://github.com/user-attachments/assets/5df17763-f527-4c39-af7a-dc9d0faa57dc" />


* O código da aplicação escreve comandos na fila do timer (*Timer command queue*) através de chamadas de API, como a função `xTimerReset()`. 
* A tarefa *Daemon* do FreeRTOS aguarda comandos e faz a leitura dessa fila usando funções como `xQueueReceive()`, processando-os em seguida. 
* O padrão de execução e bloqueio das tarefas depende se a prioridade da tarefa que chama a API (ex: `xTimerStart()`) está acima ou abaixo da prioridade da *Daemon task*. 

<img width="658" height="467" alt="image" src="https://github.com/user-attachments/assets/d11f267c-53ef-493f-bc76-ab606bc6fb77" />


---

## Funções Principais de Controle

**Iniciando o timer** 
```c
// Assinatura em timers.h
BaseType_t xTimerStart( TimerHandle_t xTimer, TickType_t xBlockTime );

// Exemplo de uso
xTimerStart( xTimers, 0 );
```

**Parando o timer** 
```c
// Assinatura em timers.h
BaseType_t xTimerStop( TimerHandle_t xTimer, TickType_t xBlockTime );

// Exemplo de uso
xTimerStop( xTimer, 0 );
```

**Deletando o timer** 
```c
// Assinatura em timers.h
BaseType_t xTimerDelete( TimerHandle_t xTimer, TickType_t xBlockTime );

// Exemplo de uso
xTimerDelete( xTimer, 0 );
```
