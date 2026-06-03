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

const char *WIFI_SSID = "SATC 2.4";
const char *EAP_IDENTITY = "aluno.123456@alunosatc.edu.br";
const char *EAP_USERNAME = "aluno.123456@alunosatc.edu.br";
const char *EAP_PASSWORD = "123456";

const char *MQTT_HOST = "broker.hivemq.com";
constexpr int MQTT_PORT = 8883;
const char *MQTT_USERNAME = "";
const char *MQTT_PASSWORD = "";
constexpr uint16_t MQTT_KEEP_ALIVE_SECONDS = 60;
constexpr uint16_t MQTT_SOCKET_TIMEOUT_SECONDS = 20;

constexpr int GROUP_ID = 7;

#define RELE 13
#define SEG_A 18
#define SEG_B 5
#define SEG_C 21
#define SEG_D 3
#define SEG_E 1
#define SEG_F 23
#define SEG_G 22
#define SEG_DP 19
#define DISPLAY_1 17
#define DISPLAY_2 16
#define DHT11_PIN 33
#define LDR_PIN 39

#define LED1 4
#define LED2 0
#define LED3 2
#define LED4 15
#define BOTAO1 4
#define BOTAO2 0
#define BOTAO3 2
#define BOTAO4 15

constexpr bool SHARE_LED_PINS_AS_BUTTONS = true;
constexpr bool DISPLAY_SEGMENT_ACTIVE_HIGH = true;
constexpr bool DISPLAY_DIGIT_ACTIVE_LOW = true;

constexpr unsigned long WIFI_CONNECT_TIMEOUT_MS = 20000;
constexpr unsigned long WIFI_RETRY_INTERVAL_MS = 5000;
constexpr unsigned long MQTT_RETRY_INTERVAL_MS = 5000;
constexpr unsigned long BUTTON_DEBOUNCE_MS = 60;
constexpr unsigned long SHARED_PIN_OUTPUT_TIME_US = 1500;

constexpr uint32_t SENSOR_SAMPLE_INTERVAL_MS = 1500;
constexpr uint32_t TELEMETRY_PUBLISH_INTERVAL_MS = 4000;
constexpr uint32_t CONTROL_LOOP_WAIT_MS = 5;
constexpr uint32_t DISPLAY_DIGIT_HOLD_MS = 2;
constexpr uint32_t IOT_LOOP_WAIT_MS = 20;

constexpr int DEFAULT_LUMINOSITY_THRESHOLD = 40;
constexpr int DEFAULT_HUMIDITY_ALERT_THRESHOLD = 30;
constexpr int DEFAULT_TEMPERATURE_ALERT_C = 30;
constexpr uint32_t DEFAULT_FOCUS_DURATION_SECONDS = 25;
constexpr uint32_t DEFAULT_PAUSE_DURATION_SECONDS = 5;

constexpr size_t SENSOR_QUEUE_LENGTH = 1;
constexpr size_t CONTROL_QUEUE_LENGTH = 10;

constexpr uint32_t NOTIFY_SENSOR_SAMPLE = 1UL << 0;
constexpr uint32_t NOTIFY_CONTROL_CYCLE_TICK = 1UL << 0;
constexpr uint32_t NOTIFY_CONTROL_SENSOR_DATA = 1UL << 1;
constexpr uint32_t NOTIFY_CONTROL_REMOTE_CMD = 1UL << 2;
constexpr uint32_t NOTIFY_CONTROL_CONNECTIVITY = 1UL << 3;
constexpr uint32_t NOTIFY_DISPLAY_REFRESH = 1UL << 0;
constexpr uint32_t NOTIFY_IOT_PUBLISH = 1UL << 0;
constexpr uint32_t NOTIFY_IOT_STATE_CHANGED = 1UL << 1;

constexpr EventBits_t BIT_WIFI_OK = 1UL << 0;
constexpr EventBits_t BIT_MQTT_OK = 1UL << 1;
constexpr EventBits_t BIT_FOCUS_ACTIVE = 1UL << 2;
constexpr EventBits_t BIT_ALERT_ACTIVE = 1UL << 3;

enum FocusMode : uint8_t {
  MODE_IDLE = 0,
  MODE_FOCUS,
  MODE_PAUSE
};

enum DisplayMode : uint8_t {
  DISPLAY_TEMPERATURE = 0,
  DISPLAY_HUMIDITY,
  DISPLAY_LUMINOSITY
};

enum AlertState : uint8_t {
  ALERT_OK = 0,
  ALERT_WARN,
  ALERT_ACTIVE
};

enum ControlSource : uint8_t {
  SOURCE_MQTT = 0,
  SOURCE_HTTP,
  SOURCE_LOCAL
};

enum ControlAction : uint8_t {
  ACTION_SET_LIGHT = 0,
  ACTION_SET_LUX_THRESHOLD,
  ACTION_SET_HUMIDITY_THRESHOLD,
  ACTION_SET_FOCUS_SECONDS,
  ACTION_SET_PAUSE_SECONDS
};

struct SensorSnapshot {
  float temperatureC;
  float humidityPercent;
  int ldrRaw;
  int ldrPercent;
  bool dhtOk;
  unsigned long timestampMs;
};

struct Thresholds {
  int luminosityPercent;
  int humidityPercent;
};

struct ConnectivityState {
  bool wifiConnected;
  bool mqttConnected;
};

struct SystemState {
  FocusMode mode;
  DisplayMode displayMode;
  bool relayOn;
  bool manualOverrideEnabled;
  bool manualOverrideState;
  Thresholds thresholds;
  uint32_t focusDurationSeconds;
  uint32_t pauseDurationSeconds;
  uint32_t countdownSeconds;
  ConnectivityState connectivity;
  SensorSnapshot lastSensors;
};

struct ControlCommand {
  ControlSource source;
  ControlAction action;
  int value;
  bool boolValue;
};

