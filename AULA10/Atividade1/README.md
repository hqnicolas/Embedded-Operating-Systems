# Atividade 1 - Controle de Ciclo com Event Groups

## Objetivo
Desenvolver um programa utilizando **FreeRTOS Event Groups** para controlar uma sequencia automatizada iniciada pelo botao `Start`.

## Descricao do hardware
Utilize o seguinte mapeamento de pinos no ESP32:

```c
#define BOTAO1 0
#define S1 2
#define S2 4
```

- `BOTAO1`: botao responsavel por iniciar o ciclo.
- `S1`: saida da valvula solenoide 1.
- `S2`: saida da valvula solenoide 2.

## Descricao do funcionamento
Ao pressionar o botao `Start`, o sistema deve iniciar um ciclo temporizado com os seguintes eventos:

1. **Apos 5 segundos**, a valvula solenoide `S1` deve ser acionada.
2. `S1` deve permanecer ligada por **3 segundos**.
3. **1 segundo depois**, a valvula solenoide `S2` deve ligar.
4. `S2` deve permanecer ligada por **5 segundos** e, em seguida, desligar.
5. **1 segundo depois**, o motor deve ser acionado.
6. O motor deve permanecer ligado por **10 segundos**.
7. Ao final, o ciclo deve ser encerrado.

## Requisitos
- Utilizar **Event Groups** para sincronizar as tarefas.
- O pressionamento do botao deve disparar o inicio do ciclo.
- Cada atuador deve respeitar o tempo definido no enunciado.
- O programa deve finalizar o ciclo corretamente apos a execucao do motor.

## Sugestao de organizacao
Voce pode dividir a aplicacao em tarefas, por exemplo:

- uma tarefa para monitorar o botao `Start`;
- uma tarefa para controlar `S1`;
- uma tarefa para controlar `S2`;
- uma tarefa para controlar o motor;
- uma tarefa principal ou mecanismo de controle para coordenar a sequencia.

## Exemplo de eventos
Alguns bits do grupo de eventos podem representar estados como:

- `BIT_START` para indicar o inicio do ciclo;
- `BIT_S1` para liberar o acionamento da valvula `S1`;
- `BIT_S2` para liberar o acionamento da valvula `S2`;
- `BIT_MOTOR` para liberar o acionamento do motor;
- `BIT_FIM` para indicar o encerramento do processo.

## Entrega esperada
O codigo deve demonstrar:

- criacao do grupo de eventos;
- sincronizacao entre tarefas por meio dos bits;
- controle correto dos tempos de acionamento;
- encerramento completo do ciclo.
