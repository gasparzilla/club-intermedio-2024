#define led 9;
int lectura_analoga;

void setup() {
  pinMode(led,OUTPUT);
}

void loop() {
  lectura_analoga = analogRead(A0);
  digitalWrite(led,HIGH);
  delay(lectura_analoga);
  digitalWrite(led,LOW);
  delay(lectura_analoga);
}