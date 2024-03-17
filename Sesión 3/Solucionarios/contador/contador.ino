#define boton 9;
#define led1 10;
#define led2 11;
#define led3 12;

int estado_boton;

int contador=0; //variable para definir cantidad de leds prendidas
void setup() {
  // put your setup code here, to run once:
  pinMode(boton, INPUT_PULLUP);
  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);
  pinMode(led3,OUTPUT);
}


void loop() {
  estado_boton=digitalRead(boton);
  if(estado_boton==0){
    contador = contador + 1
    if(contador==4){
      contador = 0;
    }
    //while para asegurarse que sólo se considere una vez la pulsación del botón
    while(estado_boton==0){
      estado_boton=digitalRead(boton);
    }    
  }
  if(contador==0){
    digitalWrite(led1,LOW);
    digitalWrite(led2,LOW);
    digitalWrite(led3,LOW);
  }else if(contador==1){
    digitalWrite(led1,HIGH);
    digitalWrite(led2,LOW);
    digitalWrite(led3,LOW);
  }else if(contador==2){
    digitalWrite(led1,HIGH);
    digitalWrite(led2,HIGH);
    digitalWrite(led3,LOW);
  }else if(contador==3){
    digitalWrite(led1,HIGH);
    digitalWrite(led2,HIGH);
    digitalWrite(led3,HIGH);
  }
}

