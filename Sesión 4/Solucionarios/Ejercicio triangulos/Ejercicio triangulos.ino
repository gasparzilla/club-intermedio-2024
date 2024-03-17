int a, b, c;
float promedio;
bool es_valido;
void setup() {
  //Se inicializa la comunicación serial
  Serial.begin(9600);
}

void loop() {
  Serial.println("Ingrese lado a: ");
  //Este while true es para que sólo pregunte una vez y se quede esperando el input
  while (true) {
    if (Serial.available()) {
      //Se lee la entrada como un string, eso facilita las cosas
      String input = Serial.readStringUntil('\n');
      //Se transforma el string a float
      a = input.toFloat();
      break;
    }
  }
  Serial.println("Ingrese lado b: ");
  //Este while true es para que sólo pregunte una vez y se quede esperando el input
  while (true) {
    if (Serial.available()) {
      //Se lee la entrada como un string, eso facilita las cosas
      String input = Serial.readStringUntil('\n');
      //Se transforma el string a float
      b = input.toFloat();
      break;
    }
  }

  Serial.println("Ingrese lado c: ");
  //Este while true es para que sólo pregunte una vez y se quede esperando el input
  while (true) {
    if (Serial.available()) {
      //Se lee la entrada como un string, eso facilita las cosas
      String input = Serial.readStringUntil('\n');
      //Se transforma el string a float
      c = input.toFloat();
      //Se revisa si el triangulo es válido
      if (((a < (b + c)) and (b < (a + c)) and (c < (a + b))) == true) {
        es_valido = true;
      } else {
        es_valido = false;
      }
      //Se imprime el resultado
      if (es_valido == true) {
        if (a==b and a==c and c==b){
          Serial.println("Es un triangulo equilatero");
        }else if(a==b or a==c or b==c){
          Serial.println("Es un triangulo isosceles");
        }else{
          Serial.println("Es un triangulo escaleno");
        }
      } else {
        Serial.println("No es un triangulo valido");
      }
      break;
    }
  }
}
