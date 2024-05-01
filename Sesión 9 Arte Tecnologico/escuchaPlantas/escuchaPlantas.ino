/*------------
  MIDI_PsychoGalvanometer_USB v0.1.3
  Acepta pulsos de un sensor de conductividad galvanico
  (incluir link de explicacion)
  Este sensor consiste de un temporizador 555 configurado como un multivibrador aestable y dos electrodos
  (incluir link de explicacion)
  El muestreo del ancho de pulso emitido y la identificacion de sus 
  fluctuaciones, nos permite generar notas y mensajes de control (ejercicio para el lector)
  
  Podemos controlar el umbral de deteccion de los pulsos, la escala de notas
  el codigo de control (ejercicio para el lector).
  
  pagina del proyecto original (pasen a dar las gracias):
  	MIDIsprout.com

  github del proyecto original:
	github.com/electricityforprogress/BiodataSonificationBreadboardKit

  playlist de spotify recomendada:
  	open.spotify.com/playlist/2Nc83XS4yxrg5XmCbOInkQ
-------------*/

// libreria que nos permite enviar mensajes midi mediante USB 
#include <MIDIUSB.h>

// inicializacion de variables que controlan las notas
const int scaleCount = 1; // Cantidad de escalas de notas a usar
const int scaleLen = 23; // largo de la escala de notas usada + 1 
int currScale = 0; // escala de notas definida por defecto
int scale[scaleCount][scaleLen] = {
  {22, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22}, //Chromatic
};

int root = 0; //inicializar la raiz de las notas,cambiar para mover el pitch

// Inicializacion de variables que controlan el comportamiento 
// de la deteccion de pulsos
const byte interruptPin = INT0; //pin de interrupcion utilizado (D3 en pro micro)
int pulseRate = 350; //frecuencia base de deteccion de pulsos

const byte samplesize = 15; //configurar cantidad de muestras guardadas
const byte analysize = samplesize - 1;  // corte para el analisis

const byte polyphony = 5; //polifonia (cuantas notas suenan al mismo tiempo)
int channel = 1;  // canal midi a utilizar
int noteMin = 36; //C2  - nota minima a emitir
int noteMax = 96; //C7  - nota maxima a emitir
byte QY8 = 0; //activa o desactiva el modo QY8 que envia MIDI al azar en 4 canales
byte controlNumber = 80; //Numero de control MIDI 
byte controlVoltage = 1; // (pendiente traduccion y  verificacion) output PWM CV on controlLED, pin 17, PB3, digital 11 *lowpass filter

byte timeout = 0;
int value = 0;
int prevValue = 0;

volatile unsigned long microseconds; //timer de muestreo
volatile byte index = 0;
volatile unsigned long samples[samplesize];

float threshold = 1.7;   //cambiar el multiplicador del umbral


unsigned long previousMillis = 0;
unsigned long currentMillis = 1;

// Estructura basica del mensaje MIDI para notas y control
typedef struct _MIDImessage { 
  unsigned int type;
  int value;
  int velocity;
  long duration;
  long period;
  int channel;
}

MIDImessage;
MIDImessage noteArray[polyphony]; //para poder manejar los mensajes midi en un array
int noteIndex = 0;
MIDImessage controlMessage; //para poder manejar los mensajes de control

// configuracion previa al programa principal
void setup()
{
  randomSeed(analogRead(0)); //semilla aleatorea para el modo QY4 (emision aleatorea en 4 canales)
  //  Serial.begin(31250);  //Baudios para emision midi por serial
  //  Serial.begin(57600);  //Baudios para debugging
  //  while (!Serial) ;
  //  delay(500);
  //  Serial.println("waiting to start up...");
  //  delay(500);
  //  Serial.println("starting up...");

  controlMessage.value = 0;  //comenzar CV en 0
  //MIDIpanic(); // activar solo de ser necesario
  attachInterrupt(interruptPin, sample, RISING);  //comenzar muestreo del pin de interrupcion
}

// bloque principal del programa
void loop()
{
  currentMillis = millis();   // actualizar contador del tiempo
  if (index >= samplesize)  {
    analyzeSample();  //si el arreglo de muestreo esta lleno, algo que tambien se revisa en analyzeSample(), analizar el muestreo
  }
  checkNote();  //apagar notas que expiraron
  checkControl();  //actualizar el volumen de control
}

