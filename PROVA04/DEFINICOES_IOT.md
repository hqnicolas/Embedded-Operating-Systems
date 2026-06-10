# Definicoes de IoT

## Objetivo deste arquivo

Este glossario resume os principais conceitos de `IoT` usados no projeto **Estacao de Trabalho Inteligente IoT**.

## Conceitos principais

| Termo | Definicao | Aplicacao no projeto |
| --- | --- | --- |
| `IoT` | Internet das Coisas. Area em que dispositivos fisicos coletam dados, se conectam em rede e interagem com outros sistemas. | O `ESP32` coleta dados da mesa, envia para a rede e recebe comandos remotos. |
| `Telemetria` | Envio periodico de dados medidos por um dispositivo para outro sistema. | Temperatura, umidade e luminosidade sao enviadas ao dashboard. |
| `Dispositivo de borda` | Equipamento que fica proximo do ambiente fisico, coletando e processando dados localmente antes de enviar para a rede. | O `ESP32` e o dispositivo de borda do projeto. |
| `MQTT` | Protocolo leve de mensagens para IoT, baseado em `publish/subscribe`. | Usado para publicar leituras e receber comandos da luminaria. |
| `Broker MQTT` | Servidor intermediario que recebe mensagens publicadas e distribui para os clientes inscritos. | Interliga `ESP32` e `Node-RED`. |
| `Publish/Subscribe` | Modelo em que um dispositivo publica dados em topicos e outros dispositivos recebem se estiverem inscritos nesses topicos. | O `ESP32` publica telemetria e o `Node-RED` consome esses dados. |
| `Dashboard` | Painel visual para monitoramento e interacao com os dados do sistema. | O `Node-RED` exibe sensores, foco e alerta. |
| `Node-RED` | Ferramenta visual de integracao por fluxos, muito usada em IoT para dashboards, automacoes e integracao entre servicos. | Mostra os dados, envia comandos e pode aplicar regras. |
| `HTTP WebServer` | Servidor web embutido no dispositivo para responder requisicoes locais. | O `ESP32` disponibiliza as rotas `/`, `/estado` e `/configuracao`. |
| `Automacao` | Acao executada pelo sistema com base em uma regra predefinida, sem depender de comando manual a cada evento. | Quando a luminosidade cai no modo `FOCO`, a luminaria pode ser ligada automaticamente. |
| `Controle remoto` | Capacidade de enviar comandos ao dispositivo a distancia, pela rede. | O dashboard pode enviar `LIGAR` e `DESLIGAR` para a luminaria. |

## Stack oficial do projeto

O projeto adota a seguinte pilha de IoT:

- `ESP32` como dispositivo de borda;
- `MQTT` como protocolo principal de troca de mensagens;
- `Node-RED` como dashboard e camada de automacao;
- `HTTP WebServer` como interface local de consulta.

## Por que usar MQTT neste projeto

`MQTT` foi escolhido porque:

- e leve para o `ESP32`;
- funciona bem com telemetria frequente;
- facilita controle bidirecional;
- integra facilmente com `Node-RED`;
- combina melhor com dashboard em tempo real do que uma solucao apenas baseada em pagina web local.

## Como esses conceitos aparecem na pratica

### Publicacao de dados

O `ESP32` publica:

- `satc/gX/telemetria/temperatura`
- `satc/gX/telemetria/umidade`
- `satc/gX/telemetria/luminosidade`
- `satc/gX/estado/foco`
- `satc/gX/estado/alerta`

### Recebimento de comandos

O `ESP32` assina:

- `satc/gX/comando/luz`

Assim, o dashboard pode enviar `LIGAR` e `DESLIGAR` para a luminaria.

### Consulta local

Sem depender do broker, o usuario ainda pode consultar o dispositivo pelas rotas:

- `/`
- `/estado`
- `/configuracao`

### Conectividade Wi-Fi

O firmware tenta primeiro a rede comum `Nicolas`. Se nao houver sucesso, usa `SATC 2.4` como acesso backup com autenticacao enterprise.

## Resumo

Neste projeto, `IoT` nao significa apenas "colocar um sensor na internet". Aqui, a ideia completa e:

- medir o ambiente;
- processar localmente;
- comunicar pela rede;
- visualizar em dashboard;
- receber comando remoto;
- manter uma interface local no proprio dispositivo.
