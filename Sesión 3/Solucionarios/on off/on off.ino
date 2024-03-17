#define boton 9;
#define led 10;

int estado_boton;

int flag=0; //variable del segundo loop para guardar el estado de la led (prendido o apagado)
void setup() {
  // put your setup code here, to run once:
  pinMode(boton, INPUT_PULLUP);
  pinMode(led,OUTPUT);
}

//en este código están las 2 formas diferentes que se piden, es por eso que hay 2 loops, borrar uno para testear

//La luz LED deberá encenderse solo cuando el botón esté apretado.
void loop() {
  estado_boton=digitalRead(boton);
  while(estado_boton==0){
    digitalWrite(led,HIGH);
    estado_boton=digitalRead(boton);
  }
  digitalWrite(led,LOW);
}
//La luz LED deberá encenderse cuando se apriete el botón y deberá apagarse cuando se vuelva a apretar el botón
void loop() {
  estado_boton=digitalRead(boton);
  if(estado_boton==0){
    if (flag==1){
      flag=0;
    }else if(flag==0){
      flag=1;
    }
    //while para asegurarse que sólo se considere una vez la pulsación del botón
    while(estado_boton==0){
      estado_boton=digitalRead(boton);
    }    
  }
  if(flag==0){
    digitalWrite(led,LOW);
  }else if(flag==1){
    digitalWrite(led,HIGH);
  }
}
