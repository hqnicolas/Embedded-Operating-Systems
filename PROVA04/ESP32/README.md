# PROVA04 - Estacao de Trabalho IoT com FreeRTOS

## Visao Geral

Esta entrega implementa a `PROVA04` como uma estacao de trabalho inteligente baseada em `ESP32`, unindo:

- `FreeRTOS` com `tasks`, `queues`, `mutex`, `software timers`, `event groups` e `task notifications`;
- `MQTT + Node-RED + HTTP WebServer` como pilha oficial de IoT;
- a mesma placa, shield e pinagem da `Internet-of-Things/AULA08`.

A aplicacao monitora `temperatura`, `umidade` e `luminosidade`, controla um ciclo `FOCUS/PAUSE`, aciona uma luminaria por `rele`, publica telemetria por `MQTT`, mostra status em `HTTP` e aceita comando remoto pelo dashboard.

## Arquivos da Entrega

- `ESP32/PROVA04.ino`: firmware principal da prova com arquitetura explicita em `FreeRTOS`
- `fluxo-node-red.json`: dashboard e automacao base para o `Node-RED`
- `CONCEITO.md`: conceito do projeto
- `FLUXOGRAMA_HARDWARE.md`: fluxo macro de hardware
- `DEFINICOES_IOT.md`: glossario resumido de IoT

## Hardware e Pinagem

O kit continua sendo o mesmo da `Internet-of-Things/AULA08`.

| Componente | Funcao | GPIO |
| --- | --- | --- |
| `DHT11` | temperatura e umidade | `33` |
| `LDR` | luminosidade analogica | `39` |
| `Rele` | acionamento da luminaria | `13` |
| `LED 1` | foco ativo | `4` |
| `LED 2` | pausa ativa | `0` |
| `LED 3` | Wi-Fi + MQTT ok | `2` |
| `LED 4` | alerta ambiental | `15` |
| `Botao 1` | iniciar ou alternar `FOCUS/PAUSE` | `4` |
| `Botao 2` | reset para `IDLE` | `0` |
| `Botao 3` | trocar exibicao local | `2` |
| `Botao 4` | override manual da luminaria | `15` |
| `SEG_A` | display 7 segmentos | `18` |
| `SEG_B` | display 7 segmentos | `5` |
| `SEG_C` | display 7 segmentos | `21` |
| `SEG_D` | display 7 segmentos | `3` |
| `SEG_E` | display 7 segmentos | `1` |
| `SEG_F` | display 7 segmentos | `23` |
| `SEG_G` | display 7 segmentos | `22` |
| `SEG_DP` | display 7 segmentos | `19` |
| `DISPLAY_1` | digito 1 | `16` |
| `DISPLAY_2` | digito 2 | `17` |

Observacao importante:

- os GPIOs `4`, `0`, `2` e `15` continuam compartilhados entre `LEDs` e `botoes`;
- por isso a `TaskControle` foi mantida como dona exclusiva desses pinos, reutilizando a estrategia de multiplexacao das aulas de IoT.

## Arquitetura FreeRTOS Implementada

### Tasks

| Task | Responsabilidade |
| --- | --- |
| `TaskSensores` | acordada por timer, le `DHT11` e `LDR`, monta `SensorSnapshot` e envia para `qSensorSnapshots` |
| `TaskControle` | le botoes, consome filas, atualiza `SystemState`, aplica regra da luminaria, controla rele e LEDs |
| `TaskDisplay` | multiplexa o display de 7 segmentos continuamente |
| `TaskIoT` | mantem `Wi-Fi`, `MQTT`, `HTTP`, publica status e recebe comandos remotos |

### Primitivas do FreeRTOS

| Recurso | Uso no projeto |
| --- | --- |
| `Queue` | `qSensorSnapshots` e `qControlCommands` |
| `Mutex` | `mutexEstado` protege a struct `SystemState` |
| `Software Timer` | amostragem, tick do ciclo e publicacao MQTT |
| `Event Group` | `BIT_WIFI_OK`, `BIT_MQTT_OK`, `BIT_FOCUS_ACTIVE`, `BIT_ALERT_ACTIVE` |
| `Task Notification` | acorda `TaskSensores`, `TaskControle`, `TaskDisplay` e `TaskIoT` sem polling pesado |

