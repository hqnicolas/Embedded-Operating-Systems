# IoT Aplicada: Classes, Objetos e Bibliotecas

## 1. O que é Programação com Classes e Objetos?

Programar utilizando classes e objetos representa um avanço na programação de sistemas embarcados. Essa abordagem permite:

* 
**Automação:** Muitas tarefas são automatizadas.


* 
**Simplificação:** Facilita a organização de sistemas complexos.



---

## 2. Conceitos Básicos: O Exemplo do LED

Em vez de criar diversas variáveis isoladas (como `pinLed1` ou `estadoLed1`), utiliza-se uma **Classe** chamada LED.

### Estrutura da Classe:

* **Atributos (Dados):** O que o objeto "sabe". Exemplo: O número do pino.


* **Métodos (Funções):** O que o objeto "faz". Exemplo: Ligar, desligar ou piscar.



---

## 3. Exemplo Prático: Classe Botão

A criação de uma classe para botões permite configurar o pino e retornar se a entrada está ativa de forma padronizada.

> 
> **Regra Importante:** O **construtor** da classe deve obrigatoriamente ter o mesmo nome da Classe.
> 
> 

O material sugere três modos de implementação (Modo 1, 2 e 3) para fins didáticos.

---

## 4. Criando uma Biblioteca em $C++$

Para organizar o código em uma biblioteca externa, são necessários dois arquivos na pasta do projeto:

| Arquivo | Nome | Função |
| --- | --- | --- |
| **.h** | *Header* | O "cardápio": declara quais funções e variáveis a classe possui.

 |
| **.cpp** | *Source* | A "cozinha": onde o código real de cada função é escrito.

 |

---

## 5. Exercício Prático: Monitoramento de Motores

Desenvolva um firmware para controlar a sinalização de motores via LED, seguindo as normas de segurança:

### Padrões de Sinalização Requeridos:

* 
**Status OK:** Pulso único e curto com intervalos iguais.


* 
**Atenção (Manutenção Preventiva):** Sequência de 3 pulsos rápidos, controlados por ciclos.


* 
**Emergência (Parada Crítica):** Padrão SOS (3 pulsos longos, 3 curtos, 3 longos).



### Requisitos da Biblioteca:

* Gerenciar múltiplos sinalizadores de forma independente.


* Permitir ritmos (tempos de resposta) diferentes para cada sinalizador (ex: lâmpadas lentas vs. LEDs instantâneos).