DFRobot_DHT11 dht;
WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);
WebServer server(80);

QueueHandle_t qSensorSnapshots = NULL;
QueueHandle_t qControlCommands = NULL;
SemaphoreHandle_t mutexEstado = NULL;
EventGroupHandle_t gEventos = NULL;

TimerHandle_t tmrSample = NULL;
TimerHandle_t tmrCycleTick = NULL;
TimerHandle_t tmrPublish = NULL;

TaskHandle_t taskSensoresHandle = NULL;
TaskHandle_t taskControleHandle = NULL;
TaskHandle_t taskDisplayHandle = NULL;
TaskHandle_t taskIoTHandle = NULL;

SystemState gSystemState;

const int LED_PINS[4] = {LED1, LED2, LED3, LED4};
const int BUTTON_PINS[4] = {BOTAO1, BOTAO2, BOTAO3, BOTAO4};
const int SEGMENT_PINS[8] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_DP};
const int DISPLAY_PINS[2] = {DISPLAY_1, DISPLAY_2};

const byte DIGIT_PATTERNS[10][8] = {
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

bool ledStates[4] = {false, false, false, false};
bool buttonStableStates[4] = {HIGH, HIGH, HIGH, HIGH};
bool buttonLastReadings[4] = {HIGH, HIGH, HIGH, HIGH};
unsigned long buttonLastDebounceMs[4] = {0, 0, 0, 0};

bool httpRoutesConfigured = false;
bool httpServerStarted = false;
unsigned long lastWiFiReconnectAttemptMs = 0;
unsigned long lastMqttReconnectAttemptMs = 0;

char topicTemperature[48];
char topicHumidity[48];
char topicLuminosity[48];
char topicLightCommand[48];
char topicFocusStatus[48];
char topicAlertStatus[48];

String lastPublishedFocus = "";
String lastPublishedAlert = "";

void failSafe(const char *message);
void buildTopics();
void initializeSystemState();
void initializeDisplayHardware();
void initializeButtonsAndLeds();
void configureSharedPinsAsInput();
void refreshSharedLedOutputs();
void updateOutputCacheFromState(const SystemState &state);
void applySegmentLevel(int pin, bool active);
void applyDigitLevel(int pin, bool active);
void disableDisplays();
void writeDigitPattern(int value, bool decimalPoint);
int computeDisplayValue(const SystemState &state);
void multiplexDisplay(const SystemState &state);
void copySystemState(SystemState &destination);
const char *focusModeToText(FocusMode mode);
const char *displayModeToText(DisplayMode mode);
AlertState computeAlertState(const SystemState &state);
const char *alertStateToText(AlertState alertState);
bool payloadMeansOn(const String &payload);
bool payloadMeansOff(const String &payload);
void syncConnectivityState();
void syncStateEventBits(const SystemState &state);
void notifyStateConsumers();
void updateActuatorsFromState();
void applyLightingDecisionLocked();
void setModeLocked(FocusMode nextMode);
bool enqueueControlCommand(const ControlCommand &command, TickType_t ticksToWait);
String buildStatusJson();
String buildHtmlPage();
void configureHttpRoutes();
void startHttpServer();
void handleRoot();
void handleStatus();
void handleConfig();
void handleNotFound();
void startWiFiConnection();
bool waitForWiFi(unsigned long timeoutMs);
void maintainWiFiConnection();
bool connectToMqttBroker();
void maintainMqttConnection();
void subscribeTopics();
void mqttCallback(char *topic, byte *payload, unsigned int length);
void publishTelemetrySnapshot(const SystemState &state);
void publishStatusSnapshot(const SystemState &state, bool force);
void publishStateToMqtt(bool publishTelemetryNow, bool forceStatus);
void processButtonPress(int buttonIndex);
void processButtons();
void handleSensorSnapshot(const SensorSnapshot &snapshot);
void handleControlCommand(const ControlCommand &command);
void handleCycleTick();
void refreshControlOutputs(bool notifyConsumersNow);
void callbackSampleTimer(TimerHandle_t xTimer);
void callbackCycleTimer(TimerHandle_t xTimer);
void callbackPublishTimer(TimerHandle_t xTimer);
void taskSensores(void *pvParameters);
void taskControle(void *pvParameters);
void taskDisplay(void *pvParameters);
void taskIoT(void *pvParameters);

void failSafe(const char *message) {
  (void) message;

  while (true) {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void buildTopics() {
  snprintf(topicTemperature, sizeof(topicTemperature), "satc/g%d/telemetry/temperature", GROUP_ID);
  snprintf(topicHumidity, sizeof(topicHumidity), "satc/g%d/telemetry/humidity", GROUP_ID);
  snprintf(topicLuminosity, sizeof(topicLuminosity), "satc/g%d/telemetry/luminosity", GROUP_ID);
  snprintf(topicLightCommand, sizeof(topicLightCommand), "satc/g%d/cmd/light", GROUP_ID);
  snprintf(topicFocusStatus, sizeof(topicFocusStatus), "satc/g%d/status/focus", GROUP_ID);
  snprintf(topicAlertStatus, sizeof(topicAlertStatus), "satc/g%d/status/alert", GROUP_ID);
}

void initializeSystemState() {
  gSystemState.mode = MODE_IDLE;
  gSystemState.displayMode = DISPLAY_TEMPERATURE;
  gSystemState.relayOn = false;
  gSystemState.manualOverrideEnabled = false;
  gSystemState.manualOverrideState = false;
  gSystemState.thresholds.luminosityPercent = DEFAULT_LUMINOSITY_THRESHOLD;
  gSystemState.thresholds.humidityPercent = DEFAULT_HUMIDITY_ALERT_THRESHOLD;
  gSystemState.focusDurationSeconds = DEFAULT_FOCUS_DURATION_SECONDS;
  gSystemState.pauseDurationSeconds = DEFAULT_PAUSE_DURATION_SECONDS;
  gSystemState.countdownSeconds = DEFAULT_FOCUS_DURATION_SECONDS;
  gSystemState.connectivity.wifiConnected = false;
  gSystemState.connectivity.mqttConnected = false;
  gSystemState.lastSensors.temperatureC = 0.0f;
  gSystemState.lastSensors.humidityPercent = 0.0f;
  gSystemState.lastSensors.ldrRaw = 0;
  gSystemState.lastSensors.ldrPercent = 0;
  gSystemState.lastSensors.dhtOk = false;
  gSystemState.lastSensors.timestampMs = 0;
}

void initializeDisplayHardware() {
  for (int i = 0; i < 8; i++) {
    pinMode(SEGMENT_PINS[i], OUTPUT);
    applySegmentLevel(SEGMENT_PINS[i], false);
  }

  for (int i = 0; i < 2; i++) {
    pinMode(DISPLAY_PINS[i], OUTPUT);
    applyDigitLevel(DISPLAY_PINS[i], false);
  }

  disableDisplays();
}

void initializeButtonsAndLeds() {
  for (int i = 0; i < 4; i++) {
    if (SHARE_LED_PINS_AS_BUTTONS) {
      pinMode(BUTTON_PINS[i], INPUT_PULLUP);
      buttonLastReadings[i] = digitalRead(BUTTON_PINS[i]);
      buttonStableStates[i] = buttonLastReadings[i];
      ledStates[i] = false;
    } else {
      pinMode(LED_PINS[i], OUTPUT);
      digitalWrite(LED_PINS[i], LOW);
    }
  }

  if (SHARE_LED_PINS_AS_BUTTONS) {
    refreshSharedLedOutputs();
  }
}

void configureSharedPinsAsInput() {
  for (int i = 0; i < 4; i++) {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
  }
}

void refreshSharedLedOutputs() {
  if (!SHARE_LED_PINS_AS_BUTTONS) {
    return;
  }

  for (int i = 0; i < 4; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], ledStates[i] ? HIGH : LOW);
  }

  delayMicroseconds(SHARED_PIN_OUTPUT_TIME_US);
  configureSharedPinsAsInput();
}

void updateOutputCacheFromState(const SystemState &state) {
  ledStates[0] = state.mode == MODE_FOCUS;
  ledStates[1] = state.mode == MODE_PAUSE;
  ledStates[2] = state.connectivity.wifiConnected && state.connectivity.mqttConnected;
  ledStates[3] = computeAlertState(state) != ALERT_OK;
}

void applySegmentLevel(int pin, bool active) {
  digitalWrite(pin, active == DISPLAY_SEGMENT_ACTIVE_HIGH ? HIGH : LOW);
}

void applyDigitLevel(int pin, bool active) {
  bool level = active ? !DISPLAY_DIGIT_ACTIVE_LOW : DISPLAY_DIGIT_ACTIVE_LOW;
  digitalWrite(pin, level ? HIGH : LOW);
}

void disableDisplays() {
  for (int i = 0; i < 2; i++) {
    applyDigitLevel(DISPLAY_PINS[i], false);
  }
}

void writeDigitPattern(int value, bool decimalPoint) {
  if (value < 0 || value > 9) {
    for (int i = 0; i < 8; i++) {
      applySegmentLevel(SEGMENT_PINS[i], false);
    }
    return;
  }

  for (int i = 0; i < 7; i++) {
    applySegmentLevel(SEGMENT_PINS[i], DIGIT_PATTERNS[value][i] == 1);
  }

  applySegmentLevel(SEG_DP, decimalPoint);
}

int computeDisplayValue(const SystemState &state) {
  if (state.mode == MODE_FOCUS || state.mode == MODE_PAUSE) {
    uint32_t longestDuration = max(state.focusDurationSeconds, state.pauseDurationSeconds);

    if (longestDuration <= 99) {
      return constrain(static_cast<int>(state.countdownSeconds), 0, 99);
    }

    uint32_t minutesRemaining = (state.countdownSeconds + 59UL) / 60UL;
    return constrain(static_cast<int>(minutesRemaining), 0, 99);
  }

  switch (state.displayMode) {
    case DISPLAY_HUMIDITY:
      return constrain(static_cast<int>(round(state.lastSensors.humidityPercent)), 0, 99);
    case DISPLAY_LUMINOSITY:
      return constrain(state.lastSensors.ldrPercent, 0, 99);
    case DISPLAY_TEMPERATURE:
    default:
      return constrain(static_cast<int>(round(state.lastSensors.temperatureC)), 0, 99);
  }
}

void multiplexDisplay(const SystemState &state) {
  static int activeDigit = 0;

  const int displayValue = computeDisplayValue(state);
  const int tens = displayValue / 10;
  const int units = displayValue % 10;

  disableDisplays();

  if (activeDigit == 0) {
    writeDigitPattern(tens, false);
    applyDigitLevel(DISPLAY_1, true);
  } else {
    writeDigitPattern(units, false);
    applyDigitLevel(DISPLAY_2, true);
  }

  activeDigit = 1 - activeDigit;
  vTaskDelay(pdMS_TO_TICKS(DISPLAY_DIGIT_HOLD_MS));
}

void copySystemState(SystemState &destination) {
  xSemaphoreTake(mutexEstado, portMAX_DELAY);
  destination = gSystemState;
  xSemaphoreGive(mutexEstado);
}

const char *focusModeToText(FocusMode mode) {
  switch (mode) {
    case MODE_FOCUS:
      return "FOCUS";
    case MODE_PAUSE:
      return "PAUSE";
    case MODE_IDLE:
    default:
      return "IDLE";
  }
}

const char *displayModeToText(DisplayMode mode) {
  switch (mode) {
    case DISPLAY_HUMIDITY:
      return "HUMIDITY";
    case DISPLAY_LUMINOSITY:
      return "LUMINOSITY";
    case DISPLAY_TEMPERATURE:
    default:
      return "TEMPERATURE";
  }
}

AlertState computeAlertState(const SystemState &state) {
  if (!state.lastSensors.dhtOk) {
    return ALERT_WARN;
  }

  if (state.lastSensors.temperatureC > DEFAULT_TEMPERATURE_ALERT_C ||
      state.lastSensors.humidityPercent < state.thresholds.humidityPercent) {
    return ALERT_ACTIVE;
  }

  return ALERT_OK;
}

const char *alertStateToText(AlertState alertState) {
  switch (alertState) {
    case ALERT_WARN:
      return "WARN";
    case ALERT_ACTIVE:
      return "ALERT";
    case ALERT_OK:
    default:
      return "OK";
  }
}

bool payloadMeansOn(const String &payload) {
  return payload.equalsIgnoreCase("ON") ||
         payload.equalsIgnoreCase("1") ||
         payload.equalsIgnoreCase("TRUE");
}

bool payloadMeansOff(const String &payload) {
  return payload.equalsIgnoreCase("OFF") ||
         payload.equalsIgnoreCase("0") ||
         payload.equalsIgnoreCase("FALSE");
}

void syncConnectivityState() {
  const bool wifiConnected = WiFi.status() == WL_CONNECTED;
  const bool mqttConnected = wifiConnected && mqttClient.connected();
  bool changed = false;

  xSemaphoreTake(mutexEstado, portMAX_DELAY);

  if (gSystemState.connectivity.wifiConnected != wifiConnected ||
      gSystemState.connectivity.mqttConnected != mqttConnected) {
    gSystemState.connectivity.wifiConnected = wifiConnected;
    gSystemState.connectivity.mqttConnected = mqttConnected;
    changed = true;
  }

  xSemaphoreGive(mutexEstado);

  if (wifiConnected) {
    xEventGroupSetBits(gEventos, BIT_WIFI_OK);
  } else {
    xEventGroupClearBits(gEventos, BIT_WIFI_OK);
  }

  if (mqttConnected) {
    xEventGroupSetBits(gEventos, BIT_MQTT_OK);
  } else {
    xEventGroupClearBits(gEventos, BIT_MQTT_OK);
  }

  if (changed && taskControleHandle != NULL) {
    xTaskNotify(taskControleHandle, NOTIFY_CONTROL_CONNECTIVITY, eSetBits);
  }
}

void syncStateEventBits(const SystemState &state) {
  if (state.mode == MODE_FOCUS) {
    xEventGroupSetBits(gEventos, BIT_FOCUS_ACTIVE);
  } else {
    xEventGroupClearBits(gEventos, BIT_FOCUS_ACTIVE);
  }

  if (computeAlertState(state) == ALERT_OK) {
    xEventGroupClearBits(gEventos, BIT_ALERT_ACTIVE);
  } else {
    xEventGroupSetBits(gEventos, BIT_ALERT_ACTIVE);
  }
}

void notifyStateConsumers() {
  if (taskDisplayHandle != NULL) {
    xTaskNotify(taskDisplayHandle, NOTIFY_DISPLAY_REFRESH, eSetBits);
  }

  if (taskIoTHandle != NULL) {
    xTaskNotify(taskIoTHandle, NOTIFY_IOT_STATE_CHANGED, eSetBits);
  }
}

void updateActuatorsFromState() {
  SystemState snapshot;
  copySystemState(snapshot);

  digitalWrite(RELE, snapshot.relayOn ? HIGH : LOW);
  updateOutputCacheFromState(snapshot);
  syncStateEventBits(snapshot);
}

void applyLightingDecisionLocked() {
  bool automaticRelay =
    gSystemState.mode == MODE_FOCUS &&
    gSystemState.lastSensors.ldrPercent < gSystemState.thresholds.luminosityPercent;

  if (gSystemState.manualOverrideEnabled) {
    gSystemState.relayOn = gSystemState.manualOverrideState;
  } else {
    gSystemState.relayOn = automaticRelay;
  }
}

void setModeLocked(FocusMode nextMode) {
  gSystemState.mode = nextMode;

  switch (nextMode) {
    case MODE_FOCUS:
      gSystemState.countdownSeconds = gSystemState.focusDurationSeconds;
      break;
    case MODE_PAUSE:
      gSystemState.countdownSeconds = gSystemState.pauseDurationSeconds;
      break;
    case MODE_IDLE:
    default:
      gSystemState.countdownSeconds = gSystemState.focusDurationSeconds;
      break;
  }
}

bool enqueueControlCommand(const ControlCommand &command, TickType_t ticksToWait) {
  if (qControlCommands == NULL) {
    return false;
  }

  return xQueueSend(qControlCommands, &command, ticksToWait) == pdPASS;
}

String buildStatusJson() {
  SystemState state;
  copySystemState(state);
  const EventBits_t eventBits = xEventGroupGetBits(gEventos);

  String json = "{";
  json += "\"mode\":\"";
  json += focusModeToText(state.mode);
  json += "\",\"displayMode\":\"";
  json += displayModeToText(state.displayMode);
  json += "\",\"relayOn\":";
  json += state.relayOn ? "true" : "false";
  json += ",\"manualOverrideEnabled\":";
  json += state.manualOverrideEnabled ? "true" : "false";
  json += ",\"manualOverrideState\":";
  json += state.manualOverrideState ? "true" : "false";
  json += ",\"countdownSeconds\":";
  json += String(state.countdownSeconds);
  json += ",\"durations\":{\"focusSeconds\":";
  json += String(state.focusDurationSeconds);
  json += ",\"pauseSeconds\":";
  json += String(state.pauseDurationSeconds);
  json += "},\"thresholds\":{\"luminosity\":";
  json += String(state.thresholds.luminosityPercent);
  json += ",\"humidity\":";
  json += String(state.thresholds.humidityPercent);
  json += ",\"temperature\":";
  json += String(DEFAULT_TEMPERATURE_ALERT_C);
  json += "},\"connectivity\":{\"wifi\":";
  json += state.connectivity.wifiConnected ? "true" : "false";
  json += ",\"mqtt\":";
  json += state.connectivity.mqttConnected ? "true" : "false";
  json += "},\"events\":{\"wifiOk\":";
  json += (eventBits & BIT_WIFI_OK) ? "true" : "false";
  json += ",\"mqttOk\":";
  json += (eventBits & BIT_MQTT_OK) ? "true" : "false";
  json += ",\"focusActive\":";
  json += (eventBits & BIT_FOCUS_ACTIVE) ? "true" : "false";
  json += ",\"alertActive\":";
  json += (eventBits & BIT_ALERT_ACTIVE) ? "true" : "false";
  json += "},\"sensors\":{\"temperature\":";
  json += state.lastSensors.dhtOk ? String(state.lastSensors.temperatureC, 1) : "null";
  json += ",\"humidity\":";
  json += state.lastSensors.dhtOk ? String(state.lastSensors.humidityPercent, 1) : "null";
  json += ",\"luminosity\":";
  json += String(state.lastSensors.ldrPercent);
  json += ",\"luminosityRaw\":";
  json += String(state.lastSensors.ldrRaw);
  json += ",\"dhtOk\":";
  json += state.lastSensors.dhtOk ? "true" : "false";
  json += ",\"timestampMs\":";
  json += String(state.lastSensors.timestampMs);
  json += "},\"alert\":\"";
  json += alertStateToText(computeAlertState(state));
  json += "\"}";

  return json;
}

String buildHtmlPage() {
  SystemState state;
  copySystemState(state);

  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>PROVA04 Smart Desk FreeRTOS</title>
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
    <p class="meta">Use <code>/status</code> para JSON e <code>/config?lux=40&humidity=30&focus=25&pause=5</code> para ajustes em segundos.</p>
    <div class="grid">
  )rawliteral";

  html += "<section class=\"card\"><div class=\"label\">Modo</div><div class=\"value\">";
  html += focusModeToText(state.mode);
  html += "</div></section>";

  html += "<section class=\"card\"><div class=\"label\">Tempo restante</div><div class=\"value\">";
  html += String(state.countdownSeconds);
  html += " s</div></section>";

  html += "<section class=\"card\"><div class=\"label\">Temperatura</div><div class=\"value\">";
  html += state.lastSensors.dhtOk ? String(state.lastSensors.temperatureC, 1) + " C" : "--";
  html += "</div></section>";

  html += "<section class=\"card\"><div class=\"label\">Umidade</div><div class=\"value\">";
  html += state.lastSensors.dhtOk ? String(state.lastSensors.humidityPercent, 1) + " %" : "--";
  html += "</div></section>";

  html += "<section class=\"card\"><div class=\"label\">Luminosidade</div><div class=\"value\">";
  html += String(state.lastSensors.ldrPercent) + " %";
  html += "</div></section>";

  html += "<section class=\"card\"><div class=\"label\">Luminaria</div><div class=\"value\">";
  html += state.relayOn ? "ON" : "OFF";
  html += "</div></section>";

  html += "<section class=\"card\"><div class=\"label\">Override manual</div><div class=\"value\">";
  html += state.manualOverrideEnabled ? "ATIVO" : "AUTO";
  html += "</div></section>";

  html += "<section class=\"card\"><div class=\"label\">Alerta</div><div class=\"value\">";
  html += alertStateToText(computeAlertState(state));
  html += "</div></section>";

  html += "<section class=\"card\"><div class=\"label\">Wi-Fi / MQTT</div><div class=\"value\">";
  html += state.connectivity.wifiConnected ? "OK" : "OFF";
  html += " / ";
  html += state.connectivity.mqttConnected ? "OK" : "OFF";
  html += "</div></section>";

  html += R"rawliteral(
    </div>
  </main>
</body>
</html>
  )rawliteral";

  return html;
}

