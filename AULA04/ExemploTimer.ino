#define LED_PIN 17
#define btnStart 32
#define btnStop 33

#define T_LED 2

hw_timer_t *timer = NULL;

int tempoLed = 0;
int iniciou=0;

void IRAM_ATTR onTimer() {
  if (tempoLed) tempoLed--;
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(btnStart, INPUT_PULLDOWN);
  pinMode(btnStop, INPUT_PULLDOWN);

  timer = timerBegin(10000); 

  if (timer) {
    timerAttachInterrupt(timer, &onTimer);
    timerAlarm(timer, 10000, true, 0);
  }
  
}

void loop() {
  if (digitalRead(btnStart)){
    iniciou = 1;
    tempoLed = T_LED;
  }
  if (digitalRead(btnStop)){
    digitalWrite(LED_PIN,0);
  }
  
  if (iniciou && !tempoLed){
    digitalWrite(LED_PIN, 1);
    iniciou = 0;
  }

}







