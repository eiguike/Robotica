const int buzzer = 13;

//Dados para sensor de chuva
int pino_d = 9; //Pino ligado ao D0 do sensor
int pino_a = A5; //Pino ligado ao A0 do sensor
int val_d = 0; //Armazena o valor lido do pino digital
int val_a = 0; //Armazena o valor lido do pino analogico
int pino_led_verde = 10;
int pino_led_vermelho = 11;
//Dados para o teclado
const int numLinhas = 4; //número de linhas do teclado
const int numColunas = 3; //número de colunas do teclado
const int debounce = 20; //tempo de pausa

String senha = "1234";
String senhaDigitada;
const char teclado[numLinhas][numColunas] = { //definição do teclado
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}};

const int PinosLINHAS[numLinhas] = { 7, 2, 3, 5}; //pinos do Arduino que correspondem às linhas do teclado
const int PinosCOLUNAS[numColunas] = { 6, 8, 4}; //pinos do Arduino que correspondem às colunas do teclado
const char kNADA = 0; //constante que simboliza nenhuma tecla pressionada (deve ser diferente de todos os valores em teclado[][] - o 0 desta variável não é o 0 do teclado)

// variáveis necesárias para o relé e o motor elétrico
const int rele1 = X;
const int rele2 = X;
unsigned long int time;
int flagMotor = 0;

//---------------------------------------------------------------------------------

void setup(){
  Serial.begin(9600); //inicializa a Serial

  //configura as linhas como entradas com padrão em nível lógico alto
  for(int linha = 0; linha < numLinhas; linha++){
    pinMode(PinosLINHAS[linha], INPUT);
    digitalWrite(PinosLINHAS[linha], HIGH); //habilita pull-up
  }
  //configura as colunas como saídas com padrão em nível lógico alto
  for(int coluna = 0; coluna < numColunas; coluna++){
    pinMode(PinosCOLUNAS[coluna], OUTPUT);
    digitalWrite(PinosCOLUNAS[coluna], HIGH);

    // Define os pinos do sensor como entrada
    pinMode(pino_d, INPUT);
    pinMode(pino_a, INPUT);
    pinMode(pino_led_verde, OUTPUT);
    pinMode(pino_led_vermelho, OUTPUT);
  }

  // Configurando pinos do Relé
  pinMode(rele1, OUTPUT); // fechando
  pinMode(rele2, OUTPUT); // abrindo

  //Serial.println("--- pressione uma tecla ---");
}
//---------------------------------------------------------------------------------
void loop(){
  // verificação pra desativar o motor depois de um certo tempo
  if(flagMotor == 1){
    // depois de 0,5 segundos, é desligado os reles
    if((time+500) >= milis()){
      digitalWrite(rele1, LOW);
      digitalWrite(rele2, LOW);
    }
    flagMotor = 0;
  }

  // recebe comandos de forma serial do computador
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
    } else { // não preciso tratar outros comandos
      val = -1;
      data = "";
    }
  }

  switch (val){
    case 1:
      // FECHANDO JANELA
      digitalWrite(rele2, LOW);
      digitalWrite(rele1, HIGH);
      time = milis();
      flagMotor = 1;
      break;
    case 2:
      // ABRINDO JANELA
      digitalWrite(rele1, LOW);
      digitalWrite(rele2, HIGH);
      time = milis();
      flagMotor = 1;
      break;
    default:
      break;
  }

  //Le e arnazena o valor do pino digital
  val_d = digitalRead(pino_d);
  //Le e armazena o valor do pino analogico
  val_a = analogRead(pino_a);

  /*
     Serial.print(" - Valor analogico : ");
     Serial.println(val_a);
   */
  if (val_a >900 && val_a <1024)
  {
    // Acende led verde - intensidade baixa
    digitalWrite(pino_led_verde, LOW);
    digitalWrite(pino_led_vermelho, LOW);

    // abrindo janela
    digitalWrite(rele1, LOW);
    digitalWrite(rele2, HIGH);
    time = milis();
    flagMotor = 1;
  }
  //verifica qual o nível de chuva
  if (val_a >400 && val_a <900)
  {
    // Acende led amarelo - intensidade moderada
    digitalWrite(pino_led_verde, HIGH);
    digitalWrite(pino_led_vermelho, LOW);
  }
  if (val_a > 0 && val_a <400)
  {
    // Acende led vermelho - intensidade alta
    digitalWrite(pino_led_verde, LOW);
    digitalWrite(pino_led_vermelho, HIGH);

    // fechando janela
    digitalWrite(rele2, LOW);
    digitalWrite(rele1, HIGH);
    time = milis();
    flagMotor = 1;
  }

  //teclado de senha
  char tecla = TeclaPressionada();
  if(tecla != kNADA){
    Serial.print("Tecla pressionada: ");
    Serial.println(tecla);
  }

  //delay(1000);
}

//Determina qual tecla foi pressionada
char TeclaPressionada(){
  char tecla = kNADA; //atribui o valor padrão de retorno (nenhuma tecla pressionada)
  boolean achou = false; //inicialmente considera que nenhuma tecla foi pressionada
  for(int coluna = 0; coluna < numColunas; coluna++){
    digitalWrite(PinosCOLUNAS[coluna], LOW); //muda o estado do pino
    //varre as linhas procurando por uma tecla pressionada
    for(int linha = 0; linha < numLinhas; linha++){
      //lê linha pela primeira vez
      if(digitalRead(PinosLINHAS[linha]) == LOW){ //tecla está pressionada
        delay(debounce); //insere pausa para descartar ruídos
      }
      //lê linha pela segunda vez
      if(digitalRead(PinosLINHAS[linha]) == LOW){ //tecla continua pressionada, portanto não é um ruído
        while(digitalRead(PinosLINHAS[linha]) != HIGH); //espera soltar o tecla para retornar
        tecla = teclado[linha][coluna]; //determina qual foi a tecla pressionada de acordo com o teclado definido
        achou = true; //uma tecla foi pressionada
        break; //sai da varredura das linhas
      }
    }
    digitalWrite(PinosCOLUNAS[coluna], HIGH); //retorna o pino ao seu estado inicial 
    //se uma tecla foi pressionada, sai do laço for
    if(achou){
      if (tecla != '#'){
        senhaDigitada.concat(tecla);
        Serial.print(senhaDigitada);
      }
      if (tecla == '#'){
        if (senhaDigitada.equals(senha)){
          Serial.println("Senha correta");
          senhaDigitada = "\0";
          tone (buzzer, 440,100);
          delay(1000);
          tone (buzzer, 440,100);
        }
        else{
          Serial.print("Senha incorreta ");
          Serial.println(senhaDigitada);
          tone (buzzer, 440,1000);
        }
      }
      if (tecla == '*')
        senhaDigitada = "\0";
      break;
    }
  }

  return tecla; //retorna a tecla pressionada (kNADA se nenhuma foi encontrada)
}
