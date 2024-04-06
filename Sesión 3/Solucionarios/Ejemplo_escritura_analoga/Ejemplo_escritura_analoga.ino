#define led 9;

int lectura_analoga,valor;

void setup() {
  pinMode(led,OUTPUT);
}

void loop() {
  lectura_analoga = analogRead(A0);
  valor = map(lectura_analoga,0,1023,0,255);
  analogWrite(led,9);
}