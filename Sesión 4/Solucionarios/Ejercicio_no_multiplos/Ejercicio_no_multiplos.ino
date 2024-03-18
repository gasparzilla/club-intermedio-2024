int numero, n = 1;
void setup() {
  //Se inicializa la comunicación serial
  Serial.begin(9600);
}

void loop() {
  Serial.println("Ingrese numero: ");
  //Este while true es para que sólo pregunte una vez y se quede esperando el input
  while (true) {
    if (Serial.available()) {
      //Se lee la entrada como un string, eso facilita las cosas
      String input = Serial.readStringUntil('\n');
      //Se transforma el string a int
      numero = input.toInt();
      //el código se ejecuta mientras n sea menor que el número ingresado
      while (n <= numero) {
        //Se verifica si los número no son múltiplos de 3 y 7
        if ((n % 3 != 0) and (n % 7 != 0)) {
          Serial.println(n);
        }
        n++;
      }
      break;
    }
  }
}
