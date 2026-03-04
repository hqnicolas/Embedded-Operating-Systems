// ===============================
// Classe para Entrada Digital
// ===============================
class EntradaDigital {
  private:
    int pino;

  public:
    EntradaDigital(int p) {
      pino = p;
    }

    void iniciar() {
      pinMode(pino, INPUT_PULLDOWN);
    }

    bool pressionado() {
      return digitalRead(pino) == HIGH;
    }
};

// ===============================
// Classe Sinalizador
// ===============================
class Sinalizador {
  private:
    int pino;
    unsigned long tempoCurto;
    unsigned long tempoLongo;
    unsigned long intervalo;

    void pulso(unsigned long tempoOn, unsigned long tempoOff) {
      digitalWrite(pino, HIGH);
      delay(tempoOn);
      digitalWrite(pino, LOW);
      delay(tempoOff);
    }

  public:
    Sinalizador(int p, unsigned long curto,
                unsigned long longo, unsigned long inter) {
      pino = p;
      tempoCurto = curto;
      tempoLongo = longo;
      intervalo = inter;
    }

    void iniciar() {
      pinMode(pino, OUTPUT);
    }

    void statusOK() {
      pulso(tempoCurto, intervalo);
    }

    void atencao(int ciclos) {
      for (int i = 0; i < ciclos; i++) {
        for (int j = 0; j < 3; j++) {
          pulso(tempoCurto, tempoCurto);
        }
        delay(intervalo);
      }
    }

    void emergencia() {
      // 3 longos
      for (int i = 0; i < 3; i++) {
        pulso(tempoLongo, tempoCurto);
      }

      // 3 curtos
      for (int i = 0; i < 3; i++) {
        pulso(tempoCurto, tempoCurto);
      }

      // 3 longos
      for (int i = 0; i < 3; i++) {
        pulso(tempoLongo, intervalo);
      }
    }
};

// ===============================
// Objetos
// ===============================

EntradaDigital botao1(32);
EntradaDigital botao2(33);

// LED vermelho no pino 19
Sinalizador sinal(19, 150, 500, 1000);

int modo = 0; 
// 0 = OK
// 1 = ATENCAO
// 2 = EMERGENCIA

void setup() {
  Serial.begin(115200);

  botao1.iniciar();
  botao2.iniciar();
  sinal.iniciar();

  Serial.println("Sistema de Monitoramento Iniciado");
}

void loop() {

  if (botao1.pressionado()) {
    modo++;
    if (modo > 1) modo = 0;
    Serial.println("Modo alternado (OK/ATENCAO)");
    delay(300); // debounce simples
  }

  if (botao2.pressionado()) {
    modo = 2;
    Serial.println("!!! EMERGENCIA !!!");
    delay(300);
  }

  switch (modo) {

    case 0:
      sinal.statusOK();
      break;

    case 1:
      sinal.atencao(2);
      break;

    case 2:
      sinal.emergencia();
      modo = 0; // volta para OK após SOS
      break;
  }
}