void configureHttpRoutes() {
  if (httpRoutesConfigured) {
    return;
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/config", HTTP_GET, handleConfig);
  server.onNotFound(handleNotFound);
  httpRoutesConfigured = true;
}

void startHttpServer() {
  if (httpServerStarted) {
    return;
  }

  configureHttpRoutes();
  server.begin();
  httpServerStarted = true;
}

void handleRoot() {
  server.send(200, "text/html; charset=utf-8", buildHtmlPage());
}

void handleStatus() {
  server.send(200, "application/json", buildStatusJson());
}

void handleConfig() {
  bool anyArgument = false;
  bool success = true;

  if (server.hasArg("lux")) {
    ControlCommand command = {SOURCE_HTTP, ACTION_SET_LUX_THRESHOLD, server.arg("lux").toInt(), false};
    success = enqueueControlCommand(command, pdMS_TO_TICKS(20)) && success;
    anyArgument = true;
  }

  if (server.hasArg("humidity")) {
    ControlCommand command = {SOURCE_HTTP, ACTION_SET_HUMIDITY_THRESHOLD, server.arg("humidity").toInt(), false};
    success = enqueueControlCommand(command, pdMS_TO_TICKS(20)) && success;
    anyArgument = true;
  }

  if (server.hasArg("focus")) {
    ControlCommand command = {SOURCE_HTTP, ACTION_SET_FOCUS_SECONDS, server.arg("focus").toInt(), false};
    success = enqueueControlCommand(command, pdMS_TO_TICKS(20)) && success;
    anyArgument = true;
  }

  if (server.hasArg("pause")) {
    ControlCommand command = {SOURCE_HTTP, ACTION_SET_PAUSE_SECONDS, server.arg("pause").toInt(), false};
    success = enqueueControlCommand(command, pdMS_TO_TICKS(20)) && success;
    anyArgument = true;
  }

  if (anyArgument && taskControleHandle != NULL) {
    xTaskNotify(taskControleHandle, NOTIFY_CONTROL_REMOTE_CMD, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(40));
  }

  if (!success) {
    server.send(503, "application/json", "{\"error\":\"command queue busy\"}");
    return;
  }

  server.send(200, "application/json", buildStatusJson());
}

void handleNotFound() {
  server.send(404, "text/plain", "Rota nao encontrada.");
}

void startWiFiConnection() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WPA2_AUTH_PEAP, EAP_IDENTITY, EAP_USERNAME, EAP_PASSWORD);
}

