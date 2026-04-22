## O que são Semáforos?

Semáforos são mecanismos de **sincronização** e **exclusão mútua** utilizados para controlar o acesso a recursos compartilhados entre diferentes tarefas, ou entre tarefas e interrupções.

Imagine um cenário onde duas tarefas (**tAccessTask 1** e **tAccessTask 2**) precisam acessar um mesmo recurso compartilhado. O semáforo binário atua como um sinalizador para garantir que o acesso ocorra de forma organizada.

---

## Tipos e Funcionamento

Em programação paralela, um semáforo é um tipo abstrato de dado que restringe o acesso a um recurso ou comunica eventos entre tarefas e rotinas de interrupção (ISRs).

O funcionamento é análogo aos semáforos de vias urbanas, que sincronizam o fluxo de veículos. Existem duas operações fundamentais:
* **Take (Pegar):** A tarefa solicita o semáforo para iniciar o uso do recurso.
* **Give (Liberar):** A tarefa libera o semáforo após o uso, permitindo que outros o utilizem.

---

## Sincronização com Interrupções

Um uso comum dos semáforos binários é a sincronização entre uma tarefa e uma interrupção:

1.  **Estado Bloqueado:** A tarefa tenta realizar um `xSemaphoreTake()`. Se o semáforo não estiver disponível, ela fica bloqueada.
2.  **Ocorrência da Interrupção:** Quando ocorre um evento externo, a interrupção executa a função `xSemaphoreGiveFromISR()`.
3.  **Desbloqueio:** O semáforo torna-se disponível, a tarefa sai do estado de bloqueio e consegue dar o "take" com sucesso.
4.  **Processamento:** A tarefa executa sua ação e, ao terminar, tenta pegar o semáforo novamente, voltando ao estado bloqueado até a próxima interrupção.

Este modelo é conhecido como **processamento adiado (deferred processing)**, onde a interrupção (ISR) apenas sinaliza o evento, permitindo que o processamento pesado ocorra dentro de uma tarefa comum, mantendo o sistema responsivo.

---

## Classificação dos Semáforos

O FreeRTOS trabalha principalmente com três tipos:

### 1. Semáforo Binário
É o tipo mais simples, funcionando como uma variável booleana (0 ou 1).
* **Valor 1:** O semáforo está disponível.
* **Valor 0:** O semáforo está ocupado e a tarefa que tentar pegá-lo será bloqueada.

### 2. Mutex (Mutual Exclusion)
Similar ao binário, mas com foco em exclusão mútua e posse de recurso. Possui uma característica vital chamada **Herança de Prioridade**.
* Se uma tarefa de alta prioridade for bloqueada por um Mutex que está com uma tarefa de baixa prioridade, o FreeRTOS aumenta temporariamente a prioridade da tarefa que detém o Mutex.
* Isso garante que a tarefa termine seu trabalho mais rápido e libere o recurso, minimizando o problema de **inversão de prioridades**.

### 3. Semáforo Contador (Counting Semaphore)
Funciona como uma fila ou um vetor de valores.
* É ideal para não perder eventos de interrupção. Se várias interrupções ocorrerem antes que a tarefa consiga processá-las, o semáforo contador acumula esses avisos (latches).
* Enquanto o contador for maior que zero, a tarefa pode dar o "take" sem entrar em estado de bloqueio.

---

## Exemplo de Uso do Mutex

O Mutex é essencial para proteger recursos críticos:

* **Cenário:** As tarefas A e B desejam acessar um recurso guardado por um Mutex.
* **Acesso:** A Tarefa A solicita o Mutex e, como ele está disponível, torna-se a sua detentora e acessa o recurso.
* **Bloqueio:** Se a Tarefa B tentar acessar o mesmo recurso enquanto a A ainda o possui, a tentativa de `xSemaphoreTake()` falhará e a Tarefa B será bloqueada.
* **Liberação:** Assim que a Tarefa A devolve o Mutex, a Tarefa B é desbloqueada, obtém o semáforo e ganha permissão para acessar o recurso.

---
