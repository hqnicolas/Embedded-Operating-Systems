# Aula 05 - Exercício 01 (ESP32 + FreeRTOS no Wokwi)

## Objetivo
Aplicar conceitos de **multitarefa com FreeRTOS** no ESP32, utilizando:
- Leitura de entradas analógicas
- Controle de tarefas (suspender e retomar)
- Temporização com `vTaskDelay`
- Manipulação de LEDs e Serial

---

## Ambiente
- Simulador: https://wokwi.com/
- Placa: **ESP32 DevKit V1**
- Linguagem: C/C++ (Arduino Framework)
- Bibliotecas: FreeRTOS (já inclusa no ESP32)

---

## Montagem do Circuito

Você deverá configurar no Wokwi:

- 2 potenciômetros:
  - **POT1** → GPIO 34 (entrada analógica principal)
  - **POT2** → GPIO 35 (controle de tempo)

- 1 LED:
  - GPIO 2 (LED de status)

---

## Requisitos do Sistema

Crie **tasks no FreeRTOS** para executar as seguintes ações:

---

### Regra principal (leitura do POT1)

Uma task deve fazer a leitura contínua do valor analógico do **POT1** e controlar as demais tarefas:

| Valor lido | Ação |
|-----------|------|
| `< 500` | Suspende todas as tasks (nenhuma executa) |
| `>= 500 e < 1000` | Executa task que pisca LED a cada 500ms |
| `>= 1000 e < 2000` | Executa task que imprime contador na Serial |
| `> 2000` | Executa task que usa POT2 para controlar o tempo do LED |

---

### Task 1 - Piscar LED
- Pisca o LED a cada **500ms**
- Só executa na faixa `500–999`

---

### Task 2 - Contador Serial
- Imprime um contador crescente no Serial Monitor
- Exemplo:
  ```
  Contador: 1
  Contador: 2
  ```
- Só executa na faixa `1000–1999`

---

### Task 3 - Controle com POT2
- Lê o valor do **POT2**
- Ajusta o tempo de piscar do LED entre:
  - **100ms (mínimo)**
  - **1000ms (máximo)**
- Só executa quando valor do POT1 for `> 2000`

---

## Regras importantes

- Utilize:
  - `xTaskCreate`
  - `vTaskSuspend`
  - `vTaskResume`
- Apenas **uma task deve estar ativa por vez**
- Use `analogRead()` para leitura
- Utilize `map()` para converter valores do POT2

---

## Dicas

- Declare handles das tasks:
  ```cpp
  TaskHandle_t taskLED;
  TaskHandle_t taskSerial;
  TaskHandle_t taskControle;
  ```

- Use delays sem bloquear o sistema:
  ```cpp
  vTaskDelay(pdMS_TO_TICKS(500));
  ```

- Cuidado para não ficar resumindo tasks repetidamente (controle estado atual)

---

## Testes esperados

| Situação | Resultado esperado |
|--------|------------------|
| POT1 baixo (<500) | LED apagado e nada acontece |
| POT1 médio (~700) | LED piscando 500ms |
| POT1 alto (~1500) | Contador na serial |
| POT1 muito alto (>2000) | LED varia velocidade com POT2 |

---

## Desafio Extra (Opcional)

- Evitar reativar tasks desnecessariamente (controle de estado)
- Usar `vTaskSuspend(NULL)` dentro da própria task
- Implementar debounce lógico entre mudanças de estado

---