bool waitForWiFi(unsigned long timeoutMs) {
  unsigned long startMs = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startMs < timeoutMs) {
    delay(500);
  }

  return WiFi.status() == WL_CONNECTED;
}

void maintainWiFiConnection() {
  if (WiFi.status() == WL_CONNECTED) {
    if (!httpServerStarted) {
      startHttpServer();
    }

    syncConnectivityState();
    return;
  }

  if (lastWiFiReconnectAttemptMs == 0 ||
      millis() - lastWiFiReconnectAttemptMs >= WIFI_RETRY_INTERVAL_MS) {
    lastWiFiReconnectAttemptMs = millis();
    startWiFiConnection();
  }

  syncConnectivityState();
}

bool connectToMqttBroker() {
  String clientId = "PROVA04-G" + String(GROUP_ID) + "-" + String((uint32_t)ESP.getEfuseMac(), HEX);

  bool connected = false;

  if (strlen(MQTT_USERNAME) > 0) {
    connected = mqttClient.connect(clientId.c_str(), MQTT_USERNAME, MQTT_PASSWORD);
  } else {
    connected = mqttClient.connect(clientId.c_str());
  }

  if (!connected) {
    syncConnectivityState();
    return false;
  }

  subscribeTopics();
  syncConnectivityState();
  publishStateToMqtt(true, true);
  return true;
}

void maintainMqttConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    syncConnectivityState();
    return;
  }

  if (mqttClient.connected()) {
    syncConnectivityState();
    return;
  }

  if (lastMqttReconnectAttemptMs == 0 ||
      millis() - lastMqttReconnectAttemptMs >= MQTT_RETRY_INTERVAL_MS) {
    lastMqttReconnectAttemptMs = millis();
    connectToMqttBroker();
  }

  syncConnectivityState();
}

void subscribeTopics() {
  mqttClient.subscribe(topicLightCommand);
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
  char messageBuffer[32];
  const unsigned int copyLength = min(length, sizeof(messageBuffer) - 1U);

  memcpy(messageBuffer, payload, copyLength);
  messageBuffer[copyLength] = '\0';

  String message(messageBuffer);

  if (strcmp(topic, topicLightCommand) != 0) {
    return;
  }

  ControlCommand command = {SOURCE_MQTT, ACTION_SET_LIGHT, 0, false};

  if (payloadMeansOn(message)) {
    command.value = 1;
    command.boolValue = true;
  } else if (payloadMeansOff(message)) {
    command.value = 0;
    command.boolValue = false;
  } else {
    return;
  }

  if (enqueueControlCommand(command, 0) && taskControleHandle != NULL) {
    xTaskNotify(taskControleHandle, NOTIFY_CONTROL_REMOTE_CMD, eSetBits);
  }
}

