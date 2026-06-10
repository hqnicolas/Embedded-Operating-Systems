# PROVA04 - Firmware ESP32

## Visao geral

Esta entrega implementa a `PROVA04` como uma estacao de trabalho inteligente baseada em `ESP32`, unindo:

- `FreeRTOS` com `tasks`, `queues`, `mutex`, `software timers`, `event groups` e `task notifications`;
- `MQTT + Node-RED + HTTP WebServer` como pilha oficial de IoT;
- a mesma placa, shield e pinagem da `Internet-of-Things/AULA08`.

A aplicacao monitora `temperatura`, `umidade` e `luminosidade`, controla um ciclo `FOCO/PAUSA`, aciona uma luminaria por `rele`, publica telemetria por `MQTT`, mostra estado em `HTTP` e aceita comando remoto pelo dashboard.

## Arquivos da entrega

- `PROVA04.ino`: firmware principal da prova com arquitetura explicita em `FreeRTOS`
- `../fluxo-node-red.json`: dashboard base para o `Node-RED`
- `../CONCEITO.md`: conceito do projeto
- `../FLUXOGRAMA_HARDWARE.md`: fluxo macro de hardware
- `../DEFINICOES_IOT.md`: glossario resumido de IoT

## Hardware e pinagem

| Componente | Funcao | GPIO |
| --- | --- | --- |
| `DHT11` | temperatura e umidade | `33` |
| `LDR` | luminosidade analogica | `39` |
| `Rele` | acionamento da luminaria | `13` |
| `LED 1` | foco ativo | `4` |
| `LED 2` | pausa ativa | `0` |
| `LED 3` | Wi-Fi + MQTT ok | `2` |
| `LED 4` | alerta ambiental | `15` |
| `Botao 1` | iniciar ou alternar `FOCO/PAUSA` | `4` |
| `Botao 2` | reset para `OCIOSO` | `0` |
| `Botao 3` | trocar exibicao local | `2` |
| `Botao 4` | controle manual da luminaria | `15` |
| `SEG_A` | display 7 segmentos | `18` |
| `SEG_B` | display 7 segmentos | `5` |
| `SEG_C` | display 7 segmentos | `21` |
| `SEG_D` | display 7 segmentos | `3` |
| `SEG_E` | display 7 segmentos | `1` |
| `SEG_F` | display 7 segmentos | `23` |
| `SEG_G` | display 7 segmentos | `22` |
| `SEG_DP` | display 7 segmentos | `19` |
| `DIGITO_1` | digito 1 | `17` |
| `DIGITO_2` | digito 2 | `16` |

Observacao importante:

- os GPIOs `4`, `0`, `2` e `15` continuam compartilhados entre `LEDs` e `botoes`;
- por isso a `tarefaControle` foi mantida como dona exclusiva desses pinos, reutilizando a estrategia de multiplexacao das aulas de IoT.

## Arquitetura FreeRTOS implementada

### Tarefas

| Tarefa | Responsabilidade |
| --- | --- |
| `tarefaSensores` | acordada por timer, le `DHT11` e `LDR`, monta `LeituraSensores` e envia para `qLeiturasSensores` |
| `tarefaControle` | le botoes, consome filas, atualiza `EstadoSistema`, aplica regra da luminaria, controla rele e LEDs |
| `tarefaDisplay` | multiplexa o display de 7 segmentos continuamente |
| `tarefaIoT` | mantem `Wi-Fi`, `MQTT`, `HTTP`, publica estado e recebe comandos remotos |

### Primitivas do FreeRTOS

| Recurso | Uso no projeto |
| --- | --- |
| `Queue` | `qLeiturasSensores` e `qComandosControle` |
| `Mutex` | `mutexEstado` protege a struct `EstadoSistema` |
| `Software Timer` | amostragem, tick do ciclo e publicacao MQTT |
| `Event Group` | `BIT_WIFI_OK`, `BIT_MQTT_OK`, `BIT_FOCO_ATIVO`, `BIT_ALERTA_ATIVO` |
| `Task Notification` | acorda `tarefaSensores`, `tarefaControle`, `tarefaDisplay` e `tarefaIoT` sem polling pesado |

