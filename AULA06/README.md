## 1. Passagem de Parâmetros para Tasks

A passagem de informações para uma task é realizada através do uso de **ponteiros**.

* A técnica mais comum utiliza um **ponteiro void** (`void *`).
* Ponteiros void não possuem tipo definido nem alocação de memória prévia, permitindo a passagem de qualquer tipo de dado.
* Para recuperar a informação dentro da task, utiliza-se o **typecast** para converter o ponteiro de volta ao tipo original.

### A Estrutura no `xTaskCreate`
Na função de criação de task, o parâmetro responsável por essa comunicação é o `pvParameters`.

```cpp
// Assinatura da função xTaskCreate 
BaseType_t xTaskCreate(
    TaskFunction_t pxTaskCode,      // Função da task
    const char * const pcName,     // Nome da task
    const uint32_t usStackDepth,   // Tamanho da stack
    void * const pvParameters,     // PARÂMETRO ENVIADO (ponteiro void) 
    UBaseType_t uxPriority,        // Prioridade
    TaskHandle_t * const pxCreatedTask // Handle da task
);
```

Este campo permite passar o endereço de qualquer variável ou estrutura de dados.

---

## 2. Exemplos Práticos

### Passando um Parâmetro Simples (Ex: Pino de um LED)
Neste exemplo, passamos apenas o número do pino para que a mesma função possa controlar diferentes LEDs.

**Criação da Task:**
```cpp
// Protótipo da função 
void vTaskBlink(void *pvParameters);

// Criação passando o valor LED1 como parâmetro 
xTaskCreate(vTaskBlink, "TASK1", configMINIMAL_STACK_SIZE, (void*)LED1, 1, &task1Handle);
```

**Rotina da Task:**
```cpp
void vTaskBlink(void *pvParameters) {
    // Converte o parâmetro void de volta para inteiro 
    int pin = (int) pvParameters; 
    pinMode(pin, OUTPUT); 

    while (1) {
        digitalWrite(pin, !digitalRead(pin)); 
        vTaskDelay(pdMS_TO_TICKS(200)); 
    }
}
```

### Passando Múltiplos Parâmetros (Uso de `struct`)
Para enviar mais de uma informação (ex: pino e tempo de delay), utiliza-se uma **struct**.

**Definição e Criação:**
```cpp
// Definição da estrutura 
typedef struct {
    int pino; 
    int tempo; 
} TaskParams_t;

// Instância dos parâmetros 
static TaskParams_t params1 = {
    .pino = 14, 
    .tempo = 500 
};

// Criação da Task passando o endereço da struct 
xTaskCreate(vTaskBlink, "TASK1", configMINIMAL_STACK_SIZE, &params1, 1, &task1Handle);
```

**Rotina da Task com Struct:**
```cpp
void vTaskBlink(void *pvParameters) {
    // Converte o ponteiro void para o tipo da struct 
    TaskParams_t *params = (TaskParams_t *)pvParameters;
    
    int pin = params->pino; 
    int tempo = params->tempo; 
    
    pinMode(pin, OUTPUT); 
    
    while (1) {
        digitalWrite(pin, !digitalRead(pin)); 
        vTaskDelay(pdMS_TO_TICKS(tempo)); 
    }
}
```

---

## 3. Seleção do Núcleo de Execução

Em sistemas multicore (como o ESP32), utiliza-se a função `xTaskCreatePinnedToCore` para definir em qual processador a task será executada.

* **Núcleo 0:** Chamado de `APP_CPU_NUM`.
* **Núcleo 1:** Chamado de `PRO_CPU_NUM`.

```cpp
// Exemplo de definição de núcleo 
xTaskCreatePinnedToCore(vTaskBlink, "TASK1", configMINIMAL_STACK_SIZE, (void*)LED1, 1, &task1Handle, APP_CPU_NUM);
xTaskCreatePinnedToCore(vTask2, "TASK2", configMINIMAL_STACK_SIZE+1024, (void*)valor, 2, &task2Handle, PRO_CPU_NUM);
```

---

## 4. Monitoramento da Stack (Memória)

Para garantir que a task não ultrapasse o limite de memória alocado, utiliza-se a função `uxTaskGetStackHighWaterMark`.

* A função retorna o valor em **bytes** de memória ainda disponível para a task (o "ponto alto" de folga).
* O tipo de dado para essa leitura é o `UBaseType_t`.

```cpp
void vTaskBlink (void *pvParameters) {
    UBaseType_t uxHighWaterMark; 
    // ... código da task ...

    while (1) {
        // ... lógica ...
        
        // Monitora a folga da stack 
        uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        
        // Imprime o nome da task e a memória disponível 
        Serial.print(pcTaskGetTaskName(NULL));
        Serial.print(" : ");
        Serial.println(uxHighWaterMark);
        
        vTaskDelay(pdMS_TO_TICKS(200)); 
    }
}