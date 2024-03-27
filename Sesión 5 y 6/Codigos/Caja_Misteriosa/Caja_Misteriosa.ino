#include <Servo.h>

Servo servo;

// constantes correspondientes a los pines utilizados
const int pin_boton_buzzer = 10;
const int pin_buzzer = 16;
const int pin_boton_s = 8;
const int pin_boton_r = 9;
const int pins_led[4] = {4, 5, 6, 7};

// variables de estado
bool buzzer_off = true;

// variables
int nota = 0;
int nota_anterior = 0;
int potenciometro = 0;
int boton = 1;
int angulo = 0;
int angulo_anterior = 0;
int leds = 0;
int leds_ = 0;
int leds_anterior = 0;
int boton_s = 1;
int boton_r = 1;

// setup
void setup() {
  pinMode(pin_boton_buzzer, INPUT_PULLUP);
  pinMode(pin_boton_s, INPUT_PULLUP);
  pinMode(pin_boton_r, INPUT_PULLUP);
  pinMode(pin_buzzer, OUTPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(pins_led[i], OUTPUT);
  }
}

// loop
void loop() {
  // leer el potenciometro
  potenciometro = analogRead(A0);

  // asignar una nota de acuerdo a la posicion
  // del potenciometro
  nota = map(potenciometro, 0, 1023, 0, 10);
  nota_anterior = nota;
  nota = nota * 100 + 300 +(leds*10);

  boton = digitalRead(pin_boton_buzzer);
  if ((boton == 0) && ((buzzer_off) || (nota != nota_anterior))) {
    tone(pin_buzzer, nota);
    buzzer_off = false;
  }
  else if ((boton == 1) && (!buzzer_off)) {
    noTone(pin_buzzer);
    buzzer_off = true;
  }

  boton_s = digitalRead(pin_boton_s);
  boton_r = digitalRead(pin_boton_r);

  if (boton_s == 0) {
    leds += 1;
  }

  if (boton_r == 0) {
    leds -= 1;
  }

  if (leds != leds_anterior) {
    leds_anterior = leds;
    leds_ = leds;
    for (int i = 3; i >= 0; i--) {
      digitalWrite(pins_led[i], leds_ % 2);
      leds_ = leds_ / 2;
    }
    delay(200);
  }

}
