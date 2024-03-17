#define led 9;
int tiempo=2000;
void setup() {
  pinMode(led,OUTPUT);
}

void loop() {

  while(tiempo > 0){
    digitalWrite(pin,HIGH);
    delay(tiempo);
    digitalWrite(pin, LOW);
    delay(200);
    //reduccion de tiempo
    tiempo= tiempo-250;
  }
  //se reinicia el tiempo para volver a hacer el loop
  tiempo=2000;

}
