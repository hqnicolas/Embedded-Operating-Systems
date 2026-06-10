#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <WebServer.h>
#include <DFRobot_DHT11.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"

const char *SSID_WIFI_PRIMARIO = "Nicolas";
const char *SENHA_WIFI_PRIMARIO = "0x123456";
const char *SSID_WIFI_BACKUP = "SATC 2.4";
const char *IDENTIDADE_EAP = "aluno.123456@alunosatc.edu.br";
const char *USUARIO_EAP = "aluno.123456@alunosatc.edu.br";
const char *SENHA_EAP = "123456";

const char *HOST_MQTT = "broker.hivemq.com";
constexpr int PORTA_MQTT = 8883;
const char *USUARIO_MQTT = "";
const char *SENHA_MQTT = "";
constexpr uint16_t SEGUNDOS_KEEP_ALIVE_MQTT = 60;
constexpr uint16_t SEGUNDOS_TIMEOUT_SOCKET_MQTT = 20;

constexpr int ID_GRUPO = 7;

#define RELE 13
#define SEG_A 18
#define SEG_B 5
#define SEG_C 21
#define SEG_D 3
#define SEG_E 1
#define SEG_F 23
#define SEG_G 22
#define SEG_DP 19
#define DIGITO_1 17
#define DIGITO_2 16
#define PINO_DHT11 33
#define PINO_LDR 39

#define LED1 4
#define LED2 0
#define LED3 2
#define LED4 15
#define BOTAO1 4
#define BOTAO2 0
#define BOTAO3 2
#define BOTAO4 15

constexpr bool COMPARTILHAR_PINOS_LEDS_COM_BOTOES = true;
constexpr bool DISPLAY_SEGMENTO_ATIVO_ALTO = true;
constexpr bool DISPLAY_DIGITO_ATIVO_BAIXO = true;

constexpr unsigned long WIFI_TIMEOUT_CONEXAO_MS = 20000;
constexpr unsigned long WIFI_INTERVALO_RECONEXAO_MS = 5000;
constexpr unsigned long MQTT_INTERVALO_RECONEXAO_MS = 5000;
constexpr unsigned long BOTAO_DEBOUNCE_MS = 60;
constexpr unsigned long TEMPO_SAIDA_PINO_COMPARTILHADO_US = 1500;

constexpr uint32_t INTERVALO_AMOSTRAGEM_SENSORES_MS = 1500;
constexpr uint32_t INTERVALO_PUBLICACAO_TELEMETRIA_MS = 4000;
constexpr uint32_t INTERVALO_LOOP_CONTROLE_MS = 5;
constexpr uint32_t INTERVALO_RETENCAO_DIGITO_MS = 2;
constexpr uint32_t INTERVALO_LOOP_IOT_MS = 20;

constexpr int LIMIAR_PADRAO_LUMINOSIDADE = 40;
constexpr int LIMIAR_PADRAO_ALERTA_UMIDADE = 30;
constexpr int TEMPERATURA_PADRAO_ALERTA_C = 30;
constexpr uint32_t DURACAO_PADRAO_FOCO_SEGUNDOS = 25;
constexpr uint32_t DURACAO_PADRAO_PAUSA_SEGUNDOS = 5;

constexpr size_t TAMANHO_FILA_SENSORES = 1;
constexpr size_t TAMANHO_FILA_CONTROLE = 10;

constexpr uint32_t NOTIFICACAO_AMOSTRAGEM_SENSORES = 1UL << 0;
constexpr uint32_t NOTIFICACAO_TICK_CICLO_CONTROLE = 1UL << 0;
constexpr uint32_t NOTIFICACAO_DADOS_SENSORES_CONTROLE = 1UL << 1;
constexpr uint32_t NOTIFICACAO_COMANDO_REMOTO_CONTROLE = 1UL << 2;
constexpr uint32_t NOTIFICACAO_CONECTIVIDADE_CONTROLE = 1UL << 3;
constexpr uint32_t NOTIFICACAO_ATUALIZAR_DISPLAY = 1UL << 0;
constexpr uint32_t NOTIFICACAO_PUBLICAR_IOT = 1UL << 0;
constexpr uint32_t NOTIFICACAO_ESTADO_ALTERADO_IOT = 1UL << 1;

constexpr EventBits_t BIT_WIFI_OK = 1UL << 0;
constexpr EventBits_t BIT_MQTT_OK = 1UL << 1;
constexpr EventBits_t BIT_FOCO_ATIVO = 1UL << 2;
constexpr EventBits_t BIT_ALERTA_ATIVO = 1UL << 3;

enum ModoFoco : uint8_t {
  MODO_OCIOSO = 0,
  MODO_FOCO,
  MODO_PAUSA
};

enum ModoExibicao : uint8_t {
  EXIBICAO_TEMPERATURA = 0,
  EXIBICAO_UMIDADE,
  EXIBICAO_LUMINOSIDADE
};

enum EstadoAlerta : uint8_t {
  ALERTA_OK = 0,
  ALERTA_AVISO,
  ALERTA_ATIVO
};

enum PerfilWifi : uint8_t {
  PERFIL_WIFI_NENHUM = 0,
  PERFIL_WIFI_PRIMARIO,
  PERFIL_WIFI_BACKUP
};

enum OrigemControle : uint8_t {
  ORIGEM_MQTT = 0,
  ORIGEM_HTTP,
  ORIGEM_LOCAL
};

enum AcaoControle : uint8_t {
  ACAO_DEFINIR_LUZ = 0,
  ACAO_DEFINIR_LIMIAR_LUMINOSIDADE,
  ACAO_DEFINIR_LIMIAR_UMIDADE,
  ACAO_DEFINIR_SEGUNDOS_FOCO,
  ACAO_DEFINIR_SEGUNDOS_PAUSA
};

struct LeituraSensores {
  float temperaturaC;
  float umidadePercentual;
  int ldrBruto;
  int ldrPercentual;
  bool dhtValido;
  unsigned long instanteMs;
};

struct Limiares {
  int luminosidadePercentual;
  int umidadePercentual;
};

struct EstadoConectividade {
  bool wifiConectado;
  bool mqttConectado;
};

struct EstadoSistema {
  ModoFoco modo;
  ModoExibicao modoExibicao;
  bool releLigado;
  bool controleManualAtivo;
  bool estadoControleManual;
  Limiares limiares;
  uint32_t duracaoFocoSegundos;
  uint32_t duracaoPausaSegundos;
  uint32_t contagemRegressivaSegundos;
  EstadoConectividade conectividade;
  LeituraSensores ultimaLeituraSensores;
};

struct ComandoControle {
  OrigemControle origem;
  AcaoControle acao;
  int valor;
  bool valorBooleano;
};

DFRobot_DHT11 sensorDht;
WiFiClientSecure clienteEsp;
PubSubClient clienteMqtt(clienteEsp);
WebServer servidor(80);

QueueHandle_t qLeiturasSensores = NULL;
QueueHandle_t qComandosControle = NULL;
SemaphoreHandle_t mutexEstado = NULL;
EventGroupHandle_t gEventos = NULL;

TimerHandle_t tmrAmostragem = NULL;
TimerHandle_t tmrTickCiclo = NULL;
TimerHandle_t tmrPublicacao = NULL;

