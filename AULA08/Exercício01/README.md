## Exercício: Botão Aciona a Leitura do Ultrassom

* O botão gera uma interrupção.
* A ISR não faz a leitura do sensor; ela apenas libera o semáforo com `xSemaphoreGiveFromISR()`.
* A task `vTaskUltrassom` fica bloqueada em `xSemaphoreTake()`.
* Quando o semáforo é entregue, a task acorda, lê o sensor ultrassônico e envia o resultado pela Serial.

Esse padrão é didático porque deixa a interrupção leve e transfere o processamento principal para uma task do FreeRTOS.