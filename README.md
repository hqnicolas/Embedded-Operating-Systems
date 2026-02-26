# AULA 01 – Atividades com ESP32 / Arduino

Nesta aula você irá desenvolver dois projetos utilizando botões e saídas digitais no ESP32 (ou Arduino compatível).

---

## Exercício 01 – Controle de LED RGB com Botões e Temporizador

###  Objetivo

Criar um circuito com:

- 1 LED RGB (ou 3 LEDs separados: vermelho, verde e azul)
- 2 botões
- Resistores apropriados
- ESP32

###  Funcionamento esperado

O sistema deve:

1. Fazer o LED vermelho piscar automaticamente a cada 200 ms (utilizando `millis()`, sem `delay()`).
2. Acender o LED verde enquanto o Botão 1 estiver pressionado.
3. Acender o LED azul enquanto o Botão 2 estiver pressionado.
4. Utilizar `INPUT_PULLUP` para os botões.

###  Conceitos trabalhados

- Saída digital (`digitalWrite`)
- Entrada digital com `INPUT_PULLUP`
- Controle de tempo com `millis()`
- Estruturas condicionais (`if`)
- Variáveis booleanas

###  O que o estudante deve montar

- Conectar os pinos do LED RGB aos pinos definidos no código.
- Conectar os botões aos pinos indicados.
- Garantir que os botões funcionem em lógica invertida (LOW quando pressionado).

---

##  Exercício 02 – Contador com Display de 7 Segmentos

###  Objetivo

Criar um contador numérico (0 a 9) utilizando:

- 1 Display de 7 segmentos
- 2 botões
- ESP32
- Resistores para os segmentos

### Funcionamento esperado

O sistema deve:

1. Mostrar números de 0 a 9 no display.
2. Quando o botão **Aumenta** for pressionado:
   - Iniciar a contagem automática de 0 até 9.
   - Atualizar o número a cada 1 segundo.
3. Se o botão **Diminui** for pressionado durante a contagem:
   - Interromper a contagem.
4. Quando o botão **Diminui** for pressionado:
   - Resetar o contador para 0.

### Conceitos trabalhados

- Arrays bidimensionais
- Controle de display de 7 segmentos
- Estruturas de repetição (`for`)
- Interrupção de laço com `break`
- Manipulação de múltiplos pinos digitais

### O que o estudante deve montar

- Conectar corretamente os 7 segmentos (A, B, C, D, E, F, G) aos pinos definidos.
- Identificar se o display é:
  - Catodo comum
  - Anodo comum  
- Ajustar a lógica caso necessário.
- Conectar os dois botões utilizando `INPUT_PULLUP`.

---

## Entregáveis

O estudante deve entregar:

- Montagem física funcionando
- Código comentado
- Vídeo demonstrando o funcionamento dos dois exercícios
- Explicação do funcionamento do código

---

## Desafio Extra (Opcional)

- Modificar o contador para contar de forma crescente e decrescente.
- Implementar debounce sem utilizar `delay()`.
- Adicionar contador automático contínuo.

---

Boa prática e bons testes! 💡