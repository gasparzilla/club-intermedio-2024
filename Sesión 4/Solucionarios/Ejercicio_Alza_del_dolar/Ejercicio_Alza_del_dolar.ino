int n;
float maximo=0;
float anterior,actual;

void setup() {
  //Se inicializa la comunicación serial
  Serial.begin(9600);
}

void loop() {
  Serial.println("Cuantos dias?");
  //Este while true es para que sólo pregunte una vez y se quede esperando el input
  while (true) {
    if (Serial.available()) {
      //Se lee la entrada como un string, eso facilita las cosas
      String input = Serial.readStringUntil('\n');
      //Se transforma el string a float
      n = input.toFloat();
      break;
    }
  }
  for(int i=1;i<=n;i++){
    Serial.print("Dia ");
    Serial.println(i);
    while (true) {
      if (Serial.available()) {
        //Se lee la entrada como un string, eso facilita las cosas
        String input = Serial.readStringUntil('\n');
        //Se transforma el string a float
        actual = input.toFloat();
        break;
      }
    }
    if (i==1){
      anterior = actual;
    }
    float temp = actual - anterior;
    if (temp > maximo){
      maximo = temp;
    }
    anterior = actual;
  }
  if(maximo >0){
    Serial.print("La mayor alza fue de $");
    Serial.println(maximo);    
  }else{
    Serial.println("No hubo alzas");
  }  
}
