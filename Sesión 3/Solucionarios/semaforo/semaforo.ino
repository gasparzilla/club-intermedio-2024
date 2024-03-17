#define boton 9;
#define led1 10;
#define led2 11;
#define led3 12;

int estado_boton;

int flag=0; //variable para definir cual led está prendido
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
    flag = flag + 1
    if(flag==3){
      flag = 0;
    }
    //while para asegurarse que sólo se considere una vez la pulsación del botón
    while(estado_boton==0){
      estado_boton=digitalRead(boton);
    }    
  }
  if(flag==0){
    digitalWrite(led1,HIGH);
    digitalWrite(led2,LOW);
    digitalWrite(led3,LOW);
  }else if(flag==1){
    digitalWrite(led1,LOW);
    digitalWrite(led2,HIGH);
    digitalWrite(led3,LOW);
  }else if(flag==2){
    digitalWrite(led1,LOW);
    digitalWrite(led2,LOW);
    digitalWrite(led3,HIGH);
  }
}

