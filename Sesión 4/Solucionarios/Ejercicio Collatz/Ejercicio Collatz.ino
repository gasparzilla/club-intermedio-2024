int n;
void setup() {
  //Se inicializa la comunicación serial
  Serial.begin(9600);
}

void loop() {
  Serial.println("Ingrese n: ");
  //Este while true es para que sólo pregunte una vez y se quede esperando el input
  while (true) {
    if (Serial.available()) {
      //Se lee la entrada como un string, eso facilita las cosas
      String input = Serial.readStringUntil('\n');
      //Se transforma el string a int
      n = input.toInt();
      Serial.print(n);
      Serial.print(" ");
      while (n != 1) {
        //Si al dividir por 2 el resto es 0, significa que el número es par
        if (n % 2 == 0) {
          n = n / 2;
          Serial.print(n);
          Serial.print(" ");
        } else {
          n = (n * 3) + 1;
          Serial.print(n);
          Serial.print(" ");
        }
      }
      Serial.println();
      break;
    }
  }
}