### Tipos internos principais

- `LeituraSensores { temperaturaC, umidadePercentual, ldrBruto, ldrPercentual, dhtValido, instanteMs }`
- `ComandoControle { origem, acao, valor, valorBooleano }`
- `EstadoSistema { modo, modoExibicao, releLigado, controleManualAtivo, estadoControleManual, limiares, duracaoFocoSegundos, duracaoPausaSegundos, contagemRegressivaSegundos, conectividade, ultimaLeituraSensores }`

## Comportamento funcional

### Modos

- `OCIOSO`: sistema parado; o display mostra o modo de exibicao selecionado pelo botao 3
- `FOCO`: cronometro ativo; se a luminosidade ficar abaixo do limiar, a luminaria liga automaticamente
- `PAUSA`: cronometro de pausa ativo; a automacao da luz nao liga o rele

### Botoes locais

| Botao | Acao |
| --- | --- |
| `Botao 1` | alterna entre `FOCO` e `PAUSA`; se estiver em `OCIOSO`, entra em `FOCO` |
| `Botao 2` | reseta o sistema para `OCIOSO` e remove o controle manual |
| `Botao 3` | alterna a exibicao local entre `temperatura`, `umidade` e `luminosidade` |
| `Botao 4` | ativa ou desativa o controle manual da luminaria |

### Display

- em `FOCO` ou `PAUSA`, o display mostra o tempo restante;
- com os tempos padrao de demonstracao, o valor e exibido em `segundos`;
- se os tempos forem configurados acima de `99 s`, o display passa a mostrar `minutos`;
- em `OCIOSO`, o display mostra `temperatura`, `umidade` ou `luminosidade`, de acordo com o ultimo modo escolhido no botao 3.

### Regra local da luminaria

- se `controleManualAtivo = true`, o estado do rele segue `estadoControleManual`;
- caso contrario, a regra automatica vale somente em `FOCO`;
- a luminaria liga automaticamente quando `luminosidade < limiar` no modo `FOCO`.

### Alertas ambientais

O firmware considera:

- `AVISO` quando o `DHT11` nao esta valido;
- `ALERTA` quando `temperatura > 30 C` ou `umidade < limiar`;
- `OK` nos demais casos.

## Configuracao padrao

Os parametros ficam centralizados no topo de `PROVA04.ino`.

Valores iniciais:

- `ID_GRUPO = 7`
- `limiar de luminosidade = 40%`
- `limiar de umidade = 30%`
- `temperatura de alerta = 30 C`
- `FOCO = 25 s`
- `PAUSA = 5 s`

Os tempos curtos sao intencionais para demonstracao da prova sem alterar a arquitetura.

## Interfaces de comunicacao

### Topicos MQTT

| Funcao | Topico |
| --- | --- |
| temperatura | `satc/gX/telemetria/temperatura` |
| umidade | `satc/gX/telemetria/umidade` |
| luminosidade | `satc/gX/telemetria/luminosidade` |
| estado de foco | `satc/gX/estado/foco` |
| alerta ambiental | `satc/gX/estado/alerta` |
| comando da luminaria | `satc/gX/comando/luz` |

Substitua `gX` pelo numero real do grupo antes da apresentacao.

### Rotas HTTP

| Rota | Funcao |
| --- | --- |
| `/` | resumo humano do estado atual |
| `/estado` | JSON completo do `EstadoSistema` |
| `/configuracao` | ajuste de `luminosidade`, `umidade`, `foco` e `pausa` |

Exemplo:

```text
/configuracao?luminosidade=45&umidade=35&foco=30&pausa=10
```

### Payloads MQTT de comando

- `LIGAR`
- `DESLIGAR`

## Conectividade Wi-Fi

O firmware tenta:

