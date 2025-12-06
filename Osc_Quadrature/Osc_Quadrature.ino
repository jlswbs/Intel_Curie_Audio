// Simple quadrature LUT oscillator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define TABLE_SIZE  1024

float sineTable[TABLE_SIZE];
float cosTable[TABLE_SIZE];
float phaseC = 0.0f;
float phaseIncC = 0.0f;
float freq = 440.0f;

float mixI = 0.5f; 
float mixQ = 0.5f;

int time;
const int oneSecInUsec = 1000000;

void sample() {

  phaseC += phaseIncC;
  if (phaseC >= TABLE_SIZE) phaseC -= TABLE_SIZE;
  int idx = (int)phaseC & (TABLE_SIZE - 1);

  float I = cosTable[idx];
  float Q = sineTable[idx];
  float val = mixI * I + mixQ * Q;

  val *= 0.5f;
  uint16_t out = (uint16_t)(32767.0f * (1.0f + val));

  analogWrite(AUDIO, out);

  CurieTimerOne.restart(time);

}

void setup() {

  pinMode(AUDIO, OUTPUT);

  analogWriteResolution(16);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);

  for (int i = 0; i < TABLE_SIZE; i++) {

    sineTable[i] = sinf(2.0f * PI * i / TABLE_SIZE);
    cosTable[i]  = cosf(2.0f * PI * i / TABLE_SIZE);

  }

  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

}

void loop() {

  freq = random(50, 880);
  phaseIncC = (freq * TABLE_SIZE) / SAMPLE_RATE;

  mixI = random(1, 10) / 10.0f;
  mixQ = random(1, 10) / 10.0f;

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}