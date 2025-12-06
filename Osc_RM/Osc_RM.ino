// Simple ring modulator LUT oscillator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define TABLE_SIZE  1024

float sineTable[TABLE_SIZE];
float phaseA = 0.0f;
float phaseIncA = 0.0f;
float freqA = 440.0f;

float phaseB = 0.0f;
float phaseIncB = 0.0f;
float freqB = 220.0f;

int time;
const int oneSecInUsec = 1000000;

void sample() {

  phaseA += phaseIncA;
  if (phaseA >= TABLE_SIZE) phaseA -= TABLE_SIZE;
  float oscA = sineTable[(int)phaseA & (TABLE_SIZE - 1)];

  phaseB += phaseIncB;
  if (phaseB >= TABLE_SIZE) phaseB -= TABLE_SIZE;
  float oscB = sineTable[(int)phaseB & (TABLE_SIZE - 1)];

  float val = oscA * oscB;

  uint16_t out = (uint16_t)(32767.0f * (1.0f + val));
  analogWrite(AUDIO, out);

  CurieTimerOne.restart(time);

}

void setup() {

  pinMode(AUDIO, OUTPUT);

  analogWriteResolution(16);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);

  for (int i = 0; i < TABLE_SIZE; i++) sineTable[i] = sinf(2.0f * PI * i / TABLE_SIZE);

  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

}

void loop() {

  freqA = random(50, 880);
  phaseIncA = (freqA * TABLE_SIZE) / SAMPLE_RATE;

  freqB = random(50, 880);
  phaseIncB = (freqB * TABLE_SIZE) / SAMPLE_RATE;

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}