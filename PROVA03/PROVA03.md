# PROVA 03 - Avaliacao FreeRTOS

## Mapeamento de Entradas e Saidas

| Dispositivo | Pino |
| --- | ---: |
| LED de alarme | 4 |
| Valvula de alivio | 0 |
| Botao de emergencia | 2 |
| LDR | 39 |
| Potenciometro / transdutor de pressao | 34 |
| Segmento A | 18 |
| Segmento B | 5 |
| Segmento C | 21 |
| Segmento D | 3 |
| Segmento E | 1 |
| Segmento F | 23 |
| Segmento G | 22 |
| Ponto decimal | 19 |
| Display dezena | 16 |
| Display unidade | 17 |

## Situacao-Problema

Desenvolva, utilizando a placa de desenvolvimento e o FreeRTOS, uma solucao para o controle de seguranca de uma prensa hidraulica.

A prensa deve exibir a pressao medida por um transdutor, na faixa de `0` a `99 bar`, em dois displays de 7 segmentos. Quando a pressao ultrapassar `50 bar`, o sistema deve entrar em estado de alarme e interromper a prensagem.

Alem do alarme por pressao, o sistema tambem deve monitorar:

- um sensor de luminosidade `LDR`;
- um botao de emergencia, acionado pela borda de descida.

Quando qualquer uma dessas condicoes de alarme ocorrer, o processo deve ser interrompido e a valvula de alivio deve ser acionada.

## Condicoes de Alarme

| Condicao | Acionamento | Limpeza automatica | Indicacao no LED |
| --- | --- | --- | --- |
| Pressao acima de `50 bar` | `pressao > 50` | `pressao <= 50` | Piscar a cada `250 ms` |
| LDR acionado | `luminosidade >= 50%` | `luminosidade < 50%` | Piscar a cada `500 ms` |
| Botao de emergencia pressionado | borda de descida e botao em nivel baixo | botao solto, em nivel alto | Piscar a cada `1 s` |

## Comportamento de Recuperacao

Cada origem de alarme controla seu proprio bit no `EventGroupAlarmes`.

- A `TaskPressao` seta `ALARME_PRESSAO` quando a pressao passa de `50 bar` e limpa esse bit quando a pressao volta para `50 bar` ou menos.
- A `TaskLDR` seta `ALARME_LDR` quando a luminosidade fica em `50%` ou mais e limpa esse bit quando volta para abaixo de `50%`.
- A `TaskBotaoEmergencia` e acordada por interrupcao na borda de descida do botao, seta `ALARME_EMERGENCIA` enquanto o botao esta pressionado e limpa esse bit quando o botao e solto.
- A `TaskAlarme` mantem a valvula de alivio acionada enquanto existir qualquer bit de alarme ativo. Quando todos os bits sao limpos, a valvula e desligada e o `TimerLED` para.

## Requisitos de Implementacao

- Utilizar o maximo possivel dos recursos do FreeRTOS.
- Organizar a solucao usando `tasks`, `software timers` e `event groups`.
- Exibir continuamente a pressao nos dois displays de 7 segmentos.
- Detectar a condicao de sobrepressao acima de `50 bar`.
- Detectar o acionamento do `LDR`.
- Detectar o botao de emergencia na borda de descida.
- Acionar a valvula de alivio sempre que houver qualquer estado de alarme.
- Limpar automaticamente cada alarme quando sua entrada voltar para um valor aceitavel.
- Alterar o padrao de piscada do LED conforme a origem do alarme ativo.

## Organizacao da Solucao

- `TaskPressao`: leitura do transdutor ou potenciometro, atualizacao dos displays e controle do bit de sobrepressao.
- `TaskLDR`: monitoramento do sensor de luminosidade e controle do bit de LDR.
- `TaskBotaoEmergencia`: deteccao da borda de descida por interrupcao, debounce e limpeza ao soltar o botao.
- `TaskAlarme`: tratamento dos eventos de alarme, acionamento da valvula e parada do alarme quando todos os bits forem limpos.
- `TimerLED`: controle do periodo de piscada do LED conforme o alarme ativo.
- `EventGroupAlarmes`: sinalizacao dos estados de alarme entre as tarefas.
