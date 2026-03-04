# Entradas Digitais e Filtro Bounce

## 1. O que é o Efeito Bouncing?

Quando trabalhamos com circuitos que envolvem **chaves mecânicas**, surge um fenômeno físico chamado **bouncing**. Esse efeito é caracterizado por:

* Pequenas oscilações mecânicas no momento do contato.


* Indicação equivocada de múltiplos acionamentos em um intervalo de tempo muito curto.


* Problemas de leitura em circuitos digitais que esperam apenas uma transição de estado.



> **[Inserir Imagem: Gráfico do efeito bouncing mostrando as oscilações de sinal entre 0V e 5V no momento do acionamento]**

---

## 2. Circuitos de Acionamento Típicos

Para ler chaves mecânicas, utilizam-se comumente resistores para garantir níveis lógicos definidos.

### Pull-Up e Pull-Down

* **Resistor de Pull-Up:** Conecta o pino ao VCC (ex: 5V). Quando a chave é pressionada, o pino é aterrado (0V).


* **Resistor de Pull-Down:** Conecta o pino ao Terra (0V). Quando a chave é pressionada, o pino recebe o sinal do VCC (5V).



> **[Inserir Imagem: Esquema elétrico comparativo entre circuito Pull-Up e Pull-Down]**

---

## 3. Soluções para o Bouncing

Existem duas formas principais de mitigar esse efeito: filtros analógicos (hardware) e filtros digitais (software).

### Filtro Analógico (Hardware)

Consiste no uso de componentes passivos, como capacitores, para suavizar a transição do sinal.

* 
**Componentes:** Geralmente um capacitor (ex: $10\mu F$) em paralelo com a chave ou o resistor.


* 
**Limitação:** Auxilia no problema, mas não o resolve com precisão extrema.



> **[Inserir Imagem: Esquema de circuito com capacitor de filtro (C1) e resistor (R1)]**

### Filtro Digital (Software)

Permite verificar a transição de estado (subida ou descida) ignorando as oscilações rápidas, o que é essencial para processos de contagem de pulsos.

#### Exemplo de Implementação 1: Verificação de Estado com Delay

Utiliza uma variável auxiliar e um pequeno atraso para confirmar o acionamento.

```c
// Exemplo simplificado baseado no material
int inc=0, aux=0, contador=0;

inc = !HAL_GPIO_ReadPin(GPIOB, IN1_Pin); [cite_start]// Leitura do pino [cite: 65]

[cite_start]if (inc && !aux){ // Detecta transição [cite: 67]
    aux = 1;
    ++contador;
    Delayms(300); [cite_start]// Filtro por tempo [cite: 68, 69, 70]
} 
else if (!inc) {
    aux = 0; [cite_start]// Reseta auxiliar [cite: 71, 72]
}

```

#### Exemplo de Implementação 2: Contador de Confirmação

Utiliza um contador decrescente para garantir que o sinal se estabilizou antes de validar a leitura.

```c
[cite_start]// Exemplo de lógica de contagem [cite: 76, 78]
inc = !HAL_GPIO_ReadPin(GPIOB, IN1_Pin);
if (inc && aux_cont > 0){
    --aux_cont;
    Delayms(1);
    if (aux_cont == 0){
        [cite_start]// Incrementa contador após estabilização [cite: 82, 83]
    }
} 
else if (!inc) {
    aux_cont = 255; [cite_start]// Reseta o filtro [cite: 86, 87]
}

```

---

