int nota1, nota2, nota3, nota4;
float promedio;

void setup() {
  //Se inicializa la comunicación serial
  Serial.begin(9600);
}

void loop() {
  Serial.println("Primera nota: ");
  //Este while true es para que sólo pregunte una vez y se quede esperando el input
  while (true) {
    if (Serial.available()) {
      //Se lee la entrada como un string, eso facilita las cosas
      String input = Serial.readStringUntil('\n');
      //Se transforma el string a int
      nota1 = input.toInt();
      break;
    }
  }
  Serial.println("Segunda nota: ");
  //Este while true es para que sólo pregunte una vez y se quede esperando el input
  while (true) {
    if (Serial.available()) {
      //Se lee la entrada como un string, eso facilita las cosas
      String input = Serial.readStringUntil('\n');
      //Se transforma el string a int
      nota2 = input.toInt();
      break;
    }
  }
  Serial.println("Tercera nota: ");
  //Este while true es para que sólo pregunte una vez y se quede esperando el input
  while (true) {
    if (Serial.available()) {
      //Se lee la entrada como un string, eso facilita las cosas
      String input = Serial.readStringUntil('\n');
      //Se transforma el string a int
      nota3 = input.toInt();
      break;
    }
  }
  Serial.println("Cuarta nota: ");
  //Este while true es para que sólo pregunte una vez y se quede esperando el input
  while (true) {
    if (Serial.available()) {
      //Se lee la entrada como un string, eso facilita las cosas
      String input = Serial.readStringUntil('\n');
      //Se transforma el string a int
      nota4 = input.toInt();
      //Se hace el cálculo del promedio
      promedio = (nota1 + nota2 + nota3 + nota4) / 4.0;
      //Se imprime el resultado
      Serial.print("El promedio es: ");
      Serial.println(promedio);
      break;
    }
  }
}