void publishTelemetrySnapshot(const SystemState &state) {
  if (!mqttClient.connected()) {
    return;
  }

  char buffer[20];

  dtostrf(state.lastSensors.temperatureC, 0, 1, buffer);
  mqttClient.publish(topicTemperature, buffer, true);

  dtostrf(state.lastSensors.humidityPercent, 0, 1, buffer);
  mqttClient.publish(topicHumidity, buffer, true);

  snprintf(buffer, sizeof(buffer), "%d", state.lastSensors.ldrPercent);
  mqttClient.publish(topicLuminosity, buffer, true);
}

void publishStatusSnapshot(const SystemState &state, bool force) {
  if (!mqttClient.connected()) {
    return;
  }

  const String focusText = String(focusModeToText(state.mode));
  const String alertText = String(alertStateToText(computeAlertState(state)));

  if (force || focusText != lastPublishedFocus) {
    mqttClient.publish(topicFocusStatus, focusText.c_str(), true);
    lastPublishedFocus = focusText;
  }

  if (force || alertText != lastPublishedAlert) {
    mqttClient.publish(topicAlertStatus, alertText.c_str(), true);
    lastPublishedAlert = alertText;
  }
}

void publishStateToMqtt(bool publishTelemetryNow, bool forceStatus) {
  if (!mqttClient.connected()) {
    return;
  }

  SystemState state;
  copySystemState(state);

  if (publishTelemetryNow) {
    publishTelemetrySnapshot(state);
  }

  publishStatusSnapshot(state, forceStatus);
}

void processButtonPress(int buttonIndex) {
  bool stateChanged = false;

  xSemaphoreTake(mutexEstado, portMAX_DELAY);

  switch (buttonIndex) {
    case 0:
      if (gSystemState.mode == MODE_IDLE || gSystemState.mode == MODE_PAUSE) {
        setModeLocked(MODE_FOCUS);
      } else {
        setModeLocked(MODE_PAUSE);
      }
      stateChanged = true;
      break;

    case 1:
      gSystemState.manualOverrideEnabled = false;
      gSystemState.manualOverrideState = false;
      setModeLocked(MODE_IDLE);
      stateChanged = true;
      break;

    case 2:
      gSystemState.displayMode = static_cast<DisplayMode>((gSystemState.displayMode + 1) % 3);
      stateChanged = true;
      break;

    case 3:
      if (gSystemState.manualOverrideEnabled) {
        gSystemState.manualOverrideEnabled = false;
      } else {
        gSystemState.manualOverrideEnabled = true;
        gSystemState.manualOverrideState = !gSystemState.relayOn;
      }
      stateChanged = true;
      break;
  }

  if (stateChanged) {
    applyLightingDecisionLocked();
  }

  xSemaphoreGive(mutexEstado);

  if (stateChanged) {
    refreshControlOutputs(true);
  }
}

