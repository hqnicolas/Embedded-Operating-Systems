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
  
  // Criando os Objetos C=
  EntradaDigital botao1(32);
  EntradaDigital botao2(33);
  
  void setup() {
    Serial.begin(9600);
    // Chamando os métodos de cada objeto
    botao1.iniciar();
    botao2.iniciar();
  }
  
  void loop() {
    if (botao1.pressionado()) {
      Serial.println("Botão 1 ativo!");
    }
    
    if (botao2.pressionado()) {
      Serial.println("Botão 2 ativo!");
    }
    delay(100);
  }