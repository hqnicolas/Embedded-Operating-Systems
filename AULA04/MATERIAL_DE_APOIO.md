# Material de Apoio: Sistemas Embarcados (ESP32)

Este guia prático foi criado para auxiliar na aplicação dos conceitos das **Aulas 01 a 04**. Aqui você encontrará exemplos condensados e prontos para adaptar em seus projetos.

---

## Aula 01: Fundamentos Digitais e Tempo

O maior erro ao iniciar é travar o processador com `delay()`. Use `millis()` para multitarefa básica e `INPUT_PULLUP` para simplificar o hardware.

### Exemplo: LED Piscando sem Travar + Botão
*Faz o LED disparar um "alerta" de 200ms enquanto lê o botão instantaneamente.*

```cpp
const int LED_PIN = 2; 
const int BTN_PIN = 4; // Conectado ao GND

unsigned long tempoAnterior = 0;
const long intervalo = 200; 
bool estadoLed = LOW;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP); // Dispensa resistor externo
}

void loop() {
  unsigned long tempoAtual = millis();

  // Controle de tempo paralelo
  if (tempoAtual - tempoAnterior >= intervalo) {
    tempoAnterior = tempoAtual;
    estadoLed = !estadoLed;
    digitalWrite(LED_PIN, estadoLed);
  }

  // Leitura instantânea (Lógica invertida devido ao PULLUP)
  if (digitalRead(BTN_PIN) == LOW) {
    // Ação imediata ao pressionar
  }
}
```

---

## 🛠 Aula 02: Organização com Objetos (C++)

Para projetos complexos, não use variáveis soltas. Crie **Classes**. Isso permite gerenciar 10 LEDs ou 10 Botões com o mesmo esforço de um.

### Exemplo: Classe Botão com Debounce Simples
```cpp
class Botao {
  private:
    int pino;
    bool ultimoEstado;
  public:
    Botao(int p) : pino(p), ultimoEstado(HIGH) {}
    
    void iniciar() {
      pinMode(pino, INPUT_PULLUP);
    }

    bool pressionado() {
      bool leitura = digitalRead(pino);
      if (leitura == LOW && ultimoEstado == HIGH) {
        delay(50); // Debounce simples (filtro)
        ultimoEstado = LOW;
        return true;
      }
      ultimoEstado = leitura;
      return false;
    }
};

Botao b1(4);

void setup() { b1.iniciar(); }
void loop() {
  if (b1.pressionado()) { /* Evento único */ }
}
```

---

## Aula 03: Sinais Analógicos (ADC e PWM)

O ESP32 tem resolução de **12 bits** (0-4095) na entrada e usa `ledc` para saída PWM (não existe `analogWrite` nativo igual no Arduino Uno em versões antigas do Core).

### Exemplo: Dimmer de LED com Potenciômetro
```cpp
const int POT_PIN = 34;
const int LED_PWM = 2;

// Configurações PWM
const int freq = 5000;
const int canal = 0;
const int resolucao = 8; // 0-255

void setup() {
  Serial.begin(115200);
  ledcSetup(canal, freq, resolucao);
  ledcAttachPin(LED_PWM, canal);
}

void loop() {
  int leitura = analogRead(POT_PIN); // 0 a 4095
  int brilho = map(leitura, 0, 4095, 0, 255);
  
  ledcWrite(canal, brilho);
  
  Serial.printf("Brilho: %d | Voltagem: %.2fV\n", brilho, (leitura * 3.3 / 4095.0));
  delay(10);
}
```

---

## Aula 04: Timers de Hardware

Quando a precisão de microssegundos é vital ou você não quer depender do `loop()`, use os **Timers**.

### Exemplo: Interrupção por Tempo
*Gera um evento crítico exatamente a cada 1 segundo, independente do que estiver ocorrendo no código.*

```cpp
hw_timer_t * timer = NULL;
volatile byte estado = LOW;

void IRAM_ATTR onTimer() {
  estado = !estado;
  digitalWrite(2, estado); // Pisca LED na interrupção
}

void setup() {
  pinMode(2, OUTPUT);
  // Timer 0, Prescaler 80 (80MHz / 80 = 1MHz ou 1 tick/us)
  timer = timerBegin(0, 80, true); 
  timerAttachInterrupt(timer, &onTimer, true);
  // 1.000.000 ticks = 1 segundo
  timerAlarmWrite(timer, 1000000, true); 
  timerAlarmEnable(timer);
}

void loop() {
  // O loop pode estar vazio ou ocupado, o LED piscará via hardware!
}
```

---

> [!TIP]
> **Dica de Ouro:** Sempre utilize nomes de variáveis significativos e comente por que você está usando um pino específico. Isso facilita a manutenção futura.
