void setup() {
  Serial.begin(115200);
  ledcAttach(26, 5000, 8); //pino (canal), frequencia, qtd bits.
}

void loop() {
  // Escreve um valor de 100 no canal 0.
  // O valor vai de 0 a 255 (pois a resolução é de 8 bits)
  for (int i = 0; i<256; i++){
    ledcWrite(26,i);
    delay(2);
  }

}