//muestreo en el pin de interrupcion
//	guarda cuanto tiempo ha pasado desde la ultima vez que se activa
//	la senal de interrupcion en el arreglo samples
void sample()
{
  if (index < samplesize) {
    samples[index] = micros() - microseconds;
    microseconds = samples[index] + microseconds; //reconstruir valor de micros sin llamar la funcion
    //micros() es muy lento
    //prueba un contador mas preciso
    //samples[index] = ((timer0_overflow_count << 8) + TCNT0) - microseconds;
    index += 1;
  }
}

// analizar la muestra obtenida
// 	una vez que se llena el arreglo de samples (tiempos entre las activaviones de la interrupcion)
// 	se analiza para saber que notas tocar
void analyzeSample()
{
  // inicializacion de variables para el analisis
  unsigned long averg = 0;
  unsigned long maxim = 0;
  unsigned long minim = 100000;
  float stdevi = 0;
  unsigned long delta = 0;
  byte change = 0;
  // chequea si el arreglo de muestreo esta lleno
  if (index == samplesize) {
    unsigned long sampanalysis[analysize];
    // para cada muestra
    for (byte i = 0; i < analysize; i++) {
      //nos saltamos la muestra inicial
      sampanalysis[i] = samples[i + 1]; //guardamos el valor de la muestra
      //guardamos el maximo
      if (sampanalysis[i] > maxim) {
        maxim = sampanalysis[i];
      }
      // guardamos el minimo
      if (sampanalysis[i] < minim) {
        minim = sampanalysis[i];
      }
      // preparamos los calculos del promedio y la desviacion estandar
      averg += sampanalysis[i];
      stdevi += sampanalysis[i] * sampanalysis[i];
      }

    //calculamos el promedio y la desviasion estandar
    averg = averg / analysize;
    stdevi = sqrt(stdevi / analysize - averg * averg);
    // corregimos para que la desviacion no sea menor a 1
    if (stdevi < 1) {
      stdevi = 1.0;  //min stdevi of 1
    }
    // calculamos la diferencia entre el maximo y el minimo
    delta = maxim - minim;

    //si la diferencia entre el maximo y el minimo es mayor a la
    // desviacion estandar multiplicada por el umbral de deteccion
    // la senal esta fluctuando
    if (delta > (stdevi * threshold)) {
      change = 1;
    }

    // si la senal esta fluctuando
    if (change) { 
      
      int dur = 150 + (map(delta % 127, 1, 127, 100, 2500)); // calculamos duracion de la nota
      int ramp = 3 + (dur % 100) ; // configurar el valor de control para el slide (permite "deslizar" las notas entre si)
      int notechannel = random(1, 5); //elegir un canal aleatoreo del 1 al 4 para modo QY8 

      //definir la escala, la raiz y la nota
      // se obtiene la nota a partir del promedio del tiempo de muestreo y se mapea entre la nota maxima y la minima definida
      int setnote = map(averg % 127, 1, 127, noteMin, noteMax);
      // se escala la nota obtenida segun la escala definida y la raiz
      setnote = scaleNote(setnote, scale[currScale], root); 
      // Si el modo QY8 esta activado se emite por el canal random
      if (QY8) {
        setNote(setnote, 100, dur, notechannel);  //set for QY8 mode
      }
      // si no, se emite por el canal definido
      else {
        setNote(setnote, 100, dur, channel);
      }

      //se obtienen los valores para emitir el control
      setControl(controlNumber, controlMessage.value, delta % 127, ramp); //set the ramp rate for the control
    }
    //reiniciamos el indice del arreglo de muestreo para la siguienteiteracion
    index = 0;
  }
}

// funcion para actualizar los parametros de control
// en caso de querer enviar senales midi de control
void setControl(int type, int value, int velocity, long duration)
{
  controlMessage.type = type;
  controlMessage.value = value;
  controlMessage.velocity = velocity;
  controlMessage.period = duration;
  controlMessage.duration = currentMillis + duration; //schedule for update cycle
}

// funcion para escalar una nota segun una escala y una raiz
int scaleNote(int note, int scale[], int root) {
  // calcula:
  // scaled -> nota modulo 12 para saber su posicion en una octava
  // octave -> nota dividida en 12 para saber la octava de la nota
  // busca la nota mas cercana en la escala definida
  // devuelve la nota escalada multiplicada por la octava
  int scaled = note % 12;
  int octave = note / 12;
  int scalesize = (scale[0]);
  // busca el arreglo devolviendo la nota mas cercana
  scaled = scaleSearch(scaled, scale, scalesize);
  scaled = (scaled + (12 * octave)) + root; //aplica la octava y la raiz
  return scaled;
}

