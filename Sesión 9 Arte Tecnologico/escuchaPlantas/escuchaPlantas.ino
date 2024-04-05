/*------------
  MIDI_PsychoGalvanometer v026
  Accepts pulse inputs from a Galvanic Conductance sensor
  consisting of a 555 timer set as an astablemultivibrator and two electrodes.
  Through sampling pulse widths and identifying fluctuations, MIDI note and control messages
  are generated.  Features include Threshold, Scaling, Control Number, and Control Voltage
  using PWM through an RC Low Pass filter.
  MIDIsprout.com
  -------------*/
#include <MIDIUSB.h>
#include <EEPROMex.h> //store and read variables to nonvolitle memory
#include <Bounce2.h> //https://github.com/thomasfredericks/Bounce-Arduino-Wiring
#include <LEDFader.h> //manage LEDs without delay() jgillick/arduino-LEDFader https://github.com/jgillick/arduino-LEDFader.git
int maxBrightness = 190;

//******************************
//set scaled values, sorted array, first element scale length
const int scaleCount = 5;
const int scaleLen = 23; //maximum scale length plus 1 for 'used length'
int currScale = 0; //current scale, default Chrom
int scale[scaleCount][scaleLen] = {
  {22, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22}, //Chromatic
  {7, 1, 3, 5, 6, 8, 10, 12}, //Major
  {7, 1, 3, 4, 6, 8, 9, 11}, //DiaMinor
  {7, 1, 2, 2, 5, 6, 9, 11}, //Indian
  {7, 1, 3, 4, 6, 8, 9, 11} //Minor
};

int root = 0; //initialize for root, pitch shifting
//*******************************

const byte interruptPin = INT0; //galvanometer input
const byte knobPin = A0; //knob analog input
Bounce button = Bounce(); //debounce button using Bounce2
const byte buttonPin = A1; //tact button input
int menus = 5; //number of main menus
int mode = 0; //0 = Threshold, 1 = Scale, 2 = Brightness
int currMenu = 0;
int pulseRate = 350; //base pulse rate

const byte samplesize = 15; //set sample array siz
const byte analysize = samplesize - 1;  //trim for analysis array

const byte polyphony = 5; //above 8 notes may run out of ram
int channel = 1;  //setting channel to 11 or 12 often helps simply computer midi routing setups
int noteMin = 36; //C2  - keyboard note minimum
int noteMax = 96; //C7  - keyboard note maximum
byte QY8 = 0; //sends each note out chan 1-4, for use with General MIDI like Yamaha QY8 sequencer
byte controlNumber = 80; //set to mappable control, low values may interfere with other soft synth controls!!
byte controlVoltage = 1; //output PWM CV on controlLED, pin 17, PB3, digital 11 *lowpass filter
long batteryLimit = 3000; //voltage check minimum, 3.0~2.7V under load; causes lightshow to turn off (save power)
byte checkBat = 1;

byte timeout = 0;
int value = 0;
int prevValue = 0;

volatile unsigned long microseconds; //sampling timer
volatile byte index = 0;
volatile unsigned long samples[samplesize];

float threshold = 1.7;   //change threshold multiplier


unsigned long previousMillis = 0;
unsigned long currentMillis = 1;
unsigned long batteryCheck = 0; //battery check delay timer
unsigned long menuTimeout = 5000; //5 seconds timeout in menu mode

typedef struct _MIDImessage { //build structure for Note and Control MIDImessages
  unsigned int type;
  int value;
  int velocity;
  long duration;
  long period;
  int channel;
}

MIDImessage;
MIDImessage noteArray[polyphony]; //manage MIDImessage data as an array with size polyphony
int noteIndex = 0;
MIDImessage controlMessage; //manage MIDImessage data for Control Message (CV out)


void setup()
{
  pinMode(knobPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  button.attach(buttonPin);
  button.interval(5);

  randomSeed(analogRead(0)); //seed for QY8 4 channel mode
  //Serial.begin(31250);  //initialize at MIDI rate
  // Serial.begin(57600); //for debugging
  //  while (!Serial) ;
  //  delay(500);
  //  Serial.println("waiting to start up...");
  //  delay(500);
  //  Serial.println("starting up...");

  controlMessage.value = 0;  //begin CV at 0
  //MIDIpanic(); //dont panic, unless you are sure it is nessisary
  attachInterrupt(interruptPin, sample, RISING);  //begin sampling from interrupt
}

void loop()
{
  currentMillis = millis();   //manage time
  if (index >= samplesize)  {
    analyzeSample();  //if samples array full, also checked in analyzeSample(), call sample analysis
  }
  checkNote();  //turn off expired notes
  checkControl();  //update control value
}

//interrupt timing sample array
void sample()
{
  if (index < samplesize) {
    samples[index] = micros() - microseconds;
    microseconds = samples[index] + microseconds; //rebuild micros() value w/o recalling
    //micros() is very slow
    //try a higher precision counter
    //samples[index] = ((timer0_overflow_count << 8) + TCNT0) - microseconds;
    index += 1;
  }
}


void analyzeSample()
{
  //eating up memory, one long at a time!
  unsigned long averg = 0;
  unsigned long maxim = 0;
  unsigned long minim = 100000;
  float stdevi = 0;
  unsigned long delta = 0;
  byte change = 0;

  if (index == samplesize) { //array is full
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
