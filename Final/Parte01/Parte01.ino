#include <DHT.h>

#include <LiquidCrystal.h>

#define DHTPIN A1 // pino que estamos conectado
#define DHTTYPE DHT11 // DHT 11

DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int incomingByte, x, y;//para display LCD
int ligarArCond = 2;
int ligarChuveiro = 2;
int flagAr = 0;
int flagChuveiro = 0;
//Dados para sensor de chuva
//int pino_d = 2; //Pino ligado ao D0 do sensor
int pino_a = A5; //Pino ligado ao A0 do sensor
//int val_d = 0; //Armazena o valor lido do pino digital
int val_a = 0; //Armazena o valor lido do pino analogico

// Portas ligadas aos leds
int pino_led_arCond = 7;

const int R = 6;
const int G = 9;
const int B = 10;

const int buzzer = 13;

String data = "";

void setup()
{
  // Define os pinos do sensor de chuva como entrada
  //pinMode(pino_d, INPUT);
  pinMode(pino_a, INPUT);
  
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);

  // Define os pinos dos leds como saida
  pinMode(pino_led_arCond, OUTPUT);

  lcd.begin(16, 2);
  Serial.begin(115200);
  
  lcd.begin(16, 2);
  lcd.clear();
  //lcd.print("Primeiro Teste")
  dht.begin();
}

void loop()
{
  int val = 0;
  if(Serial.available() > 0) {
    char temp = Serial.read();
    data.concat(temp);
    if(data == "1\n") {
      val = 1;
      data = "";
    } else if(data == "2\n") {
      val = 2;
      data = "";
    } else if(data == "3\n") {
      val = 3;
      data = "";
    } else if(data == "4\n") {
      val = 4;
      data = "";
    } else if(data == "5\n") {
      val = 5;
      data = "";
    } else if(data == "6\n") {
      val = 6;
      data = "";
    }
  }
      
  switch (val){
    case 1:
      escreveLCD("Janela Fechada");
      break;
    case 2:
      escreveLCD("Janela Aberta");
      break;
    case 3:
      ligarArCond = 1;
      break;
    case 4:
      ligarArCond = 0;
      break;
    case 5:
        //Serial.print("oi");
        //Serial.print(temp);
      ligarChuveiro = 1;
      break;
    case 6:
      ligarChuveiro = 0;
      break;
    case 7:
      escreveLCD("Alarme Ativado");
      tone (buzzer, 440,1000);
      delay(1000);
      tone (buzzer, 440,1000);
      break;
    case 8:
      escreveLCD("Senha Incorreta");
      tone (buzzer, 440,1000);
      break;
    }
 
 //Limpa a tela
  //lcd.clear();
  
  //Ligar ar condicionado - fazer leitura do ar
  //indicar no painel que o ar esta ligado
    if (ligarArCond == 1){
      digitalWrite(pino_led_arCond, HIGH);
      escreveLCD("Ar condicionado ligado");
      flagAr = 1;
    }
    else if (ligarArCond == 0 && flagAr == 1){
      digitalWrite(pino_led_arCond, LOW);
      escreveLCD("Ar condicionado desligado");  
      flagAr = 2;
  }
  
  
  //Umidade e temperatura
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  // testa se retorno é valido, caso contrário algo está errado.
  //indicar no painel que o chuveiro esta ligado
  //Ligar chuveiro - fazer leitura do chuveiro
  if (ligarChuveiro == 1){//verificar se vai ligar o chuveiro de acordo com a temperatura ambiente. Cada cor representa uma intensidade de temperatura
    if (t > 10 && t < 20){//agua quente
      digitalWrite(R, HIGH);
      digitalWrite(G, LOW);
      digitalWrite(B, LOW);
    }
    else if (t >= 20 && t < 30)//agua morna
    {
      digitalWrite(R, LOW);
      digitalWrite(G, HIGH);
      digitalWrite(B, LOW);
    }
   else if (t >= 30)//agua gelada
   {
      digitalWrite(R, LOW);
      digitalWrite(G, LOW);
      digitalWrite(B, HIGH);
   }
   escreveLCD("Chuveiro Ligado");
   flagChuveiro = 1;
 }
 else if (ligarChuveiro == 0 && flagChuveiro == 1){
      digitalWrite(R, LOW);
      digitalWrite(G, LOW);
      digitalWrite(B, LOW);
      escreveLCD("Chuveiro Desligado");
      flagChuveiro = 2;
    }
    
    //painel de alarme
    
  delay(1000);
  lcd.clear();
}

void escreveLCD(String texto){
  lcd.print(texto);
   x = x + 1;//Para mudar de linha
    y = y + 1;//Para mudar de coluna
    if(x > 15){
      lcd.setCursor(0,2);
      x = 0;
    }
    if(y > 31){
      lcd.setCursor(0,0);
      y = 0;
      x = 0;
    } 
}
