# Projeto FreeRTOS - Controle de Luminosidade com ESP32

## Descrição Geral
Desenvolver uma aplicação utilizando **FreeRTOS** no ESP32 para realizar a leitura de luminosidade através de um sensor LDR e controlar um relé com base em um valor de *setpoint* ajustável. O sistema deve possuir modos de operação (automático e programação), interface de exibição em display e controle via botões.

## Requisitos Funcionais

### 1. Leitura de Luminosidade (Task 1)
*   Realizar a leitura analógica do LDR.
*   Converter o valor lido para porcentagem (0% a 100%).
*   Enviar o valor lido para outras tasks através de **Filas (Queues)**.

### 2. Controle de Relé (Modo Automático)
*   O sistema deve controlar o relé baseado na histerese para evitar oscilações rápidas.
*   **Ligar o Relé:** Quando a luminosidade estiver **abaixo** do valor de *Setpoint* menos 5 Percent (ou equivalente em porcentagem).
*   **Desligar o Relé:** Quando a luminosidade estiver **acima** do valor de *Setpoint* mais 5 Percent.
*   A diferença entre os limiares de ligar e desligar garante uma histerese de 10%.

### 3. Controle de Menu e Modos (Task 2)
Esta task gerencia a máquina de estados do sistema:

*   **Modo Automático (Padrão):**
    *   O sistema opera realizando a leitura e controle do relé.
    *   O LED indicativo de programação deve permanecer **apagado**.
    
*   **Modo Programação:**
    *   **Entrada:** Ao detectar um pulso no **Botão 1**, o sistema entra em modo de configuração.
    *   **Indicação:** O LED indicativo deve acender.
    *   **Ajuste:** Permite incrementar ou decrementar o valor do *Setpoint* através de outros dois botões (Botões de ajuste).
    *   **Faixa de Valor:** O *Setpoint* deve variar de **00 a 99**.
    *   **Saída:** Ao detectar um segundo pulso no **Botão 1**, o sistema sai do modo programação (LED apaga) e retorna ao modo automático com o novo valor salvo.

### 4. Exibição no Display (Task 3)
*   O display deve ser atualizado em uma task dedicada.
*   **Parâmetros de Entrada:** Recebe via Fila o valor a ser exibido e o estado atual (Configuração ou Automático).
*   **Exibição:**
    *   Em **Modo Automático**: Exibir o valor atual da luminosidade (0-100%).
    *   Em **Modo Programação**: Exibir o valor do *Setpoint* sendo ajustado.