1. `Nicolas` como rede primaria comum;
2. `SATC 2.4` como rede backup enterprise;
3. novo ciclo sempre reiniciado pela rede primaria em caso de perda de conexao.

## Dashboard Node-RED

Arquivo:

- `../fluxo-node-red.json`

O fluxo inclui:

- gauge de `temperatura`
- gauge de `umidade`
- grafico e texto de `luminosidade`
- texto de `estado de foco`
- texto de `alerta`
- switch manual para comando `LIGAR/DESLIGAR` da luminaria
- automacao concentrada no `ESP32`, evitando republicar `comando/luz` a cada atualizacao de sensor ou status

## Como executar

### 1. Preparar o firmware

1. Abra `PROVA04.ino`.
2. Ajuste `IDENTIDADE_EAP`, `USUARIO_EAP` e `SENHA_EAP`.
3. Troque `ID_GRUPO` se o grupo nao for `7`.
4. Confira as bibliotecas:
   - `WiFi.h`
   - `WiFiClientSecure.h`
   - `PubSubClient.h`
   - `WebServer.h`
   - `DFRobot_DHT11.h`

### 2. Gravar no ESP32

1. Selecione a placa ESP32 correta.
2. Compile o sketch.
3. Grave o firmware.

Na inicializacao, o firmware deve:

- criar filas, mutex, timers e `event group`;
- iniciar as quatro tarefas;
- tentar o Wi-Fi `Nicolas` e usar `SATC 2.4` como fallback;
- subir o `HTTP WebServer`;
- conectar ao `broker.hivemq.com:8883`;
- assinar `satc/gX/comando/luz`.

### 3. Importar o Node-RED

1. Abra o Node-RED.
2. Instale o pacote `@flowfuse/node-red-dashboard` se ainda nao estiver instalado.
3. Importe `fluxo-node-red.json`.
4. Ajuste os topicos se o grupo nao for `g7`.
5. Configure o broker `broker.hivemq.com` na porta `8883` com TLS habilitado.

### 4. Testar o HTTP

Com o ESP32 conectado, abra no navegador:

- `http://IP_DO_ESP32/`
- `http://IP_DO_ESP32/estado`
- `http://IP_DO_ESP32/configuracao?luminosidade=40&umidade=30&foco=25&pausa=5`

## Roteiro de validacao da prova

### FreeRTOS

- confirmar que `tarefaSensores`, `tarefaControle`, `tarefaDisplay` e `tarefaIoT` estao ativas;
- confirmar uso explicito de `queue`, `mutex`, `software timers`, `event group` e `task notifications`;
- mostrar no codigo onde cada recurso e usado.

### Sensores e display

- variar a luz no `LDR` e verificar mudanca no display e no dashboard;
- validar leitura de `temperatura` e `umidade`;
- trocar exibicao local com o botao 3.

### Ciclo de foco

- botao 1: entrar em `FOCO`;
- aguardar o fim do tempo e verificar troca automatica para `PAUSA`;
- aguardar novamente e verificar retorno para `FOCO`;
- botao 2: reset para `OCIOSO`.

### Luminaria

- em `FOCO` com baixa luminosidade, o rele deve ligar;
- fora de `FOCO`, a regra automatica nao deve ligar o rele;
- botao 4 deve ativar e desativar o controle manual;
- `Node-RED` deve conseguir publicar `LIGAR` e `DESLIGAR` em `satc/gX/comando/luz`.

### Conectividade

- `BIT_WIFI_OK` deve subir quando o Wi-Fi conectar;
- `BIT_MQTT_OK` deve subir quando o broker conectar;
- `BIT_FOCO_ATIVO` deve refletir o modo `FOCO`;
- `BIT_ALERTA_ATIVO` deve refletir `AVISO` ou `ALERTA`.

### HTTP e MQTT

- `/estado` deve retornar JSON valido;
- `/configuracao` deve alterar limiares e duracoes;
- o dashboard deve mostrar os mesmos estados publicados pelo firmware.