TaskHandle_t handleTarefaSensores = NULL;
TaskHandle_t handleTarefaControle = NULL;
TaskHandle_t handleTarefaDisplay = NULL;
TaskHandle_t handleTarefaIoT = NULL;

EstadoSistema gEstadoSistema;

const int PINOS_LEDS[4] = {LED1, LED2, LED3, LED4};
const int PINOS_BOTOES[4] = {BOTAO1, BOTAO2, BOTAO3, BOTAO4};
const int PINOS_SEGMENTOS[8] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_DP};
const int PINOS_DIGITOS[2] = {DIGITO_1, DIGITO_2};

const byte PADROES_DIGITOS[10][8] = {
  {1, 1, 1, 1, 1, 1, 0, 0},
  {0, 1, 1, 0, 0, 0, 0, 0},
  {1, 1, 0, 1, 1, 0, 1, 0},
  {1, 1, 1, 1, 0, 0, 1, 0},
  {0, 1, 1, 0, 0, 1, 1, 0},
  {1, 0, 1, 1, 0, 1, 1, 0},
  {1, 0, 1, 1, 1, 1, 1, 0},
  {1, 1, 1, 0, 0, 0, 0, 0},
  {1, 1, 1, 1, 1, 1, 1, 0},
  {1, 1, 1, 1, 0, 1, 1, 0}
};

bool estadosLeds[4] = {false, false, false, false};
bool estadosEstaveisBotoes[4] = {HIGH, HIGH, HIGH, HIGH};
bool ultimasLeiturasBotoes[4] = {HIGH, HIGH, HIGH, HIGH};
unsigned long ultimosDebouncesBotoesMs[4] = {0, 0, 0, 0};

bool rotasHttpConfiguradas = false;
bool servidorHttpIniciado = false;
PerfilWifi perfilWifiAtivo = PERFIL_WIFI_NENHUM;
unsigned long tentativaWifiIniciadaMs = 0;
unsigned long ultimoCicloReconexaoWifiMs = 0;
unsigned long ultimaTentativaReconexaoMqttMs = 0;

char topicoTemperatura[48];
char topicoUmidade[48];
char topicoLuminosidade[48];
char topicoComandoLuz[48];
char topicoEstadoFoco[48];
char topicoEstadoAlerta[48];

String ultimoFocoPublicado = "";
String ultimoAlertaPublicado = "";

void falhaSegura(const char *mensagem);
void montarTopicos();
void inicializarEstadoSistema();
void inicializarHardwareDisplay();
void inicializarBotoesELeds();
void configurarPinosCompartilhadosComoEntrada();
void atualizarSaidasCompartilhadasLeds();
void atualizarCacheSaidasComEstado(const EstadoSistema &estado);
void aplicarNivelSegmento(int pino, bool ativo);
void aplicarNivelDigito(int pino, bool ativo);
void desabilitarDisplays();
void escreverPadraoDigito(int valor, bool pontoDecimal);
int calcularValorExibicao(const EstadoSistema &estado);
void multiplexarDisplay(const EstadoSistema &estado);
void copiarEstadoSistema(EstadoSistema &destino);
const char *modoFocoParaTexto(ModoFoco modo);
const char *modoExibicaoParaTexto(ModoExibicao modo);
EstadoAlerta calcularEstadoAlerta(const EstadoSistema &estado);
const char *estadoAlertaParaTexto(EstadoAlerta estadoAlerta);
bool payloadSignificaLigar(const String &payload);
bool payloadSignificaDesligar(const String &payload);
void sincronizarEstadoConectividade();
void sincronizarBitsEventosEstado(const EstadoSistema &estado);
void notificarConsumidoresEstado();
void atualizarAtuadoresComEstado();
void aplicarDecisaoIluminacaoTravado();
void definirModoTravado(ModoFoco proximoModo);
bool enfileirarComandoControle(const ComandoControle &comando, TickType_t ticksDeEspera);
String montarJsonEstado();
String montarPaginaHtml();
void configurarRotasHttp();
void iniciarServidorHttp();
void tratarRaiz();
void tratarEstado();
void tratarConfiguracao();
void tratarNaoEncontrado();
void limparTentativaConexaoWifi();
void iniciarConexaoWifi(PerfilWifi perfil);
bool aguardarWifi(unsigned long timeoutMs);
void manterConexaoWifi();
bool conectarBrokerMqtt();
void manterConexaoMqtt();
void inscreverTopicos();
void callbackMqtt(char *topico, byte *payload, unsigned int comprimento);
void publicarLeituraTelemetria(const EstadoSistema &estado);
void publicarResumoStatus(const EstadoSistema &estado, bool forcar);
void publicarEstadoNoMqtt(bool publicarTelemetriaAgora, bool forcarStatus);
void processarPressionamentoBotao(int indiceBotao);
void processarBotoes();
void tratarLeituraSensores(const LeituraSensores &leitura);
void tratarComandoControle(const ComandoControle &comando);
void tratarTickCiclo();
void atualizarSaidasControle(bool notificarConsumidoresAgora);
void callbackTimerAmostragem(TimerHandle_t xTimer);
void callbackTimerCiclo(TimerHandle_t xTimer);
void callbackTimerPublicacao(TimerHandle_t xTimer);
void tarefaSensores(void *pvParameters);
void tarefaControle(void *pvParameters);
void tarefaDisplay(void *pvParameters);
void tarefaIoT(void *pvParameters);

