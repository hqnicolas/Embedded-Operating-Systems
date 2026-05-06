# Event Groups e Task Notifications

## Grupo de eventos
* **Event group** é outro recurso do FreeRTOS que permite que os eventos sejam comunicados às tarefas.
* Ao contrário dos semáforos e filas:
    * Permite que uma tarefa aguarde no estado bloqueada até que ocorra uma combinação de um ou mais eventos;
    * Pode desbloquear todas as tarefas que estavam aguardando o mesmo evento ou combinação de eventos quando o evento ocorre.

---

## Events groups
* São úteis para sincronizar várias tarefas;
* São usadas para que uma ação aconteça após um conjunto de eventos definidos;
* Reduzem o consumo de memória RAM, pois muitas vezes é possível substituir muitos semáforos binários por um único grupo de eventos;
* É opcional no FreeRTOS: `event_groups.c`

---

## Trabalhando com flags
* Neste caso, cada bit vai indicar uma ação de uma tarefa a ser executada.
* O status do evento é indicado em cada bit.

**Status flags (exemplo de bits):**
* **CF:** Carry flag (Bit 0)
* **PF:** Parity flag (Bit 2)
* **AF:** Auxiliary carry flag (Bit 4)
* **ZF:** Zero flag (Bit 6)
* **SF:** Sign flag (Bit 7)
* **TF:** Trap flag (Bit 8)
* **IF:** Interrupt enable flag (Bit 9)
* **DF:** Direction flag (Bit 10)
* **OF:** Overflow flag (Bit 11)

---

## Event groups, events flags e event bits
Os bits de evento são armazenados em uma variável do tipo `EventBits_t`.
* **Bit 0** é a Flag 0.
* **Bit 23** é a Flag 23 (em sistemas de 32 bits).

---

## Quantidade de eventos
O número de bits de evento em um grupo de eventos depende da configuração do `configUSE_16_BIT_TICKS`:
* Se `configUSE_16_BIT_TICKS` for 1, cada grupo de eventos conterá 8 bits de evento utilizáveis.
* Se `configUSE_16_BIT_TICKS` for 0, cada grupo de eventos conterá 24 bits de evento utilizáveis.

---

## Timer service task
* Iniciada automaticamente junto ao scheduler, desde que a funcionalidade de software timer esteja habilitada no FreeRTOS;
* Responsável por receber e executar comandos sobre timers e também executar a função de callback;
* Funciona igual a uma tarefa do RTOS.

---

## Criando um grupo de eventos
```c
EventGroupHandle_t xEventGroupCreate( void );

/* Declaração de variável para o grupo de eventos */
EventGroupHandle_t xCreatedEventGroup;

/* Tentativa de criação do grupo */
xCreatedEventGroup = xEventGroupCreate();

/* Verificação de sucesso */
if( xCreatedEventGroup == NULL ) {
    /* Falha por memória insuficiente no heap */
} else {
    /* Grupo de eventos criado com sucesso */
}
```

---

## Setando um bit
```c
EventBits_t xEventGroupSetBits(
    EventGroupHandle_t xEventGroup, 
    const EventBits_t uxBitsToSet
);
```

---

## Aguardando eventos
```c
EventBits_t xEventGroupWaitBits(
    const EventGroupHandle_t xEventGroup,
    const EventBits_t uxBitsToWaitFor,
    const BaseType_t xClearOnExit,
    const BaseType_t xWaitForAllBits,
    TickType_t xTicksToWait 
);
```
* **xEventGroup:** qual o grupo de eventos;
* **uxBitsToWaitFor:** quais bits esperar;
* **xClearOnExit:** se deseja limpar os bits ao sair;
* **xWaitForAllBits:** esperar por todos os bits (AND) ou qualquer um (OR);
* **xTicksToWait:** tempo máximo para esperar os bits (timeout).

---

## Aguardando eventos - Exemplo
```c
EventBits_t uxBits;
const TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS;

/* Espera no máximo 100ms para que o bit 0 ou o bit 4 sejam setados.
   Limpa os bits antes de retornar. */
uxBits = xEventGroupWaitBits(
            xEventGroup,    /* Grupo sendo testado */
            BIT_0 | BIT_4,  /* Bits a esperar */
            pdTRUE,         /* Limpar bits ao sair */
            pdFALSE,        /* Não esperar por ambos, qualquer um serve */
            xTicksToWait ); /* Timeout de 100ms */
```

---

## Deletando um grupo de eventos
```c
void vEventGroupDelete( EventGroupHandle_t xEventGroup );
```

---

## Task notifications
* Disponível no FreeRTOS 8.2.0;
* Cada tarefa do RTOS possui uma variável de notificação de 32 bits;
* Uma task notification no RTOS é um evento enviado diretamente a uma tarefa, podendo desbloqueá-la;
* Desbloquear uma tarefa RTOS com uma notificação direta é 45% mais rápido e usa menos RAM do que desbloquear uma tarefa com um semáforo binário;
* As task notifications podem ser usadas como: semáforos binários, semáforos contadores e grupos de eventos.

---

## Parando o timer
```c
/*timers.h*/
BaseType_t xTimerStop( TimerHandle_t xTimer, TickType_t xBlockTime );

/* Exemplo */
xTimerStop( xTimer, 0 );
```

---

## Comunicação entre tasks
Filas e semáforos necessitam de um objeto intermediário para comunicar entre as tasks.

## Comunicação com task notifications
Com task notifications, a comunicação é direta entre as tarefas.

---

## Benefícios
* Mais rápido que o uso de fila, semáforo ou grupo de eventos para executar operações equivalentes;
* Menor consumo de memória RAM.

## Limitações
* Não é possível enviar uma notificação para uma ISR;
* Não é possível enviar notificações para mais de uma tarefa simultaneamente;
* Necessário aguardar o tratamento da última notificação para o próximo envio.