### Tipos internos principais

- `SensorSnapshot { temperatureC, humidityPercent, ldrRaw, ldrPercent, dhtOk, timestampMs }`
- `ControlCommand { source, action, value, boolValue }`
- `SystemState { mode, displayMode, relayOn, manualOverrideEnabled, manualOverrideState, thresholds, focusDurationSeconds, pauseDurationSeconds, countdownSeconds, connectivity, lastSensors }`

## Comportamento Funcional

### Modos

- `IDLE`: sistema parado; o display mostra o modo selecionado pelo botao 3
- `FOCUS`: cronometro ativo; se a luminosidade ficar abaixo do limiar, a luminaria liga automaticamente
- `PAUSE`: cronometro de pausa ativo; a automacao da luz nao liga o rele

### Botoes locais

| Botao | Acao |
| --- | --- |
| `Botao 1` | alterna entre `FOCUS` e `PAUSE`; se estiver em `IDLE`, entra em `FOCUS` |
| `Botao 2` | reseta o sistema para `IDLE` e remove override manual |
| `Botao 3` | alterna a exibicao local entre `temperatura`, `umidade` e `luminosidade` |
| `Botao 4` | ativa ou desativa o override manual da luminaria |

### Display

- em `FOCUS` ou `PAUSE`, o display mostra o tempo restante;
- com os tempos padrao de demonstracao, o valor e exibido em `segundos`;
- se os tempos forem configurados acima de `99 s`, o display passa a mostrar `minutos`;
- em `IDLE`, o display mostra `temperatura`, `umidade` ou `luminosidade`, de acordo com o ultimo modo escolhido no botao 3.

### Regra local da luminaria

- se `manualOverrideEnabled = true`, o estado do rele segue o override manual;
- caso contrario, a regra automatica vale somente em `FOCUS`;
- a luminaria liga automaticamente quando `luminosidade < threshold` no modo `FOCUS`.

### Alertas ambientais

O firmware considera:

- `WARN` quando o `DHT11` nao esta valido;
- `ALERT` quando `temperatura > 30 C` ou `umidade < threshold`;
- `OK` nos demais casos.

## Configuracao Padrao

Os parametros ficam centralizados no topo de `ESP32/PROVA04.ino`.

Valores iniciais:

- `GROUP_ID = 7`
- `limite de luminosidade = 40%`
- `limite de umidade = 30%`
- `temperatura de alerta = 30 C`
- `FOCUS = 25 s`
- `PAUSE = 5 s`

Os tempos curtos sao intencionais para demonstracao da prova sem alterar a arquitetura.

## Interfaces de Comunicacao

### Topicos MQTT

| Funcao | Topico |
| --- | --- |
| temperatura | `satc/gX/telemetry/temperature` |
| umidade | `satc/gX/telemetry/humidity` |
| luminosidade | `satc/gX/telemetry/luminosity` |
| modo atual | `satc/gX/status/focus` |
| alerta ambiental | `satc/gX/status/alert` |
| comando da luminaria | `satc/gX/cmd/light` |

Substitua `gX` pelo numero real do grupo antes da apresentacao.

### Rotas HTTP

| Rota | Funcao |
| --- | --- |
| `/` | resumo humano do estado atual |
| `/status` | JSON completo do `SystemState` |
| `/config` | ajuste de `lux`, `humidity`, `focus` e `pause` |

Exemplo:

```text
/config?lux=45&humidity=35&focus=30&pause=10
```

Observacao:

- `focus` e `pause` sao configurados em `segundos`.

## Dashboard Node-RED

Arquivo:

- `fluxo-node-red.json`

O fluxo inclui:

