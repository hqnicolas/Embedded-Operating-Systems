# Filas no FreeRTOS

## 1. Comunicação entre Tarefas
Em sistemas multitarefa como o FreeRTOS, é essencial haver uma forma de comunicação para a troca de dados entre as tarefas.

### Abordagem com Variável Global
* Uma forma simples é usar uma variável global para escrita e leitura pelas tarefas.
* **Problema:** Em sistemas preemptivos, o SO pode interromper uma tarefa a qualquer momento para dar tempo de processamento a outra.
* Como a escrita/leitura de uma variável global geralmente envolve várias instruções assembly, a interrupção pode ocorrer durante esse processo, resultando em dados inconsistentes ou incompletos.
* Se outra tarefa ler esses dados, o resultado será imprevisível e provavelmente incorreto.

### Comunicação *Thread Safe*
* Mecanismos que evitam problemas de inconsistência são chamados de *thread safe*.
* Nesses sistemas, as tarefas podem ser interrompidas sem corromper os dados transferidos.
* O FreeRTOS oferece mecanismos *thread safe* como:
    * Filas (Queues)
    * Semáforos
    * Mutexes

---

## 2. Filas (Queues)
As filas são estruturas de dados do tipo **FIFO** (*first-in first-out*), onde o primeiro elemento adicionado é o primeiro a ser removido.

* São o principal meio de comunicação entre tarefas e interrupções no FreeRTOS.
* Funcionam tanto para troca de mensagens quanto como mecanismo de sincronização.

---

## 3. Funções de Manipulação de Filas 

| Nome da Função | Finalidade | Observação |
| :--- | :--- | :--- |
| `xQueueCreate` | Cria uma nova fila. | Deve ser executada antes de qualquer operação com a fila. |
| `vQueueDelete` | Apaga a fila e libera memória. | Importante para sistemas com pouca memória. |
| `xQueueSend` | Enfileira um elemento no final. | **Não** deve ser usada em interrupções (ISR). |
| `xQueueSendFromISR` | Enfileira um elemento no final. | Própria para uso dentro de interrupções. |
| `xQueueReceive` | Remove elemento do início. | **Não** deve ser usada em interrupções (ISR). |
| `xQueueReceiveFromISR`| Remove elemento do início. | Própria para uso dentro de interrupções. |

---

## 4. Detalhes Técnicos

### Criando Filas
A função `xQueueCreate()` cria um *buffer* na memória RAM. É necessário usar uma variável do tipo `QueueHandle_t` como identificador.

**Protótipo:**
```c
QueueHandle_t xQueueCreate(UBaseType_t uxQueueLength, UBaseType_t uxItemSize);
```
* `uxQueueLength`: Número de elementos na fila.
* `uxItemSize`: Tamanho em bytes de cada elemento.

### Inserindo Dados
Existem duas tratativas para inserção:
1.  **Entre Tarefas:** Usa-se a função genérica `xQueueSend()`.
2.  **Entre Interrupção e Tarefa:** Usa-se a função especializada `xQueueSendFromISR()`.

**Parâmetros de `xQueueSend`:**
* `xQueue`: Identificador da fila.
* `pvItemToQueue`: Ponteiro para o conteúdo.
* `xTicksToWait`: Tempo máximo de bloqueio caso a fila esteja cheia.

### Removendo Dados
Assim como na inserção, há funções específicas para tarefas (`xQueueReceive`) e interrupções (`xQueueReceiveFromISR`).

**Configurações de `xTicksToWait` em `xQueueReceive`:**
* `0`: Retorna imediatamente se a fila estiver vazia.
* `portMAX_DELAY`: Bloqueia indefinidamente aguardando um dado.

### Deletando Filas
Usa-se `vQueueDelete(QueueHandle_t xQueue)` para excluir a fila e liberar a memória RAM alocada.

---

## 5. Exemplos de Projetos

* **Exemplo 09 (Função Genérica):** Uma Tarefa A envia uma contagem a cada 500ms; uma Tarefa B lê a fila com *timeout* de 1s, indicando erro caso não receba dados.
* **Exemplo 10 (Interrupção):** Uma interrupção de pino (botão) incrementa uma variável e a envia para uma fila, que é monitorada por uma tarefa para exibição no LOG serial.
* **Exemplo 11 (Erro de Sincronização):** Demonstra problemas de compartilhamento de barramento serial entre tarefas, causando mensagens sobrepostas, o que pode ser resolvido com semáforos.

---

## 6. Referências 
* [Eletrogate - Filas no FreeRTOS](https://blog.eletrogate.com/freertos-filas-trocando-informacao-entre-tarefas/)
* [Embarcados - Comunicação entre tarefas](https://embarcados.com.br/comunicacao-entre-tarefas-no-freertos-filas/)
* [FreeRTOS.org - Queue Management](https://www.freertos.org/a00018.html)