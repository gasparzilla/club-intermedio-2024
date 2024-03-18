float x1,y1,r1,x2,y2,r2;


void setup() {
  //Se inicializa la comunicación serial
  Serial.begin(9600);
}

void loop() {
  Serial.println("X1:");
  //Este while true es para que sólo pregunte una vez y se quede esperando el input
  while (true) {
    if (Serial.available()) {
      //Se lee la entrada como un string, eso facilita las cosas
      String input = Serial.readStringUntil('\n');
      //Se transforma el string a float
      x1 = input.toFloat();
      break;
    }
  }
  
  Serial.println("Y1:");
  //Este while true es para que sólo pregunte una vez y se quede esperando el input
  while (true) {
    if (Serial.available()) {
      //Se lee la entrada como un string, eso facilita las cosas
      String input = Serial.readStringUntil('\n');
      //Se transforma el string a float
      y1 = input.toFloat();
      break;
    }
  }
  
  Serial.println("R1:");
  //Este while true es para que sólo pregunte una vez y se quede esperando el input
  while (true) {
    if (Serial.available()) {
      //Se lee la entrada como un string, eso facilita las cosas
      String input = Serial.readStringUntil('\n');
      //Se transforma el string a float
      r1 = input.toFloat();
      break;
    }
  }
  
  Serial.println("X2:");
  //Este while true es para que sólo pregunte una vez y se quede esperando el input
  while (true) {
    if (Serial.available()) {
      //Se lee la entrada como un string, eso facilita las cosas
      String input = Serial.readStringUntil('\n');
      //Se transforma el string a float
      x2 = input.toFloat();
      break;
    }
  }
  
  Serial.println("Y2:");
  //Este while true es para que sólo pregunte una vez y se quede esperando el input
  while (true) {
    if (Serial.available()) {
      //Se lee la entrada como un string, eso facilita las cosas
      String input = Serial.readStringUntil('\n');
      //Se transforma el string a float
      y2 = input.toFloat();
      break;
    }
  }
  
  Serial.println("R2:");
  //Este while true es para que sólo pregunte una vez y se quede esperando el input
  while (true) {
    if (Serial.available()) {
      //Se lee la entrada como un string, eso facilita las cosas
      String input = Serial.readStringUntil('\n');
      //Se transforma el string a float
      r2 = input.toFloat();
      break;
    }
  }
  
  //cálculo de la distancia entre los 2 puntos
  float distancia = sqrt(pow(x2-x1,2)+pow(y2-y1,2));  `

  //revisión de intersección
  if(distancia < (r1+r2)){
    Serial.println("Se intersectan");
  }else{
    Serial.println("No se intersectan");
  }  
}
