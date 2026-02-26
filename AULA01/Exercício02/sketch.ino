const int pinoA = 18;
const int pinoB = 19;
const int pinoC = 0;
const int pinoD = 4;
const int pinoE = 16;
const int pinoF = 5;
const int pinoG = 17;

const int btnAumenta = 14;
const int btnDiminui = 12;

int contador = 0;

const byte numeros[10][7] = {
  {1, 1, 1, 1, 1, 1, 0}, 
  {0, 1, 1, 0, 0, 0, 0},
  {1, 1, 0, 1, 1, 0, 1}, 
  {1, 1, 1, 1, 0, 0, 1}, 
  {0, 1, 1, 0, 0, 1, 1},
  {1, 0, 1, 1, 0, 1, 1}, 
  {1, 0, 1, 1, 1, 1, 1},
  {1, 1, 1, 0, 0, 0, 0}, 
  {1, 1, 1, 1, 1, 1, 1}, 
  {1, 1, 1, 1, 0, 1, 1} 
};

void atualizarDisplay(int n) {
  digitalWrite(pinoA, numeros[n][0]);
  digitalWrite(pinoB, numeros[n][1]);
  digitalWrite(pinoC, numeros[n][2]);
  digitalWrite(pinoD, numeros[n][3]);
  digitalWrite(pinoE, numeros[n][4]);
  digitalWrite(pinoF, numeros[n][5]);
  digitalWrite(pinoG, numeros[n][6]);
}

void setup() {
  // Configura segmentos como saída
  int pinos[] = {pinoA, pinoB, pinoC, pinoD, pinoE, pinoF, pinoG};
  for(int i=0; i<7; i++) pinMode(pinos[i], OUTPUT);

  pinMode(btnAumenta, INPUT_PULLUP);
  pinMode(btnDiminui, INPUT_PULLUP);

  atualizarDisplay(contador);
}

void loop() {
  if (digitalRead(btnAumenta) == LOW) {
    delay(50);
    for (contador = 0; contador <= 9; contador++) {
      atualizarDisplay(contador);
  
      if (digitalRead(btnDiminui) == LOW) {
        break; 
      }
      
      delay(1000);
    }
  }

  if (digitalRead(btnDiminui) == LOW) {
    contador = 0;
    atualizarDisplay(contador);
    delay(200);
  }
}