// funcion que busca la escala y devuelve la nota mas cercana
int scaleSearch(int note, int scale[], int scalesize) {
  // para cada nota definida en la escala
  for (byte i = 1; i < scalesize; i++) {
  	// si la nota es igual a alguna posicion en la escala, devolverla
    if (note == scale[i]) {
      return note;
    }
    // si la nota es menor a la definida por la escala,devolver la nota mas cercana
    else {
      if (note < scale[i]) {
        return scale[i];
      }
    }
    //continuar la busqueda
  }
  //devolver un valor en caso de fallar
  return 6;
}

// funcion que actualiza el mensaje midi a emitir
void setNote(int value, int velocity, long duration, int notechannel)
{
  // encontrar una nota disponible en el arreglo de notas emitidas
  // veolcity = 0
  for (int i = 0; i < polyphony; i++) {
    if (!noteArray[i].velocity) {
      // si la velocidad de una nota es 0, reemplazarla en el arreglo
      noteArray[i].type = 0;
      noteArray[i].value = value;
      noteArray[i].velocity = velocity;
      noteArray[i].duration = currentMillis + duration;
      noteArray[i].channel = notechannel;

	  // modo qy8 notechannel es aleatoreo de 1 a 4
      if (QY8) {
        midiSerial(144, notechannel, value, velocity, 1);
      }
      // modo normal
      else {
        midiSerial(144, channel, value, velocity, 1);
      }

      break;
    }
  }
}

// funcion para revisar las notas que estan sonando
void checkNote()
{
  // para cada nota que suena en el arreglo
  for (int i = 0; i < polyphony; i++) {
    if (noteArray[i].velocity) {
      if (noteArray[i].duration <= currentMillis) {
        //apagar todas las notas que han expirado
        if (QY8) {
          midiSerial(144, noteArray[i].channel, noteArray[i].value, 0, 0);
        }
        else {
          midiSerial(144, channel, noteArray[i].value, 0, 0);
        }
        // marcar la nota como apagada
        noteArray[i].velocity = 0;
      }
    }
  }
}

// lo mismo que checkNote pero para la instruccion de control
void checkControl()
{
  //la distancia es el valor actual menos el objetivo
  signed int distance =  controlMessage.velocity - controlMessage.value;
  //si seguimos deslizando
  if (distance != 0) {
    //revisamos el tiempo
    if (currentMillis > controlMessage.duration) { //si la duracion expiro
      controlMessage.duration = currentMillis + controlMessage.period; //extendemos la duracion
      //actualizar el valor
      if (distance > 0) {
        controlMessage.value += 1;
      } else {
        controlMessage.value -= 1;
      }

      //enviar el mensaje de control midi
      midiSerial(176, channel, controlMessage.type, controlMessage.value, 1);
    }
  }
}

// funcion que envia la el mensaje midi por usb
void midiSerial(int type, int channel, int data1, int data2, int onOrOff) {

  cli(); //detiene la interrupcion, posiblemente innecesario

  //  Note type = 144
  //  Control type = 176

  // encendido de notas
  if (type == 144 && onOrOff == 1) {
    midiEventPacket_t noteOn = {0x09, 0x90 | 0, data1, data2};
    MidiUSB.sendMIDI(noteOn);
    MidiUSB.flush();
  }
  // apagado de notas
  else if (type == 144 && onOrOff == 0) {
    midiEventPacket_t noteOff = {0x08, 0x80 | 0, data1, data2};
    MidiUSB.sendMIDI(noteOff);
    MidiUSB.flush();
  }
  // senal de control
  else if (type == 176) {
    midiEventPacket_t event = {0x0B, 0xB0 | 0, data1, data2};
    MidiUSB.sendMIDI(event);
    MidiUSB.flush();
  }

  sei(); //vuelve a permitir interrupciones
}

// Panico MIDI, apaga todas las notas
void MIDIpanic()
{
  // apaga todas las notas con fuerza bruta
  for (byte i = 1; i < 128; i++) {
    delay(1); 
    midiSerial(144, channel, i, 0, 0); //apaga todas las notas del canal usado

	// lo mismo para el modo QY8
    if (QY8) { 
      for (byte k = 1; k < 5; k++) {
        delay(1);
        midiSerial(144, k, i, 0, 0);
      }
    }
  }
}
