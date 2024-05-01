/*------------
  MIDI_PsychoGalvanometer v026
  Acepta pulsos de un sensor de conductividad galvanico
  (incluir link de explicacion)
  Este sensor consiste de un temporizador 555 configurado como un multivibrador aestable y dos electrodos
  (incluir link de explicacion)
  El muestreo del ancho de pulso emitido y la identificacion de sus 
  fluctuaciones, nos permite generar notas y mensajes de control (ejercicio para el lector)
  
  Podemos controlar el umbral de deteccion de los pulsos, la escala de notas
  el codigo de control (ejercicio para el lector).
  
  pagina del proyecto original (pasen a dar las gracias): MIDIsprout.com
  github del proyecto original:

  playlist de spotify recomendada:
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
void analyzeSample()
{
  unsigned long averg = 0;
  unsigned long maxim = 0;
  unsigned long minim = 100000;
  float stdevi = 0;
  unsigned long delta = 0;
  byte change = 0;
  // chequea si el arreglo de muestreo esta lleno
  if (index == samplesize) {
    unsigned long sampanalysis[analysize];
    for (byte i = 0; i < analysize; i++) {
      //skip first element in the array
      sampanalysis[i] = samples[i + 1]; //load analysis table (due to volitle)
      //manual calculation
      if (sampanalysis[i] > maxim) {
        maxim = sampanalysis[i];
      }
      if (sampanalysis[i] < minim) {
        minim = sampanalysis[i];
      }
      averg += sampanalysis[i];
      stdevi += sampanalysis[i] * sampanalysis[i];  //prep stdevi
    }

    //manual calculation
    averg = averg / analysize;
    stdevi = sqrt(stdevi / analysize - averg * averg); //calculate stdevu
    if (stdevi < 1) {
      stdevi = 1.0;  //min stdevi of 1
    }
    delta = maxim - minim;

    //**********perform change detection
    if (delta > (stdevi * threshold)) {
      change = 1;
    }
    //*********

    if (change) { // set note and control vector
      int dur = 150 + (map(delta % 127, 1, 127, 100, 2500)); //length of note
      int ramp = 3 + (dur % 100) ; //control slide rate, min 25 (or 3 ;)
      int notechannel = random(1, 5); //gather a random channel for QY8 mode

      //set scaling, root key, note
      int setnote = map(averg % 127, 1, 127, noteMin, noteMax); //derive note, min and max note
      setnote = scaleNote(setnote, scale[currScale], root);  //scale the note
      // setnote = setnote + root; // (apply root?)
      if (QY8) {
        setNote(setnote, 100, dur, notechannel);  //set for QY8 mode
      }
      else {
        setNote(setnote, 100, dur, channel);
      }

      //derive control parameters and set
      setControl(controlNumber, controlMessage.value, delta % 127, ramp); //set the ramp rate for the control
    }
    //reset array for next sample
    index = 0;
  }
}

void setControl(int type, int value, int velocity, long duration)
{
  controlMessage.type = type;
  controlMessage.value = value;
  controlMessage.velocity = velocity;
  controlMessage.period = duration;
  controlMessage.duration = currentMillis + duration; //schedule for update cycle
}

int scaleNote(int note, int scale[], int root) {
  //input note mod 12 for scaling, note/12 octave
  //search array for nearest note, return scaled*octave
  int scaled = note % 12;
  int octave = note / 12;
  int scalesize = (scale[0]);
  //search entire array and return closest scaled note
  scaled = scaleSearch(scaled, scale, scalesize);
  scaled = (scaled + (12 * octave)) + root; //apply octave and root
  return scaled;
}

int scaleSearch(int note, int scale[], int scalesize) {
  for (byte i = 1; i < scalesize; i++) {
    if (note == scale[i]) {
      return note;
    }
    else {
      if (note < scale[i]) {
        return scale[i];  //highest scale value less than or equal to note
      }
    }
    //otherwise continue search
  }
  //didn't find note and didn't pass note value, uh oh!
  return 6;//give arbitrary value rather than fail
}


void setNote(int value, int velocity, long duration, int notechannel)
{
  //find available note in array (velocity = 0);
  for (int i = 0; i < polyphony; i++) {
    if (!noteArray[i].velocity) {
      //if velocity is 0, replace note in array
      noteArray[i].type = 0;
      noteArray[i].value = value;
      noteArray[i].velocity = velocity;
      noteArray[i].duration = currentMillis + duration;
      noteArray[i].channel = notechannel;


      if (QY8) {
        midiSerial(144, notechannel, value, velocity, 1);
      }
      else {
        midiSerial(144, channel, value, velocity, 1);
      }

      break;
    }
  }
}

void checkNote()
{
  for (int i = 0; i < polyphony; i++) {
    if (noteArray[i].velocity) {
      if (noteArray[i].duration <= currentMillis) {
        //send noteOff for all notes with expired duration
        if (QY8) {
          midiSerial(144, noteArray[i].channel, noteArray[i].value, 0, 0);
        }
        else {
          midiSerial(144, channel, noteArray[i].value, 0, 0);
        }
        noteArray[i].velocity = 0;
      }
    }
  }
}

void checkControl()
{
  //need to make this a smooth slide transition, using high precision
  //distance is current minus goal
  signed int distance =  controlMessage.velocity - controlMessage.value;
  //if still sliding
  if (distance != 0) {
    //check timing
    if (currentMillis > controlMessage.duration) { //and duration expired
      controlMessage.duration = currentMillis + controlMessage.period; //extend duration
      //update value
      if (distance > 0) {
        controlMessage.value += 1;
      } else {
        controlMessage.value -= 1;
      }

      //send MIDI control message after ramp duration expires, on each increment
      midiSerial(176, channel, controlMessage.type, controlMessage.value, 1);
    }
  }
}

void midiSerial(int type, int channel, int data1, int data2, int onOrOff) {

  cli(); //kill interrupts, probably unnessisary
  //  Note type = 144
  //  Control type = 176
  // remove MSBs on data

  if (type == 144 && onOrOff == 1) {
    midiEventPacket_t noteOn = {0x09, 0x90 | 0, data1, data2};
    MidiUSB.sendMIDI(noteOn);
    MidiUSB.flush();
  }
  else if (type == 144 && onOrOff == 0) {
    midiEventPacket_t noteOff = {0x08, 0x80 | 0, data1, data2};
    MidiUSB.sendMIDI(noteOff);
    MidiUSB.flush();
  }
  else if (type == 176) {
    midiEventPacket_t event = {0x0B, 0xB0 | 0, data1, data2};
    MidiUSB.sendMIDI(event);
    MidiUSB.flush();
  }

  //Serial.println(data1);
  //Serial.println(data2);
  //Serial.println(channel);

  //Serial.write(statusbyte);
  //Serial.write(data1);
  //Serial.write(data2);

  sei(); //enable interrupts
}

void MIDIpanic()
{
  //120 - all sound off
  //123 - All Notes off
  // midiSerial(21, panicChannel, 123, 0); //123 kill all notes

  //brute force all notes Off
  for (byte i = 1; i < 128; i++) {
    delay(1); //don't choke on note offs!
    midiSerial(144, channel, i, 0, 0); //clear notes on main channel

    if (QY8) { //clear on all four channels
      for (byte k = 1; k < 5; k++) {
        delay(1); //don't choke on note offs!
        midiSerial(144, k, i, 0, 0);
      }
    }
  }
}
