//Este código funciona con configuracion pullUp y pullDown

//definicion pin
#define boton 9;

int estado_boton;
void setup() {
  //seteo modo pin
  pinMode(boton,INPUT);
  //Se inicia la comunicación Serial
  Serial.begin(9600);
}

void loop() {
  //Lectura estado del boton
  estado_boton=digitalRead(boton);

  //Se imprime por consola serial el estado del botón
  Serial.print("Estado boton: ");
  Serial.println(boton);
}
