const int PIN_RED = 19;
const int PIN_GREEN = 18;
const int PIN_BLUE = 5;
const int BTN_1 = 32;
const int BTN_2 = 33;

unsigned long previousMillis = 0;
const long interval = 200; 
bool redState = LOW;

void setup() {
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP);
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    redState = !redState;
    digitalWrite(PIN_RED, redState);
  }

  if (digitalRead(BTN_1) == LOW) {
    digitalWrite(PIN_GREEN, HIGH);
  } else {
    digitalWrite(PIN_GREEN, LOW);
  }

  if (digitalRead(BTN_2) == LOW) {
    digitalWrite(PIN_BLUE, HIGH);
  } else {
    digitalWrite(PIN_BLUE, LOW);
  }
}