float radio, area, perimetro;

void setup() {
  //Se inicializa la comunicación serial
  Serial.begin(9600);
}

void loop() {
  Serial.println("Ingrese el radio");
  //Este while true es para que sólo pregunte una vez y se quede esperando el input
  while (true) {
    if (Serial.available()) {
      //Se lee la entrada como un string, eso facilita las cosas
      String input = Serial.readStringUntil('\n');
      //Se transforma el string a float
      radio = input.toFloat();
      //Se hacen los cálculos
      perimetro = 2 * 3.14 * radio;
      area = 3.14 * pow(radio, 2);
      //Se imprime el resultado
      Serial.print("El perimetro es: ");
      Serial.println(perimetro);
      Serial.print("El area es: ");
      Serial.println(area);
      //El break es para salir del while y volver a preguntar el radio
      break;
    }
  }
}