void processButtons() {
  if (!SHARE_LED_PINS_AS_BUTTONS) {
    return;
  }

  for (int i = 0; i < 4; i++) {
    bool reading = digitalRead(BUTTON_PINS[i]);

    if (reading != buttonLastReadings[i]) {
      buttonLastReadings[i] = reading;
      buttonLastDebounceMs[i] = millis();
    }

    if (millis() - buttonLastDebounceMs[i] < BUTTON_DEBOUNCE_MS) {
      continue;
    }

    if (reading != buttonStableStates[i]) {
      buttonStableStates[i] = reading;

      if (buttonStableStates[i] == LOW) {
        processButtonPress(i);
      }
    }
  }
}

void handleSensorSnapshot(const SensorSnapshot &snapshot) {
  xSemaphoreTake(mutexEstado, portMAX_DELAY);
  gSystemState.lastSensors = snapshot;
  applyLightingDecisionLocked();
  xSemaphoreGive(mutexEstado);

  refreshControlOutputs(true);
}

void handleControlCommand(const ControlCommand &command) {
  bool stateChanged = false;

  xSemaphoreTake(mutexEstado, portMAX_DELAY);

  switch (command.action) {
    case ACTION_SET_LIGHT:
      if (!gSystemState.manualOverrideEnabled ||
          gSystemState.manualOverrideState != command.boolValue) {
        gSystemState.manualOverrideEnabled = true;
        gSystemState.manualOverrideState = command.boolValue;
        stateChanged = true;
      }
      break;

    case ACTION_SET_LUX_THRESHOLD:
      gSystemState.thresholds.luminosityPercent = constrain(command.value, 0, 100);
      stateChanged = true;
      break;

    case ACTION_SET_HUMIDITY_THRESHOLD:
      gSystemState.thresholds.humidityPercent = constrain(command.value, 0, 100);
      stateChanged = true;
      break;

    case ACTION_SET_FOCUS_SECONDS:
      gSystemState.focusDurationSeconds = max(1, command.value);
      if (gSystemState.mode == MODE_IDLE || gSystemState.mode == MODE_FOCUS) {
        gSystemState.countdownSeconds = gSystemState.focusDurationSeconds;
      }
      stateChanged = true;
      break;

    case ACTION_SET_PAUSE_SECONDS:
      gSystemState.pauseDurationSeconds = max(1, command.value);
      if (gSystemState.mode == MODE_PAUSE) {
        gSystemState.countdownSeconds = gSystemState.pauseDurationSeconds;
      }
      stateChanged = true;
      break;
  }

  if (stateChanged) {
    applyLightingDecisionLocked();
  }

  xSemaphoreGive(mutexEstado);

  if (stateChanged) {
    refreshControlOutputs(true);
  }
}

void handleCycleTick() {
  bool stateChanged = false;

  xSemaphoreTake(mutexEstado, portMAX_DELAY);

  if (gSystemState.mode == MODE_FOCUS || gSystemState.mode == MODE_PAUSE) {
    if (gSystemState.countdownSeconds > 0) {
      gSystemState.countdownSeconds--;
      stateChanged = true;
    }

    if (gSystemState.countdownSeconds == 0) {
      if (gSystemState.mode == MODE_FOCUS) {
        setModeLocked(MODE_PAUSE);
      } else {
        setModeLocked(MODE_FOCUS);
      }
      stateChanged = true;
    }

    applyLightingDecisionLocked();
  }

  xSemaphoreGive(mutexEstado);

  if (stateChanged) {
    refreshControlOutputs(true);
  }
}

void refreshControlOutputs(bool notifyConsumersNow) {
  updateActuatorsFromState();

  if (notifyConsumersNow) {
    notifyStateConsumers();
  }
}

void callbackSampleTimer(TimerHandle_t xTimer) {
  (void) xTimer;

  if (taskSensoresHandle != NULL) {
    xTaskNotify(taskSensoresHandle, NOTIFY_SENSOR_SAMPLE, eSetBits);
  }
}

void callbackCycleTimer(TimerHandle_t xTimer) {
  (void) xTimer;

  if (taskControleHandle != NULL) {
    xTaskNotify(taskControleHandle, NOTIFY_CONTROL_CYCLE_TICK, eSetBits);
  }
}

void callbackPublishTimer(TimerHandle_t xTimer) {
  (void) xTimer;

  if (taskIoTHandle != NULL) {
    xTaskNotify(taskIoTHandle, NOTIFY_IOT_PUBLISH, eSetBits);
  }
}

void taskSensores(void *pvParameters) {
  (void) pvParameters;

  float lastValidTemperature = 0.0f;
  float lastValidHumidity = 0.0f;

  while (true) {
    uint32_t notificationValue = 0;
    xTaskNotifyWait(0, 0xFFFFFFFFUL, &notificationValue, portMAX_DELAY);

    if ((notificationValue & NOTIFY_SENSOR_SAMPLE) == 0) {
      continue;
    }

    SensorSnapshot snapshot = {};

    dht.read(DHT11_PIN);

    snapshot.ldrRaw = analogRead(LDR_PIN);
    snapshot.ldrPercent = map(snapshot.ldrRaw, 0, 4095, 100, 0);
    snapshot.ldrPercent = constrain(snapshot.ldrPercent, 0, 100);
    snapshot.timestampMs = millis();

    const float rawTemperature = dht.temperature;
    const float rawHumidity = dht.humidity;

    snapshot.dhtOk =
      rawTemperature > -40.0f && rawTemperature < 100.0f &&
      rawHumidity >= 0.0f && rawHumidity <= 100.0f;

    if (snapshot.dhtOk) {
      lastValidTemperature = rawTemperature;
      lastValidHumidity = rawHumidity;
    }

    snapshot.temperatureC = lastValidTemperature;
    snapshot.humidityPercent = lastValidHumidity;

    xQueueOverwrite(qSensorSnapshots, &snapshot);

    if (taskControleHandle != NULL) {
      xTaskNotify(taskControleHandle, NOTIFY_CONTROL_SENSOR_DATA, eSetBits);
    }
  }
}

