void setup() {
  //Se inicializa la comunicación serial
  Serial.begin(9600);
}

void loop() {
  Serial.println("Ingrese su nombre");
  //Este while true es para que sólo pregunte una vez y se quede esperando el input
  while (true) {
    if (Serial.available()) {
      //Se lee la entrada como un string, eso facilita las cosas
      String input = Serial.readStringUntil('\n');
      Serial.print("Hola, ");
      Serial.println(input);
      break;
    }
  }
}

