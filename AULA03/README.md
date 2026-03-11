# Aula 03: Entradas e Saídas Analógicas – Porta Serial

**Disciplina:** IoT Aplicada

**Professor:** Me. Cleber Lourenço Izidoro

**Instituição:** Centro Universitário UNISATC – Engenharia de Computação


---

## Tipos de Entradas Analógicas

Os microcontroladores, em geral, podem possuir diferentes circuitos para efetuar a conversão analógica para digital, sendo elas: 

* **Conversão direta;** 


* **SAR - Aproximação sucessiva;** 


* **ADC Delta-Sigma;** 


---

## Conversão Direta ou Flash

* Também chamado de conversor paralelo ou conversor flash; 


* Possui uma tensão de referência e diversos comparadores de tensão para obter a tensão de saída; 


* São utilizados em aquisição de sinais em aparelhos de medição avançados, como osciloscópios, devido à sua alta velocidade, compensando assim seu custo maior; 


* Por serem muito rápidos e eficientes, muitos circuitos integrados hoje em dia utilizam apenas um conversor paralelo para mais de uma entrada analógica. 



---

## Conversão SAR - Aproximação Sucessiva



* Necessita de processamento para efetuar a conversão; 


* Ocupa pouco espaço e tem um baixo consumo de energia. 



---

## Conversão SAR – Funcionamento (Passo 1)

* Primeiro, o ADC SAR rastreia o valor da entrada analógica; 


* Cada ADC SAR terá um tempo mínimo de rastreamento; 


* Neste caso, a referência é de 2,4V. 



---

## Conversão SAR – Funcionamento (Passo 2)

* A entrada analógica é amostrada e mantida durante o processo de conversão. 



---

## Conversão SAR – Funcionamento (Passo 3)

* O DAC é definido para metade da saída de fundo de escala e comparado ao valor de entrada mantido. 



---

## Conversão SAR – Funcionamento (Passos seguintes)

* O resultado da primeira comparação é movido para o valor de teste, o próximo bit é definido como 1 e a saída do DAC é comparada à entrada novamente. 


* Esta etapa é repetida para cada bit no ADC. 



---

## Conversão SAR – Funcionamento (Final)

* O resultado final de 4 bits é publicado e o ADC retorna ao rastreamento do sinal de entrada (Etapa 1). 




---

## Conversão Delta-Sigma

* Um projeto ADC mais recente é o ADC Delta-Sigma (ou conversor delta), que aproveita a tecnologia DSP para melhorar a resolução do eixo de amplitude e reduzir o ruído de quantização de alta frequência inerente aos projetos SAR. 



---

## ESP32 - Entrada Analógica

* O ESP32 possui 2x12bit SAR (Sucessive Approximation Register) que pode ler 18 diferentes pinos de entrada analógica divididos em 8 canais do ADC1 e 10 canais do ADC2. 


* Entretanto, o uso do ADC2 possui algumas restrições: 


* O ADC2 é usado pelo driver Wi-Fi, ou seja, só se pode usar o ADC2 quando o Wi-Fi não estiver sendo utilizado; 


* Alguns dos pinos do ADC2 são usados como pinos de “strapping” (GPIO0, 2, 5, 12 e 15), portanto, não podem ser usados livremente. 



---

## ESP32 – Atenuação ADC

* É possível modificar a atenuação do conversor ADC. 


* A atenuação padrão utilizada pelas funções do Core do Arduino é de -11dB. 


* Devido às características do ADC, os resultados mais precisos são obtidos dentro de uma “faixa sugerida” mostrada na tabela a seguir. 

 

---

## ESP32 – Resolução ADC

* O ESP32 possui um conversor ADC com resolução programável. 


* A resolução padrão é de 12 bits (o que dá leituras de 0 – 4095). 


* No entanto, pode-se alterar para resoluções de 9, 10 ou 11 bits através da função do Core do Arduino: `analogReadResolution(resolução)`. 


---

## ESP32 – Clock ADC

* O SAR ADC faz a conversão em vários ciclos de clock, dependendo da resolução da conversão. 


* Quanto mais rápida for a taxa de clock, mais rápido o ADC terminará cada processo de conversão A/D. 


* Podemos controlar a frequência de clock do ADC modificando o fator de divisão da frequência de clock do conversor. 


* O tempo de conversão mais rápido para o ADC ocorre quando `clockDiv = 1`, enquanto a opção mais lenta é quando `clockDiv = 255`. 



---

## ESP32 – Linearidade ADC

* Um ADC ideal deve ter uma resposta linear conforme representa a curva na cor azul da figura abaixo. 


* O conversor AD do ESP32 possui uma resposta não linear, representada pela curva em vermelho. 


* **Problema:** Como poderíamos resolver o problema da leitura do sensor LM35? 



---

## ESP32 - Saída Analógica (DAC)

* DAC significa Digital-to-Analog Converter. 


* Em resumo, ele faz o inverso do ADC: converte um valor digital em uma tensão analógica (de 0 até a tensão de alimentação). 



---

## Funcionamento do DAC no ESP32

* Um DAC recebe como entrada um valor digital:
* O valor digital 0 produz na saída uma tensão de 0V; 


* O valor digital máximo produz na saída uma tensão no valor da tensão de alimentação. 




* Software: O DAC é suportado de forma nativa pelas bibliotecas do ESP32 através da função: `dacWrite(canal_DAC, valor_digital);` 


* **canal_DAC:** 25 (GPIO 25/DAC0) ou 26 (GPIO 26/DAC1); 


* **valor_digital:** 0 a 255 (0 = 0V; 255 = 3.3V). 



---

## ESP32 – Modulação PWM

Etapas para utilizar PWM no IDE do Arduino: 

1. Escolher um canal (0 a 15) e vincular um pino; 


2. Definir a frequência (ex: 5000 Hz para um LED); 


3. Definir a resolução (1 a 16 bits); 


4. Usar `ledcSetup(canal, freq, resolution);` 


5. Usar `ledcAttachPin(GPIO, canal);` 


6. Controlar o brilho com `ledcWrite(canal, dutycycle);` 



---

## Porta Serial (RS232)

* Padrão de comunicação de um PC que geralmente utiliza conector DB9. 



**Parâmetros:** 

* Data bits: 7 ou 8;
* Paridade: Não, Par ou Ímpar;
* Stop Bits: 1 ou 2;
* Baud Rate: 110 a 38400 bits/s;
* Distância máxima: 15 metros. 



**Níveis Lógicos:** 

* -3 V a -15 V: Marca (Logic 1 / OFF);
* +3 V a +15 V: Espaço (Logic 0 / ON);
* Tensões entre -3 V e +3 V são indefinidas. 



---

## Interfaces UART no ESP32

O ESP32 suporta até três interfaces UART: UART0, UART1 e UART2. 

* **UART0:** Geralmente reservada para o Monitor Serial (upload e depuração). Pode ser usada para outros dispositivos se o monitor não for necessário. 


* **UART1 e UART2:** Disponíveis para comunicação com dispositivos externos. 



**Funções Principais:** 

* `Serial.begin(115200)`: Inicia a comunicação; 


* `Serial.println(" ")`: Escreve caracteres; 


* `Serial.available()`: Verifica dados recebidos. 