- gauge de `temperatura`
- gauge de `umidade`
- grafico e texto de `luminosidade`
- texto de `modo`
- texto de `alerta`
- switch para comando `ON/OFF` da luminaria
- automacao simples: se `FOCUS` estiver ativo e `luminosidade < 40`, o dashboard tambem publica `ON`

## Como Executar

### 1. Preparar o firmware

1. Abra `ESP32/PROVA04.ino`.
2. Ajuste `EAP_IDENTITY`, `EAP_USERNAME` e `EAP_PASSWORD`.
3. Troque `GROUP_ID` se o grupo nao for `7`.
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
4. Abra o monitor serial em `115200`.

Observacao pratica:

- a Arduino IDE e o `arduino-cli` esperam que o nome da pasta do sketch combine com o nome do arquivo `.ino` principal;
- como a entrega foi organizada em `PROVA04/ESP32/PROVA04.ino`, a validacao de build foi feita a partir de uma copia temporaria em uma pasta `PROVA04`, sem alterar o codigo da entrega.

Na inicializacao, o firmware deve:

- criar `queues`, `mutex`, `timers` e `event group`;
- iniciar as quatro `tasks`;
- conectar ao `Wi-Fi Enterprise`;
- subir o `HTTP WebServer`;
- conectar ao `broker.hivemq.com:8883`;
- assinar `satc/gX/cmd/light`.

### 3. Importar o Node-RED

1. Abra o Node-RED.
2. Instale o pacote `@flowfuse/node-red-dashboard` se ainda nao estiver instalado.
3. Importe `fluxo-node-red.json`.
4. Ajuste os topicos se o grupo nao for `g7`.
5. Configure o broker `broker.hivemq.com` na porta `8883` com TLS habilitado.

### 4. Testar o HTTP

Com o ESP32 conectado, abra no navegador:

- `http://IP_DO_ESP32/`
- `http://IP_DO_ESP32/status`
- `http://IP_DO_ESP32/config?lux=40&humidity=30&focus=25&pause=5`

## Roteiro de Validacao da Prova

### FreeRTOS

- confirmar que `TaskSensores`, `TaskControle`, `TaskDisplay` e `TaskIoT` estao ativas
- confirmar uso explicito de `queue`, `mutex`, `software timers`, `event group` e `task notifications`
- mostrar no codigo onde cada recurso e usado

### Sensores e display

- variar a luz no `LDR` e verificar mudanca no display e no dashboard
- validar leitura de `temperatura` e `umidade`
- trocar exibicao local com o botao 3

### Ciclo de foco

- botao 1: entrar em `FOCUS`
- aguardar o fim do tempo e verificar troca automatica para `PAUSE`
- aguardar novamente e verificar retorno para `FOCUS`
- botao 2: reset para `IDLE`

### Luminaria

- em `FOCUS` com baixa luminosidade, o rele deve ligar
- fora de `FOCUS`, a regra automatica nao deve ligar o rele
- botao 4 deve ativar e desativar override manual
- `Node-RED` deve conseguir publicar `ON` e `OFF` em `satc/gX/cmd/light`

### Conectividade

- `BIT_WIFI_OK` deve subir quando o Wi-Fi conectar
- `BIT_MQTT_OK` deve subir quando o broker conectar
- `BIT_FOCUS_ACTIVE` deve refletir o modo `FOCUS`
- `BIT_ALERT_ACTIVE` deve refletir `WARN` ou `ALERT`

### HTTP e MQTT

- `/status` deve retornar JSON valido
- `/config` deve alterar thresholds e duracoes
- o dashboard deve mostrar os mesmos estados publicados pelo firmware

## Observacoes Finais

- a `AULA06` foi usada apenas como referencia de servico de rede; a prova continua baseada em `HTTP WebServer`, nao em `CoAP`
- a estrategia de polling controlado para botoes foi escolhida de proposito, porque os pinos sao compartilhados com `LEDs`
- a `Internet-of-Things/AULA08` foi mantida intacta; a `PROVA04` recebeu seu proprio sketch e seu proprio fluxo `Node-RED`