void falhaSegura(const char *mensagem) {
  (void) mensagem;

  while (true) {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void montarTopicos() {
  snprintf(topicoTemperatura, sizeof(topicoTemperatura), "satc/g%d/telemetria/temperatura", ID_GRUPO);
  snprintf(topicoUmidade, sizeof(topicoUmidade), "satc/g%d/telemetria/umidade", ID_GRUPO);
  snprintf(topicoLuminosidade, sizeof(topicoLuminosidade), "satc/g%d/telemetria/luminosidade", ID_GRUPO);
  snprintf(topicoComandoLuz, sizeof(topicoComandoLuz), "satc/g%d/comando/luz", ID_GRUPO);
  snprintf(topicoEstadoFoco, sizeof(topicoEstadoFoco), "satc/g%d/estado/foco", ID_GRUPO);
  snprintf(topicoEstadoAlerta, sizeof(topicoEstadoAlerta), "satc/g%d/estado/alerta", ID_GRUPO);
}

void inicializarEstadoSistema() {
  gEstadoSistema.modo = MODO_OCIOSO;
  gEstadoSistema.modoExibicao = EXIBICAO_TEMPERATURA;
  gEstadoSistema.releLigado = false;
  gEstadoSistema.controleManualAtivo = false;
  gEstadoSistema.estadoControleManual = false;
  gEstadoSistema.limiares.luminosidadePercentual = LIMIAR_PADRAO_LUMINOSIDADE;
  gEstadoSistema.limiares.umidadePercentual = LIMIAR_PADRAO_ALERTA_UMIDADE;
  gEstadoSistema.duracaoFocoSegundos = DURACAO_PADRAO_FOCO_SEGUNDOS;
  gEstadoSistema.duracaoPausaSegundos = DURACAO_PADRAO_PAUSA_SEGUNDOS;
  gEstadoSistema.contagemRegressivaSegundos = DURACAO_PADRAO_FOCO_SEGUNDOS;
  gEstadoSistema.conectividade.wifiConectado = false;
  gEstadoSistema.conectividade.mqttConectado = false;
  gEstadoSistema.ultimaLeituraSensores.temperaturaC = 0.0f;
  gEstadoSistema.ultimaLeituraSensores.umidadePercentual = 0.0f;
  gEstadoSistema.ultimaLeituraSensores.ldrBruto = 0;
  gEstadoSistema.ultimaLeituraSensores.ldrPercentual = 0;
  gEstadoSistema.ultimaLeituraSensores.dhtValido = false;
  gEstadoSistema.ultimaLeituraSensores.instanteMs = 0;
}

void inicializarHardwareDisplay() {
  for (int i = 0; i < 8; i++) {
    pinMode(PINOS_SEGMENTOS[i], OUTPUT);
    aplicarNivelSegmento(PINOS_SEGMENTOS[i], false);
  }

  for (int i = 0; i < 2; i++) {
    pinMode(PINOS_DIGITOS[i], OUTPUT);
    aplicarNivelDigito(PINOS_DIGITOS[i], false);
  }

  desabilitarDisplays();
}

void inicializarBotoesELeds() {
  for (int i = 0; i < 4; i++) {
    if (COMPARTILHAR_PINOS_LEDS_COM_BOTOES) {
      pinMode(PINOS_BOTOES[i], INPUT_PULLUP);
      ultimasLeiturasBotoes[i] = digitalRead(PINOS_BOTOES[i]);
      estadosEstaveisBotoes[i] = ultimasLeiturasBotoes[i];
      estadosLeds[i] = false;
    } else {
      pinMode(PINOS_LEDS[i], OUTPUT);
      digitalWrite(PINOS_LEDS[i], LOW);
    }
  }

  if (COMPARTILHAR_PINOS_LEDS_COM_BOTOES) {
    atualizarSaidasCompartilhadasLeds();
  }
}

void configurarPinosCompartilhadosComoEntrada() {
  for (int i = 0; i < 4; i++) {
    pinMode(PINOS_BOTOES[i], INPUT_PULLUP);
  }
}

void atualizarSaidasCompartilhadasLeds() {
  if (!COMPARTILHAR_PINOS_LEDS_COM_BOTOES) {
    return;
  }

  for (int i = 0; i < 4; i++) {
    pinMode(PINOS_LEDS[i], OUTPUT);
    digitalWrite(PINOS_LEDS[i], estadosLeds[i] ? HIGH : LOW);
  }

  delayMicroseconds(TEMPO_SAIDA_PINO_COMPARTILHADO_US);
  configurarPinosCompartilhadosComoEntrada();
}

void atualizarCacheSaidasComEstado(const EstadoSistema &estado) {
  estadosLeds[0] = estado.modo == MODO_FOCO;
  estadosLeds[1] = estado.modo == MODO_PAUSA;
  estadosLeds[2] = estado.conectividade.wifiConectado && estado.conectividade.mqttConectado;
  estadosLeds[3] = calcularEstadoAlerta(estado) != ALERTA_OK;
}

void aplicarNivelSegmento(int pino, bool ativo) {
  digitalWrite(pino, ativo == DISPLAY_SEGMENTO_ATIVO_ALTO ? HIGH : LOW);
}

void aplicarNivelDigito(int pino, bool ativo) {
  bool nivel = ativo ? !DISPLAY_DIGITO_ATIVO_BAIXO : DISPLAY_DIGITO_ATIVO_BAIXO;
  digitalWrite(pino, nivel ? HIGH : LOW);
}

void desabilitarDisplays() {
  for (int i = 0; i < 2; i++) {
    aplicarNivelDigito(PINOS_DIGITOS[i], false);
  }
}

void escreverPadraoDigito(int valor, bool pontoDecimal) {
  if (valor < 0 || valor > 9) {
    for (int i = 0; i < 8; i++) {
      aplicarNivelSegmento(PINOS_SEGMENTOS[i], false);
    }
    return;
  }

  for (int i = 0; i < 7; i++) {
    aplicarNivelSegmento(PINOS_SEGMENTOS[i], PADROES_DIGITOS[valor][i] == 1);
  }

  aplicarNivelSegmento(SEG_DP, pontoDecimal);
}

int calcularValorExibicao(const EstadoSistema &estado) {
  if (estado.modo == MODO_FOCO || estado.modo == MODO_PAUSA) {
    uint32_t maiorDuracao = max(estado.duracaoFocoSegundos, estado.duracaoPausaSegundos);

    if (maiorDuracao <= 99) {
      return constrain(static_cast<int>(estado.contagemRegressivaSegundos), 0, 99);
    }

    uint32_t minutosRestantes = (estado.contagemRegressivaSegundos + 59UL) / 60UL;
    return constrain(static_cast<int>(minutosRestantes), 0, 99);
  }

  switch (estado.modoExibicao) {
    case EXIBICAO_UMIDADE:
      return constrain(static_cast<int>(round(estado.ultimaLeituraSensores.umidadePercentual)), 0, 99);
    case EXIBICAO_LUMINOSIDADE:
      return constrain(estado.ultimaLeituraSensores.ldrPercentual, 0, 99);
    case EXIBICAO_TEMPERATURA:
    default:
      return constrain(static_cast<int>(round(estado.ultimaLeituraSensores.temperaturaC)), 0, 99);
  }
}

void multiplexarDisplay(const EstadoSistema &estado) {
  static int digitoAtivo = 0;

  const int valorExibicao = calcularValorExibicao(estado);
  const int dezenas = valorExibicao / 10;
  const int unidades = valorExibicao % 10;

  desabilitarDisplays();

  if (digitoAtivo == 0) {
    escreverPadraoDigito(dezenas, false);
    aplicarNivelDigito(DIGITO_1, true);
  } else {
    escreverPadraoDigito(unidades, false);
    aplicarNivelDigito(DIGITO_2, true);
  }

  digitoAtivo = 1 - digitoAtivo;
  vTaskDelay(pdMS_TO_TICKS(INTERVALO_RETENCAO_DIGITO_MS));
}

void copiarEstadoSistema(EstadoSistema &destino) {
  xSemaphoreTake(mutexEstado, portMAX_DELAY);
  destino = gEstadoSistema;
  xSemaphoreGive(mutexEstado);
}

const char *modoFocoParaTexto(ModoFoco modo) {
  switch (modo) {
    case MODO_FOCO:
      return "FOCO";
    case MODO_PAUSA:
      return "PAUSA";
    case MODO_OCIOSO:
    default:
      return "OCIOSO";
  }
}

const char *modoExibicaoParaTexto(ModoExibicao modo) {
  switch (modo) {
    case EXIBICAO_UMIDADE:
      return "UMIDADE";
    case EXIBICAO_LUMINOSIDADE:
      return "LUMINOSIDADE";
    case EXIBICAO_TEMPERATURA:
    default:
      return "TEMPERATURA";
  }
}

EstadoAlerta calcularEstadoAlerta(const EstadoSistema &estado) {
  if (!estado.ultimaLeituraSensores.dhtValido) {
    return ALERTA_AVISO;
  }

  if (estado.ultimaLeituraSensores.temperaturaC > TEMPERATURA_PADRAO_ALERTA_C ||
      estado.ultimaLeituraSensores.umidadePercentual < estado.limiares.umidadePercentual) {
    return ALERTA_ATIVO;
  }

  return ALERTA_OK;
}

const char *estadoAlertaParaTexto(EstadoAlerta estadoAlerta) {
  switch (estadoAlerta) {
    case ALERTA_AVISO:
      return "AVISO";
    case ALERTA_ATIVO:
      return "ALERTA";
    case ALERTA_OK:
    default:
      return "OK";
  }
}

bool payloadSignificaLigar(const String &payload) {
  return payload.equalsIgnoreCase("LIGAR") ||
         payload.equalsIgnoreCase("LIGADO") ||
         payload.equalsIgnoreCase("1") ||
         payload.equalsIgnoreCase("VERDADEIRO");
}

bool payloadSignificaDesligar(const String &payload) {
  return payload.equalsIgnoreCase("DESLIGAR") ||
         payload.equalsIgnoreCase("DESLIGADO") ||
         payload.equalsIgnoreCase("0") ||
         payload.equalsIgnoreCase("FALSO");
}

void sincronizarEstadoConectividade() {
  const bool wifiConectado = WiFi.status() == WL_CONNECTED;
  const bool mqttConectado = wifiConectado && clienteMqtt.connected();
  bool houveMudanca = false;

  xSemaphoreTake(mutexEstado, portMAX_DELAY);

  if (gEstadoSistema.conectividade.wifiConectado != wifiConectado ||
      gEstadoSistema.conectividade.mqttConectado != mqttConectado) {
    gEstadoSistema.conectividade.wifiConectado = wifiConectado;
    gEstadoSistema.conectividade.mqttConectado = mqttConectado;
    houveMudanca = true;
  }

  xSemaphoreGive(mutexEstado);

  if (wifiConectado) {
    xEventGroupSetBits(gEventos, BIT_WIFI_OK);
  } else {
    xEventGroupClearBits(gEventos, BIT_WIFI_OK);
  }

  if (mqttConectado) {
    xEventGroupSetBits(gEventos, BIT_MQTT_OK);
  } else {
    xEventGroupClearBits(gEventos, BIT_MQTT_OK);
  }

  if (houveMudanca && handleTarefaControle != NULL) {
    xTaskNotify(handleTarefaControle, NOTIFICACAO_CONECTIVIDADE_CONTROLE, eSetBits);
  }
}

void sincronizarBitsEventosEstado(const EstadoSistema &estado) {
  if (estado.modo == MODO_FOCO) {
    xEventGroupSetBits(gEventos, BIT_FOCO_ATIVO);
  } else {
    xEventGroupClearBits(gEventos, BIT_FOCO_ATIVO);
  }

  if (calcularEstadoAlerta(estado) == ALERTA_OK) {
    xEventGroupClearBits(gEventos, BIT_ALERTA_ATIVO);
  } else {
    xEventGroupSetBits(gEventos, BIT_ALERTA_ATIVO);
  }
}

void notificarConsumidoresEstado() {
  if (handleTarefaDisplay != NULL) {
    xTaskNotify(handleTarefaDisplay, NOTIFICACAO_ATUALIZAR_DISPLAY, eSetBits);
  }

  if (handleTarefaIoT != NULL) {
    xTaskNotify(handleTarefaIoT, NOTIFICACAO_ESTADO_ALTERADO_IOT, eSetBits);
  }
}

void atualizarAtuadoresComEstado() {
  EstadoSistema estado;
  copiarEstadoSistema(estado);

  digitalWrite(RELE, estado.releLigado ? HIGH : LOW);
  atualizarCacheSaidasComEstado(estado);
  sincronizarBitsEventosEstado(estado);
}

void aplicarDecisaoIluminacaoTravado() {
  bool releAutomatico =
    gEstadoSistema.modo == MODO_FOCO &&
    gEstadoSistema.ultimaLeituraSensores.ldrPercentual < gEstadoSistema.limiares.luminosidadePercentual;

  if (gEstadoSistema.controleManualAtivo) {
    gEstadoSistema.releLigado = gEstadoSistema.estadoControleManual;
  } else {
    gEstadoSistema.releLigado = releAutomatico;
  }
}

void definirModoTravado(ModoFoco proximoModo) {
  gEstadoSistema.modo = proximoModo;

  switch (proximoModo) {
    case MODO_FOCO:
      gEstadoSistema.contagemRegressivaSegundos = gEstadoSistema.duracaoFocoSegundos;
      break;
    case MODO_PAUSA:
      gEstadoSistema.contagemRegressivaSegundos = gEstadoSistema.duracaoPausaSegundos;
      break;
    case MODO_OCIOSO:
    default:
      gEstadoSistema.contagemRegressivaSegundos = gEstadoSistema.duracaoFocoSegundos;
      break;
  }
}

bool enfileirarComandoControle(const ComandoControle &comando, TickType_t ticksDeEspera) {
  if (qComandosControle == NULL) {
    return false;
  }

  return xQueueSend(qComandosControle, &comando, ticksDeEspera) == pdPASS;
}

String montarJsonEstado() {
  EstadoSistema estado;
  copiarEstadoSistema(estado);
  const EventBits_t bitsEventos = xEventGroupGetBits(gEventos);

  String jsonEstado = "{";
  jsonEstado += "\"modo\":\"";
  jsonEstado += modoFocoParaTexto(estado.modo);
  jsonEstado += "\",\"modoExibicao\":\"";
  jsonEstado += modoExibicaoParaTexto(estado.modoExibicao);
  jsonEstado += "\",\"releLigado\":";
  jsonEstado += estado.releLigado ? "true" : "false";
  jsonEstado += ",\"controleManualAtivo\":";
  jsonEstado += estado.controleManualAtivo ? "true" : "false";
  jsonEstado += ",\"estadoControleManual\":";
  jsonEstado += estado.estadoControleManual ? "true" : "false";
  jsonEstado += ",\"contagemRegressivaSegundos\":";
  jsonEstado += String(estado.contagemRegressivaSegundos);
  jsonEstado += ",\"duracoes\":{\"focoSegundos\":";
  jsonEstado += String(estado.duracaoFocoSegundos);
  jsonEstado += ",\"pausaSegundos\":";
  jsonEstado += String(estado.duracaoPausaSegundos);
  jsonEstado += "},\"limiares\":{\"luminosidade\":";
  jsonEstado += String(estado.limiares.luminosidadePercentual);
  jsonEstado += ",\"umidade\":";
  jsonEstado += String(estado.limiares.umidadePercentual);
  jsonEstado += ",\"temperatura\":";
  jsonEstado += String(TEMPERATURA_PADRAO_ALERTA_C);
  jsonEstado += "},\"conectividade\":{\"wifi\":";
  jsonEstado += estado.conectividade.wifiConectado ? "true" : "false";
  jsonEstado += ",\"mqtt\":";
  jsonEstado += estado.conectividade.mqttConectado ? "true" : "false";
  jsonEstado += "},\"eventos\":{\"wifiOk\":";
  jsonEstado += (bitsEventos & BIT_WIFI_OK) ? "true" : "false";
  jsonEstado += ",\"mqttOk\":";
  jsonEstado += (bitsEventos & BIT_MQTT_OK) ? "true" : "false";
  jsonEstado += ",\"focoAtivo\":";
  jsonEstado += (bitsEventos & BIT_FOCO_ATIVO) ? "true" : "false";
  jsonEstado += ",\"alertaAtivo\":";
  jsonEstado += (bitsEventos & BIT_ALERTA_ATIVO) ? "true" : "false";
  jsonEstado += "},\"sensores\":{\"temperatura\":";
  jsonEstado += estado.ultimaLeituraSensores.dhtValido ? String(estado.ultimaLeituraSensores.temperaturaC, 1) : "null";
  jsonEstado += ",\"umidade\":";
  jsonEstado += estado.ultimaLeituraSensores.dhtValido ? String(estado.ultimaLeituraSensores.umidadePercentual, 1) : "null";
  jsonEstado += ",\"luminosidade\":";
  jsonEstado += String(estado.ultimaLeituraSensores.ldrPercentual);
  jsonEstado += ",\"luminosidadeBruta\":";
  jsonEstado += String(estado.ultimaLeituraSensores.ldrBruto);
  jsonEstado += ",\"dhtValido\":";
  jsonEstado += estado.ultimaLeituraSensores.dhtValido ? "true" : "false";
  jsonEstado += ",\"instanteMs\":";
  jsonEstado += String(estado.ultimaLeituraSensores.instanteMs);
  jsonEstado += "},\"alerta\":\"";
  jsonEstado += estadoAlertaParaTexto(calcularEstadoAlerta(estado));
  jsonEstado += "\"}";

  return jsonEstado;
}

String montarPaginaHtml() {
  EstadoSistema estado;
  copiarEstadoSistema(estado);

  String paginaHtml = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>PROVA04 Bancada Inteligente com FreeRTOS</title>
  <style>
    body { font-family: Arial, sans-serif; margin: 0; padding: 24px; background: #f4f7fb; color: #10233a; }
    main { max-width: 920px; margin: 0 auto; background: #ffffff; border-radius: 18px; padding: 24px; box-shadow: 0 14px 40px rgba(16, 35, 58, 0.12); }
    .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(170px, 1fr)); gap: 12px; margin-top: 18px; }
    .card { border: 1px solid #d8e1ec; border-radius: 14px; padding: 16px; background: #f8fbff; }
    .label { font-size: 0.9rem; color: #4c6480; }
    .value { font-size: 1.7rem; font-weight: 700; margin-top: 8px; }
    code { background: #eef4fb; padding: 2px 6px; border-radius: 6px; }
    p.meta { color: #4c6480; line-height: 1.5; }
  </style>
</head>
<body>
  <main>
    <h1>PROVA04 - Estacao de Trabalho IoT com FreeRTOS</h1>
    <p class="meta">Use <code>/estado</code> para JSON e <code>/configuracao?luminosidade=40&umidade=30&foco=25&pausa=5</code> para ajustes em segundos.</p>
    <div class="grid">
  )rawliteral";

  paginaHtml += "<section class=\"card\"><div class=\"label\">Modo</div><div class=\"value\">";
  paginaHtml += modoFocoParaTexto(estado.modo);
  paginaHtml += "</div></section>";

  paginaHtml += "<section class=\"card\"><div class=\"label\">Tempo restante</div><div class=\"value\">";
  paginaHtml += String(estado.contagemRegressivaSegundos);
  paginaHtml += " s</div></section>";

  paginaHtml += "<section class=\"card\"><div class=\"label\">Temperatura</div><div class=\"value\">";
  paginaHtml += estado.ultimaLeituraSensores.dhtValido ? String(estado.ultimaLeituraSensores.temperaturaC, 1) + " C" : "--";
  paginaHtml += "</div></section>";

  paginaHtml += "<section class=\"card\"><div class=\"label\">Umidade</div><div class=\"value\">";
  paginaHtml += estado.ultimaLeituraSensores.dhtValido ? String(estado.ultimaLeituraSensores.umidadePercentual, 1) + " %" : "--";
  paginaHtml += "</div></section>";

  paginaHtml += "<section class=\"card\"><div class=\"label\">Luminosidade</div><div class=\"value\">";
  paginaHtml += String(estado.ultimaLeituraSensores.ldrPercentual) + " %";
  paginaHtml += "</div></section>";

  paginaHtml += "<section class=\"card\"><div class=\"label\">Luminaria</div><div class=\"value\">";
  paginaHtml += estado.releLigado ? "LIGADO" : "DESLIGADO";
  paginaHtml += "</div></section>";

  paginaHtml += "<section class=\"card\"><div class=\"label\">Controle manual</div><div class=\"value\">";
  paginaHtml += estado.controleManualAtivo ? "ATIVO" : "AUTOMATICO";
  paginaHtml += "</div></section>";

  paginaHtml += "<section class=\"card\"><div class=\"label\">Alerta</div><div class=\"value\">";
  paginaHtml += estadoAlertaParaTexto(calcularEstadoAlerta(estado));
  paginaHtml += "</div></section>";

  paginaHtml += "<section class=\"card\"><div class=\"label\">Wi-Fi / MQTT</div><div class=\"value\">";
  paginaHtml += estado.conectividade.wifiConectado ? "OK" : "SEM";
  paginaHtml += " / ";
  paginaHtml += estado.conectividade.mqttConectado ? "OK" : "SEM";
  paginaHtml += "</div></section>";

  paginaHtml += R"rawliteral(
    </div>
  </main>
</body>
</html>
  )rawliteral";

  return paginaHtml;
}

void configurarRotasHttp() {
  if (rotasHttpConfiguradas) {
    return;
  }

  servidor.on("/", HTTP_GET, tratarRaiz);
  servidor.on("/estado", HTTP_GET, tratarEstado);
  servidor.on("/configuracao", HTTP_GET, tratarConfiguracao);
  servidor.onNotFound(tratarNaoEncontrado);
  rotasHttpConfiguradas = true;
}

void iniciarServidorHttp() {
  if (servidorHttpIniciado) {
    return;
  }

  configurarRotasHttp();
  servidor.begin();
  servidorHttpIniciado = true;
}

void tratarRaiz() {
  servidor.send(200, "text/html; charset=utf-8", montarPaginaHtml());
}

void tratarEstado() {
  servidor.send(200, "application/json", montarJsonEstado());
}

void tratarConfiguracao() {
  bool houveArgumento = false;
  bool sucesso = true;

  if (servidor.hasArg("luminosidade")) {
    ComandoControle comando = {ORIGEM_HTTP, ACAO_DEFINIR_LIMIAR_LUMINOSIDADE, servidor.arg("luminosidade").toInt(), false};
    sucesso = enfileirarComandoControle(comando, pdMS_TO_TICKS(20)) && sucesso;
    houveArgumento = true;
  }

  if (servidor.hasArg("umidade")) {
    ComandoControle comando = {ORIGEM_HTTP, ACAO_DEFINIR_LIMIAR_UMIDADE, servidor.arg("umidade").toInt(), false};
    sucesso = enfileirarComandoControle(comando, pdMS_TO_TICKS(20)) && sucesso;
    houveArgumento = true;
  }

  if (servidor.hasArg("foco")) {
    ComandoControle comando = {ORIGEM_HTTP, ACAO_DEFINIR_SEGUNDOS_FOCO, servidor.arg("foco").toInt(), false};
    sucesso = enfileirarComandoControle(comando, pdMS_TO_TICKS(20)) && sucesso;
    houveArgumento = true;
  }

  if (servidor.hasArg("pausa")) {
    ComandoControle comando = {ORIGEM_HTTP, ACAO_DEFINIR_SEGUNDOS_PAUSA, servidor.arg("pausa").toInt(), false};
    sucesso = enfileirarComandoControle(comando, pdMS_TO_TICKS(20)) && sucesso;
    houveArgumento = true;
  }

  if (houveArgumento && handleTarefaControle != NULL) {
    xTaskNotify(handleTarefaControle, NOTIFICACAO_COMANDO_REMOTO_CONTROLE, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(40));
  }

  if (!sucesso) {
    servidor.send(503, "application/json", "{\"erro\":\"fila de comandos ocupada\"}");
    return;
  }

  servidor.send(200, "application/json", montarJsonEstado());
}

void tratarNaoEncontrado() {
  servidor.send(404, "text/plain", "Rota nao encontrada.");
}

void limparTentativaConexaoWifi() {
  perfilWifiAtivo = PERFIL_WIFI_NENHUM;
  tentativaWifiIniciadaMs = 0;
}

void iniciarConexaoWifi(PerfilWifi perfil) {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);

  perfilWifiAtivo = perfil;
  tentativaWifiIniciadaMs = millis();
  ultimoCicloReconexaoWifiMs = 0;

  if (perfil == PERFIL_WIFI_PRIMARIO) {
    WiFi.begin(SSID_WIFI_PRIMARIO, SENHA_WIFI_PRIMARIO);
    return;
  }

  WiFi.begin(SSID_WIFI_BACKUP, WPA2_AUTH_PEAP, IDENTIDADE_EAP, USUARIO_EAP, SENHA_EAP);
}

bool aguardarWifi(unsigned long timeoutMs) {
  unsigned long inicioMs = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - inicioMs < timeoutMs) {
    delay(500);
  }

  return WiFi.status() == WL_CONNECTED;
}

void manterConexaoWifi() {
  if (WiFi.status() == WL_CONNECTED) {
    limparTentativaConexaoWifi();
    ultimoCicloReconexaoWifiMs = 0;

    if (!servidorHttpIniciado) {
      iniciarServidorHttp();
    }

    sincronizarEstadoConectividade();
    return;
  }

  if (perfilWifiAtivo == PERFIL_WIFI_NENHUM) {
    if (ultimoCicloReconexaoWifiMs == 0 ||
        millis() - ultimoCicloReconexaoWifiMs >= WIFI_INTERVALO_RECONEXAO_MS) {
      iniciarConexaoWifi(PERFIL_WIFI_PRIMARIO);
    }

    sincronizarEstadoConectividade();
    return;
  }

  if (millis() - tentativaWifiIniciadaMs >= WIFI_TIMEOUT_CONEXAO_MS) {
    if (perfilWifiAtivo == PERFIL_WIFI_PRIMARIO) {
      iniciarConexaoWifi(PERFIL_WIFI_BACKUP);
    } else {
      WiFi.disconnect(true);
      WiFi.mode(WIFI_STA);
      limparTentativaConexaoWifi();
      ultimoCicloReconexaoWifiMs = millis();
    }
  }

  sincronizarEstadoConectividade();
}

bool conectarBrokerMqtt() {
  String idCliente = "PROVA04-G" + String(ID_GRUPO) + "-" + String((uint32_t)ESP.getEfuseMac(), HEX);

  bool conectado = false;

  if (strlen(USUARIO_MQTT) > 0) {
    conectado = clienteMqtt.connect(idCliente.c_str(), USUARIO_MQTT, SENHA_MQTT);
  } else {
    conectado = clienteMqtt.connect(idCliente.c_str());
  }

  if (!conectado) {
    sincronizarEstadoConectividade();
    return false;
  }

  inscreverTopicos();
  sincronizarEstadoConectividade();
  publicarEstadoNoMqtt(true, true);
  return true;
}

void manterConexaoMqtt() {
  if (WiFi.status() != WL_CONNECTED) {
    sincronizarEstadoConectividade();
    return;
  }

  if (clienteMqtt.connected()) {
    sincronizarEstadoConectividade();
    return;
  }

  if (ultimaTentativaReconexaoMqttMs == 0 ||
      millis() - ultimaTentativaReconexaoMqttMs >= MQTT_INTERVALO_RECONEXAO_MS) {
    ultimaTentativaReconexaoMqttMs = millis();
    conectarBrokerMqtt();
  }

  sincronizarEstadoConectividade();
}

void inscreverTopicos() {
  clienteMqtt.subscribe(topicoComandoLuz);
}

void callbackMqtt(char *topico, byte *payload, unsigned int comprimento) {
  char bufferMensagem[32];
  const unsigned int tamanhoCopia = min(comprimento, sizeof(bufferMensagem) - 1U);

  memcpy(bufferMensagem, payload, tamanhoCopia);
  bufferMensagem[tamanhoCopia] = '\0';

  String mensagem(bufferMensagem);

  if (strcmp(topico, topicoComandoLuz) != 0) {
    return;
  }

  ComandoControle comando = {ORIGEM_MQTT, ACAO_DEFINIR_LUZ, 0, false};

  if (payloadSignificaLigar(mensagem)) {
    comando.valor = 1;
    comando.valorBooleano = true;
  } else if (payloadSignificaDesligar(mensagem)) {
    comando.valor = 0;
    comando.valorBooleano = false;
  } else {
    return;
  }

  if (enfileirarComandoControle(comando, 0) && handleTarefaControle != NULL) {
    xTaskNotify(handleTarefaControle, NOTIFICACAO_COMANDO_REMOTO_CONTROLE, eSetBits);
  }
}

void publicarLeituraTelemetria(const EstadoSistema &estado) {
  if (!clienteMqtt.connected()) {
    return;
  }

  char buffer[20];

  dtostrf(estado.ultimaLeituraSensores.temperaturaC, 0, 1, buffer);
  clienteMqtt.publish(topicoTemperatura, buffer, true);

  dtostrf(estado.ultimaLeituraSensores.umidadePercentual, 0, 1, buffer);
  clienteMqtt.publish(topicoUmidade, buffer, true);

  snprintf(buffer, sizeof(buffer), "%d", estado.ultimaLeituraSensores.ldrPercentual);
  clienteMqtt.publish(topicoLuminosidade, buffer, true);
}

void publicarResumoStatus(const EstadoSistema &estado, bool forcar) {
  if (!clienteMqtt.connected()) {
    return;
  }

  const String textoFoco = String(modoFocoParaTexto(estado.modo));
  const String textoAlerta = String(estadoAlertaParaTexto(calcularEstadoAlerta(estado)));

  if (forcar || textoFoco != ultimoFocoPublicado) {
    clienteMqtt.publish(topicoEstadoFoco, textoFoco.c_str(), true);
    ultimoFocoPublicado = textoFoco;
  }

  if (forcar || textoAlerta != ultimoAlertaPublicado) {
    clienteMqtt.publish(topicoEstadoAlerta, textoAlerta.c_str(), true);
    ultimoAlertaPublicado = textoAlerta;
  }
}

void publicarEstadoNoMqtt(bool publicarTelemetriaAgora, bool forcarStatus) {
  if (!clienteMqtt.connected()) {
    return;
  }

  EstadoSistema estado;
  copiarEstadoSistema(estado);

  if (publicarTelemetriaAgora) {
    publicarLeituraTelemetria(estado);
  }

  publicarResumoStatus(estado, forcarStatus);
}

void processarPressionamentoBotao(int indiceBotao) {
  bool houveMudancaEstado = false;

  xSemaphoreTake(mutexEstado, portMAX_DELAY);

  switch (indiceBotao) {
    case 0:
      if (gEstadoSistema.modo == MODO_OCIOSO || gEstadoSistema.modo == MODO_PAUSA) {
        definirModoTravado(MODO_FOCO);
      } else {
        definirModoTravado(MODO_PAUSA);
      }
      houveMudancaEstado = true;
      break;

    case 1:
      gEstadoSistema.controleManualAtivo = false;
      gEstadoSistema.estadoControleManual = false;
      definirModoTravado(MODO_OCIOSO);
      houveMudancaEstado = true;
      break;

    case 2:
      gEstadoSistema.modoExibicao = static_cast<ModoExibicao>((gEstadoSistema.modoExibicao + 1) % 3);
      houveMudancaEstado = true;
      break;

    case 3:
      if (gEstadoSistema.controleManualAtivo) {
        gEstadoSistema.controleManualAtivo = false;
      } else {
        gEstadoSistema.controleManualAtivo = true;
        gEstadoSistema.estadoControleManual = !gEstadoSistema.releLigado;
      }
      houveMudancaEstado = true;
      break;
  }

  if (houveMudancaEstado) {
    aplicarDecisaoIluminacaoTravado();
  }

  xSemaphoreGive(mutexEstado);

  if (houveMudancaEstado) {
    atualizarSaidasControle(true);
  }
}

void processarBotoes() {
  if (!COMPARTILHAR_PINOS_LEDS_COM_BOTOES) {
    return;
  }

  for (int i = 0; i < 4; i++) {
    bool leitura = digitalRead(PINOS_BOTOES[i]);

    if (leitura != ultimasLeiturasBotoes[i]) {
      ultimasLeiturasBotoes[i] = leitura;
      ultimosDebouncesBotoesMs[i] = millis();
    }

    if (millis() - ultimosDebouncesBotoesMs[i] < BOTAO_DEBOUNCE_MS) {
      continue;
    }

    if (leitura != estadosEstaveisBotoes[i]) {
      estadosEstaveisBotoes[i] = leitura;

      if (estadosEstaveisBotoes[i] == LOW) {
        processarPressionamentoBotao(i);
      }
    }
  }
}

void tratarLeituraSensores(const LeituraSensores &leitura) {
  xSemaphoreTake(mutexEstado, portMAX_DELAY);
  gEstadoSistema.ultimaLeituraSensores = leitura;
  aplicarDecisaoIluminacaoTravado();
  xSemaphoreGive(mutexEstado);

  atualizarSaidasControle(true);
}

void tratarComandoControle(const ComandoControle &comando) {
  bool houveMudancaEstado = false;

  xSemaphoreTake(mutexEstado, portMAX_DELAY);

  switch (comando.acao) {
    case ACAO_DEFINIR_LUZ:
      if (!gEstadoSistema.controleManualAtivo ||
          gEstadoSistema.estadoControleManual != comando.valorBooleano) {
        gEstadoSistema.controleManualAtivo = true;
        gEstadoSistema.estadoControleManual = comando.valorBooleano;
        houveMudancaEstado = true;
      }
      break;

    case ACAO_DEFINIR_LIMIAR_LUMINOSIDADE:
      gEstadoSistema.limiares.luminosidadePercentual = constrain(comando.valor, 0, 100);
      houveMudancaEstado = true;
      break;

    case ACAO_DEFINIR_LIMIAR_UMIDADE:
      gEstadoSistema.limiares.umidadePercentual = constrain(comando.valor, 0, 100);
      houveMudancaEstado = true;
      break;

    case ACAO_DEFINIR_SEGUNDOS_FOCO:
      gEstadoSistema.duracaoFocoSegundos = max(1, comando.valor);
      if (gEstadoSistema.modo == MODO_OCIOSO || gEstadoSistema.modo == MODO_FOCO) {
        gEstadoSistema.contagemRegressivaSegundos = gEstadoSistema.duracaoFocoSegundos;
      }
      houveMudancaEstado = true;
      break;

    case ACAO_DEFINIR_SEGUNDOS_PAUSA:
      gEstadoSistema.duracaoPausaSegundos = max(1, comando.valor);
      if (gEstadoSistema.modo == MODO_PAUSA) {
        gEstadoSistema.contagemRegressivaSegundos = gEstadoSistema.duracaoPausaSegundos;
      }
      houveMudancaEstado = true;
      break;
  }

  if (houveMudancaEstado) {
    aplicarDecisaoIluminacaoTravado();
  }

  xSemaphoreGive(mutexEstado);

  if (houveMudancaEstado) {
    atualizarSaidasControle(true);
  }
}

void tratarTickCiclo() {
  bool houveMudancaEstado = false;

  xSemaphoreTake(mutexEstado, portMAX_DELAY);

  if (gEstadoSistema.modo == MODO_FOCO || gEstadoSistema.modo == MODO_PAUSA) {
    if (gEstadoSistema.contagemRegressivaSegundos > 0) {
      gEstadoSistema.contagemRegressivaSegundos--;
      houveMudancaEstado = true;
    }

    if (gEstadoSistema.contagemRegressivaSegundos == 0) {
      if (gEstadoSistema.modo == MODO_FOCO) {
        definirModoTravado(MODO_PAUSA);
      } else {
        definirModoTravado(MODO_FOCO);
      }
      houveMudancaEstado = true;
    }

    aplicarDecisaoIluminacaoTravado();
  }

  xSemaphoreGive(mutexEstado);

  if (houveMudancaEstado) {
    atualizarSaidasControle(true);
  }
}

void atualizarSaidasControle(bool notificarConsumidoresAgora) {
  atualizarAtuadoresComEstado();

  if (notificarConsumidoresAgora) {
    notificarConsumidoresEstado();
  }
}

void callbackTimerAmostragem(TimerHandle_t xTimer) {
  (void) xTimer;

  if (handleTarefaSensores != NULL) {
    xTaskNotify(handleTarefaSensores, NOTIFICACAO_AMOSTRAGEM_SENSORES, eSetBits);
  }
}

void callbackTimerCiclo(TimerHandle_t xTimer) {
  (void) xTimer;

  if (handleTarefaControle != NULL) {
    xTaskNotify(handleTarefaControle, NOTIFICACAO_TICK_CICLO_CONTROLE, eSetBits);
  }
}

void callbackTimerPublicacao(TimerHandle_t xTimer) {
  (void) xTimer;

  if (handleTarefaIoT != NULL) {
    xTaskNotify(handleTarefaIoT, NOTIFICACAO_PUBLICAR_IOT, eSetBits);
  }
}

void tarefaSensores(void *pvParameters) {
  (void) pvParameters;

  float ultimaTemperaturaValida = 0.0f;
  float ultimaUmidadeValida = 0.0f;

  while (true) {
    uint32_t valorNotificacao = 0;
    xTaskNotifyWait(0, 0xFFFFFFFFUL, &valorNotificacao, portMAX_DELAY);

    if ((valorNotificacao & NOTIFICACAO_AMOSTRAGEM_SENSORES) == 0) {
      continue;
    }

    LeituraSensores leitura = {};

    sensorDht.read(PINO_DHT11);

    leitura.ldrBruto = analogRead(PINO_LDR);
    leitura.ldrPercentual = map(leitura.ldrBruto, 0, 4095, 100, 0);
    leitura.ldrPercentual = constrain(leitura.ldrPercentual, 0, 100);
    leitura.instanteMs = millis();

    const float temperaturaBruta = sensorDht.temperature;
    const float umidadeBruta = sensorDht.humidity;

    leitura.dhtValido =
      temperaturaBruta > -40.0f && temperaturaBruta < 100.0f &&
      umidadeBruta >= 0.0f && umidadeBruta <= 100.0f;

    if (leitura.dhtValido) {
      ultimaTemperaturaValida = temperaturaBruta;
      ultimaUmidadeValida = umidadeBruta;
    }

    leitura.temperaturaC = ultimaTemperaturaValida;
    leitura.umidadePercentual = ultimaUmidadeValida;

    xQueueOverwrite(qLeiturasSensores, &leitura);

    if (handleTarefaControle != NULL) {
      xTaskNotify(handleTarefaControle, NOTIFICACAO_DADOS_SENSORES_CONTROLE, eSetBits);
    }
  }
}

void tarefaControle(void *pvParameters) {
  (void) pvParameters;

  atualizarSaidasControle(true);

  while (true) {
    uint32_t valorNotificacao = 0;
    xTaskNotifyWait(0, 0xFFFFFFFFUL, &valorNotificacao, pdMS_TO_TICKS(INTERVALO_LOOP_CONTROLE_MS));

    if (valorNotificacao & NOTIFICACAO_TICK_CICLO_CONTROLE) {
      tratarTickCiclo();
    }

    LeituraSensores leituraSensores;
    while (xQueueReceive(qLeiturasSensores, &leituraSensores, 0) == pdTRUE) {
      tratarLeituraSensores(leituraSensores);
    }

    ComandoControle comandoControle;
    while (xQueueReceive(qComandosControle, &comandoControle, 0) == pdTRUE) {
      tratarComandoControle(comandoControle);
    }

    if (valorNotificacao & NOTIFICACAO_CONECTIVIDADE_CONTROLE) {
      atualizarSaidasControle(false);
    }

    atualizarSaidasCompartilhadasLeds();
    processarBotoes();
  }
}

void tarefaDisplay(void *pvParameters) {
  (void) pvParameters;

  EstadoSistema estadoEmCache;
  copiarEstadoSistema(estadoEmCache);

  while (true) {
    uint32_t valorNotificacao = 0;

    if (xTaskNotifyWait(0, 0xFFFFFFFFUL, &valorNotificacao, 0) == pdTRUE) {
      if (valorNotificacao & NOTIFICACAO_ATUALIZAR_DISPLAY) {
        copiarEstadoSistema(estadoEmCache);
      }
    }

    multiplexarDisplay(estadoEmCache);
  }
}

void tarefaIoT(void *pvParameters) {
  (void) pvParameters;

  while (true) {
    uint32_t valorNotificacao = 0;
    xTaskNotifyWait(0, 0xFFFFFFFFUL, &valorNotificacao, pdMS_TO_TICKS(INTERVALO_LOOP_IOT_MS));

    manterConexaoWifi();
    manterConexaoMqtt();

    if (clienteMqtt.connected()) {
      clienteMqtt.loop();
    }

    if (servidorHttpIniciado && WiFi.status() == WL_CONNECTED) {
      servidor.handleClient();
    }

    if (valorNotificacao & NOTIFICACAO_PUBLICAR_IOT) {
      publicarEstadoNoMqtt(true, false);
    }

    if (valorNotificacao & NOTIFICACAO_ESTADO_ALTERADO_IOT) {
      publicarEstadoNoMqtt(false, false);
    }
  }
}

void setup() {
  delay(200);

  pinMode(RELE, OUTPUT);
  digitalWrite(RELE, LOW);
  pinMode(PINO_LDR, INPUT);
  analogReadResolution(12);

  inicializarEstadoSistema();
  inicializarHardwareDisplay();
  inicializarBotoesELeds();
  montarTopicos();
  configurarRotasHttp();

  qLeiturasSensores = xQueueCreate(TAMANHO_FILA_SENSORES, sizeof(LeituraSensores));
  qComandosControle = xQueueCreate(TAMANHO_FILA_CONTROLE, sizeof(ComandoControle));
  mutexEstado = xSemaphoreCreateMutex();
  gEventos = xEventGroupCreate();

  tmrAmostragem = xTimerCreate("Amostragem", pdMS_TO_TICKS(INTERVALO_AMOSTRAGEM_SENSORES_MS), pdTRUE, 0, callbackTimerAmostragem);
  tmrTickCiclo = xTimerCreate("TickCiclo", pdMS_TO_TICKS(1000), pdTRUE, 0, callbackTimerCiclo);
  tmrPublicacao = xTimerCreate("Publicacao", pdMS_TO_TICKS(INTERVALO_PUBLICACAO_TELEMETRIA_MS), pdTRUE, 0, callbackTimerPublicacao);

  if (qLeiturasSensores == NULL || qComandosControle == NULL || mutexEstado == NULL ||
      gEventos == NULL || tmrAmostragem == NULL || tmrTickCiclo == NULL || tmrPublicacao == NULL) {
    falhaSegura("Falha ao criar fila, mutex, timers ou event group.");
  }

  clienteMqtt.setServer(HOST_MQTT, PORTA_MQTT);
  clienteMqtt.setCallback(callbackMqtt);
  clienteMqtt.setKeepAlive(SEGUNDOS_KEEP_ALIVE_MQTT);
  clienteMqtt.setSocketTimeout(SEGUNDOS_TIMEOUT_SOCKET_MQTT);
  clienteEsp.setInsecure();

  if (xTaskCreate(tarefaSensores, "TarefaSensores", 4096, NULL, 2, &handleTarefaSensores) != pdPASS ||
      xTaskCreate(tarefaControle, "TarefaControle", 6144, NULL, 3, &handleTarefaControle) != pdPASS ||
      xTaskCreate(tarefaDisplay, "TarefaDisplay", 3072, NULL, 2, &handleTarefaDisplay) != pdPASS ||
      xTaskCreate(tarefaIoT, "TarefaIoT", 8192, NULL, 1, &handleTarefaIoT) != pdPASS) {
    falhaSegura("Falha ao criar as tarefas do sistema.");
  }

  if (xTimerStart(tmrAmostragem, 0) != pdPASS ||
      xTimerStart(tmrTickCiclo, 0) != pdPASS ||
      xTimerStart(tmrPublicacao, 0) != pdPASS) {
    falhaSegura("Falha ao iniciar os software timers.");
  }

  iniciarConexaoWifi(PERFIL_WIFI_PRIMARIO);
  bool wifiConectado = aguardarWifi(WIFI_TIMEOUT_CONEXAO_MS);

  if (!wifiConectado) {
    iniciarConexaoWifi(PERFIL_WIFI_BACKUP);
    wifiConectado = aguardarWifi(WIFI_TIMEOUT_CONEXAO_MS);
  }

  if (wifiConectado) {
    limparTentativaConexaoWifi();
    ultimoCicloReconexaoWifiMs = 0;
    iniciarServidorHttp();
    conectarBrokerMqtt();
  } else {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    limparTentativaConexaoWifi();
    ultimoCicloReconexaoWifiMs = millis();
  }

  atualizarSaidasControle(true);
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
