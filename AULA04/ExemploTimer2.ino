#define LED_PIN 17
#define btnPulso 32

#define TEMPOFILTRO 10

hw_timer_t *timer = NULL;

int tempobtnPulso = 0, auxbtnPulso = 0;
int contador=0;

void IRAM_ATTR onTimer() {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  if (tempobtnPulso) tempobtnPulso--;
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(btnPulso, INPUT_PULLUP);

  timer = timerBegin(10000); 

  if (timer) {
    timerAttachInterrupt(timer, &onTimer);
    timerAlarm(timer, 1000, true, 0);
  }
  
}

void loop() {
  if ((digitalRead(btnPulso)) (auxbtnPulso) && (!tempobtnPulso)){
    auxbtnPulso = 1;
    ++contador;
    Serial.println(contador);
  }
  else if (!digitalRead(btnPulso)){
    auxbtnPulso = 0;
    tempobtnPulso = TEMPOFILTRO;
  }
}






