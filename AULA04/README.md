# Timers no ESP32

**Disciplina:** IoT Aplicada 

**Professor:** Me. Cleber Lourenço Izidoro 

**Instituição:** Centro Universitário UNISATC 

**Curso:** Engenharia de Computação 

---

## Introdução aos Timers do ESP32

* Os SoCs ESP32 são equipados com **4 temporizadores de hardware**.


* Cada temporizador possui um **contador de 64 bits** para cima/baixo.


* Eles incluem um **pré-escalador (prescaler) de 16 bits**.


* O clock base utilizado é o **APB (APB_CLK)**, que opera normalmente a **80 MHz**.


* O prescaler reduz esse clock para gerar o tempo de tique-taque (tick) da base de tempo.


* Alteramos o valor do prescaler para controlar a duração de cada tique do timer.


* O prescaler pode dividir o APB_CLK por um fator entre **2 e 65536**.


* Se definido como 1 ou 2, o divisor é 2.


* Se definido como 0, o divisor é 65536.


* Qualquer outro valor divide o clock exatamente pelo valor registrado.





---

## Rotinas de Configuração: `timerBegin()`

A função `timerBegin` é responsável por iniciar o temporizador com uma frequência específica de ticks.

* **Frequência (Hz):** Define a resolução do temporizador.


* **Exemplo:** Ao definir a frequência para **1.000.000 Hz (1 MHz)**, o contador incrementará a cada **1 microssegundo** ($1 / 1.000.000s$).



---

## Rotinas de Configuração: `timerAttachInterrupt()`

Esta função associa o temporizador configurado a um endereço de interrupção.

* **Sintaxe:** `timerAttachInterrupt(timer, &onTimer);` 


* **Objetivo:** Apontar para a função (neste caso, `onTimer`) sempre que houver um **estouro (overflow)** do temporizador.



---

## Rotinas de Configuração: `timerAlarm()`

Utilizada para configurar o comportamento do alarme do temporizador.

| Parâmetro | Descrição |
| --- | --- |
| **Timer** | O ponteiro do temporizador a ser configurado.|
 | **Alarm_value** | Número de contagens necessárias para ocorrer o overflow.|
| **Autoreload** | Se `true`, o timer reseta automaticamente após o estouro.|
| **Reload_count** | Número de disparos permitidos; se for 0, será sempre recarregado.|

---

## Exemplos e Finalização



> 
> **[Inserir Imagem do Slide 7 aqui: Mão segurando caneta escrevendo "Obrigado!"]** 
> 
> 

---