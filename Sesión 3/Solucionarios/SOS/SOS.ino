#define led 9;
void setup() {
  pinMode(led,OUTPUT);
}

void loop() {
  //S = ...
  digitalWrite(pin,HIGH);
  delay(300);
  digitalWrite(pin,LOW);
  delay(300);
  digitalWrite(pin,HIGH);
  delay(300);
  digitalWrite(pin,LOW);
  delay(300);
  digitalWrite(pin,HIGH);
  delay(300);
  digitalWrite(pin,LOW);
  delay(300);

  delay(700);

  //O = ---
  digitalWrite(pin,HIGH);
  delay(700);
  digitalWrite(pin,LOW);
  delay(300);
  digitalWrite(pin,HIGH);
  delay(700);
  digitalWrite(pin,LOW);
  delay(300);
  digitalWrite(pin,HIGH);
  delay(700);
  digitalWrite(pin,LOW);
  delay(300);

  delay(700);

  //S = ...
  digitalWrite(pin,HIGH);
  delay(300);
  digitalWrite(pin,LOW);
  delay(300);
  digitalWrite(pin,HIGH);
  delay(300);
  digitalWrite(pin,LOW);
  delay(300);
  digitalWrite(pin,HIGH);
  delay(300);
  digitalWrite(pin,LOW);
  delay(300);

}
