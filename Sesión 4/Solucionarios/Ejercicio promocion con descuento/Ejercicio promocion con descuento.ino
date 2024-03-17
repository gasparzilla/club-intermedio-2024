int n, productos, total = 0, descuento_total = 0, descuento = 20, temp = 1,n_temp;
void setup() {
  //Se inicializa la comunicación serial
  Serial.begin(9600);
}

void loop() {
  Serial.println("n: ");
  //Este while true es para que sólo pregunte una vez y se quede esperando el input
  while (true) {
    if (Serial.available()) {
      //Se lee la entrada como un string, eso facilita las cosas
      String input = Serial.readStringUntil('\n');
      //Se transforma el string a int
      n = input.toInt();
      break;
    }
  }
  Serial.println("Cantidad de productos: ");
  //Este while true es para que sólo pregunte una vez y se quede esperando el input
  while (true) {
    if (Serial.available()) {
      //Se lee la entrada como un string, eso facilita las cosas
      String input = Serial.readStringUntil('\n');
      //Se transforma el string a int
      productos = input.toInt();
      break;
    }
  }
  for (int i = 1; i <= productos;i++) {
    Serial.print("Precio producto ");
    Serial.println(i);
    //Este while true es para que sólo pregunte una vez y se quede esperando el input
    while (true) {
      if (Serial.available()) {
        //Se lee la entrada como un string, eso facilita las cosas
        String input = Serial.readStringUntil('\n');
        //Se transforma el string a int
        n_temp = input.toInt();
        break;
      }
    }
    total += n_temp;
    descuento_total += (n_temp * descuento) / 100;
    temp++;
    if (temp > n) {
      temp = 1;
      descuento = descuento / 2;
    }
  }
  Serial.print("Total: ");
  Serial.println(total);
  Serial.print("Descuento: ");
  Serial.println(descuento_total);
  Serial.print("Por pagar: ");
  Serial.println(total-descuento_total);
}
