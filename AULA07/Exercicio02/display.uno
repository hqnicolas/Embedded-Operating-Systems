/*
  Autor: João Carlos de Souza Filho / 2024

  Este algoritmo tem como objetivo realizar a escrita de números nos
  displays de 7 segmentos. O algoritmo escreverá de 0 até 99 e trocará
  o número a cada 500ms.

  IMPORTANTE:
  Os displays utilizados na placa são Catodo Comum, o que significa que o comum dos
  segmentos são interligados no GND, sendo necessário enviar nível lógico 1 para
  que o segmento seja acionado.
  A placa utiliza a técnica de multiplexação para escrever nos 2 displays,
  no intuito de economizar pinos do microcontrolador.
  Para mais informações, consultar o esquemático ou pesquisar como a técnica funciona.
*/

#define LED1 4  
#define LED2 0  
#define LED3 2  
#define LED4 15 
#define BOTAO1 4
#define BOTAO2 0
#define BOTAO3 2
#define BOTAO4 15
#define RGB_RED 25 
#define RGB_GREEN 26 
#define RGB_BLUE 27 
#define RELE 13
#define A 18
#define B 5
#define C 21
#define D 03
#define E 01
#define F 23
#define G 22
#define DP 19
#define DISPLAY1 16
#define DISPLAY2 17
#define DHT11_PIN 33 //VP
#define LDR 39 //VN
#define POTENCIOMETRO 34
#define ULTRASSONIC_TRIG 32
#define ULTRASSONIC_ECHO 35

byte segNum[][8] = 
{
  {1,1,1,1,1,1,0,0}, //0
  {0,1,1,0,0,0,0,0}, //1
  {1,1,0,1,1,0,1,0}, //2
  {1,1,1,1,0,0,1,0}, //3
  {0,1,1,0,0,1,1,0}, //4
  {1,0,1,1,0,1,1,0}, //5
  {1,0,1,1,1,1,1,0}, //6
  {1,1,1,0,0,0,0,0}, //7
  {1,1,1,1,1,1,1,0}, //8 
  {1,1,1,1,0,1,1,0}, //9
};

int unity = 0;
int ten   = 0;

int numero = 0;

unsigned long temporizador = millis();

byte display_pins[] = {A, B, C, D, E, F, G, DP, DISPLAY1, DISPLAY2};

void setDigit(int flag)
{
  for(int i = 0; i < 8; i++)
  {
    digitalWrite(display_pins[i],segNum[flag][i]);
  }
}

void multiplexing()
{
  setDigit(ten);
  digitalWrite(DISPLAY1, HIGH);
  delay(1);
  digitalWrite(DISPLAY1, LOW);
    
  setDigit(unity);
  digitalWrite(DISPLAY2, HIGH);
  delay(1);
  digitalWrite(DISPLAY2, LOW);
}

// Essa função é executada apenas uma vez ao iniciar o algoritmo, antes do loop
void setup() {
  Serial.begin(115200);

  for(int i = 0; i <= sizeof(display_pins); i++){
    pinMode(display_pins[i], OUTPUT);
  }
}

// Essa função repetirá indefinidamente
void loop() {
  //Incrementa o número a cada 500ms
  if((millis() - temporizador) > 500){
    if (numero == 99){
      numero = 0;
    }else{
      numero++;
    }
    
    temporizador = millis();
  }

  ten = numero / 10;
  unity = numero % 10;

  multiplexing();
}