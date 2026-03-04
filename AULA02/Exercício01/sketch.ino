class EntradaDigital {
    private:
      int pino; // Atributo: apenas a classe mexe aqui
  
    public:
      // Construtor: executa quando o objeto é criado para ajudar a inicializar
      EntradaDigital(int p) {
        pino = p;
      }
  
      // Método para configurar o pino
      void iniciar() {
        pinMode(pino, INPUT_PULLDOWN);
      }
  
      // Método para ler o estado (invertido por causa do Pullup)
      bool pressionado() {
        return digitalRead(pino) == HIGH; 
      }
  };
  
  #include "entradaDigital.h"

  // Criando objetos
  entradaDigital botao1(32);
  entradaDigital botao2(33);
  
  void setup() {
    Serial.begin(9600);
    botao1.iniciar();
    botao2.iniciar();
  }
  
  void loop() {
  
    if (botao1.pulso()) {
      Serial.println("Botão 1 pressionado (pulso)!");
    }
  
    if (botao2.pulso()) {
      Serial.println("Botão 2 pressionado (pulso)!");
    }
  
  }