void taskControle(void *pvParameters) {
  (void) pvParameters;

  refreshControlOutputs(true);

  while (true) {
    uint32_t notificationValue = 0;
    xTaskNotifyWait(0, 0xFFFFFFFFUL, &notificationValue, pdMS_TO_TICKS(CONTROL_LOOP_WAIT_MS));

    if (notificationValue & NOTIFY_CONTROL_CYCLE_TICK) {
      handleCycleTick();
    }

    SensorSnapshot sensorSnapshot;
    while (xQueueReceive(qSensorSnapshots, &sensorSnapshot, 0) == pdTRUE) {
      handleSensorSnapshot(sensorSnapshot);
    }

    ControlCommand controlCommand;
    while (xQueueReceive(qControlCommands, &controlCommand, 0) == pdTRUE) {
      handleControlCommand(controlCommand);
    }

    if (notificationValue & NOTIFY_CONTROL_CONNECTIVITY) {
      refreshControlOutputs(false);
    }

    refreshSharedLedOutputs();
    processButtons();
  }
}

void taskDisplay(void *pvParameters) {
  (void) pvParameters;

  SystemState cachedState;
  copySystemState(cachedState);

  while (true) {
    uint32_t notificationValue = 0;

    if (xTaskNotifyWait(0, 0xFFFFFFFFUL, &notificationValue, 0) == pdTRUE) {
      if (notificationValue & NOTIFY_DISPLAY_REFRESH) {
        copySystemState(cachedState);
      }
    }

    multiplexDisplay(cachedState);
  }
}

void taskIoT(void *pvParameters) {
  (void) pvParameters;

  while (true) {
    uint32_t notificationValue = 0;
    xTaskNotifyWait(0, 0xFFFFFFFFUL, &notificationValue, pdMS_TO_TICKS(IOT_LOOP_WAIT_MS));

    maintainWiFiConnection();
    maintainMqttConnection();

    if (mqttClient.connected()) {
      mqttClient.loop();
    }

    if (httpServerStarted && WiFi.status() == WL_CONNECTED) {
      server.handleClient();
    }

    if (notificationValue & NOTIFY_IOT_PUBLISH) {
      publishStateToMqtt(true, false);
    }

    if (notificationValue & NOTIFY_IOT_STATE_CHANGED) {
      publishStateToMqtt(false, false);
    }
  }
}

void setup() {
  delay(200);

  pinMode(RELE, OUTPUT);
  digitalWrite(RELE, LOW);
  pinMode(LDR_PIN, INPUT);
  analogReadResolution(12);

  initializeSystemState();
  initializeDisplayHardware();
  initializeButtonsAndLeds();
  buildTopics();
  configureHttpRoutes();

  qSensorSnapshots = xQueueCreate(SENSOR_QUEUE_LENGTH, sizeof(SensorSnapshot));
  qControlCommands = xQueueCreate(CONTROL_QUEUE_LENGTH, sizeof(ControlCommand));
  mutexEstado = xSemaphoreCreateMutex();
  gEventos = xEventGroupCreate();

  tmrSample = xTimerCreate("Sample", pdMS_TO_TICKS(SENSOR_SAMPLE_INTERVAL_MS), pdTRUE, 0, callbackSampleTimer);
  tmrCycleTick = xTimerCreate("CycleTick", pdMS_TO_TICKS(1000), pdTRUE, 0, callbackCycleTimer);
  tmrPublish = xTimerCreate("Publish", pdMS_TO_TICKS(TELEMETRY_PUBLISH_INTERVAL_MS), pdTRUE, 0, callbackPublishTimer);

  if (qSensorSnapshots == NULL || qControlCommands == NULL || mutexEstado == NULL ||
      gEventos == NULL || tmrSample == NULL || tmrCycleTick == NULL || tmrPublish == NULL) {
    failSafe("Falha ao criar queue, mutex, timers ou event group.");
  }

  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  mqttClient.setKeepAlive(MQTT_KEEP_ALIVE_SECONDS);
  mqttClient.setSocketTimeout(MQTT_SOCKET_TIMEOUT_SECONDS);
  espClient.setInsecure();

  if (xTaskCreate(taskSensores, "TaskSensores", 4096, NULL, 2, &taskSensoresHandle) != pdPASS ||
      xTaskCreate(taskControle, "TaskControle", 6144, NULL, 3, &taskControleHandle) != pdPASS ||
      xTaskCreate(taskDisplay, "TaskDisplay", 3072, NULL, 2, &taskDisplayHandle) != pdPASS ||
      xTaskCreate(taskIoT, "TaskIoT", 8192, NULL, 1, &taskIoTHandle) != pdPASS) {
    failSafe("Falha ao criar as tasks do sistema.");
  }

  if (xTimerStart(tmrSample, 0) != pdPASS ||
      xTimerStart(tmrCycleTick, 0) != pdPASS ||
      xTimerStart(tmrPublish, 0) != pdPASS) {
    failSafe("Falha ao iniciar os software timers.");
  }

  startWiFiConnection();

  if (waitForWiFi(WIFI_CONNECT_TIMEOUT_MS)) {
    startHttpServer();
    connectToMqttBroker();
  }

  refreshControlOutputs(true);
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
