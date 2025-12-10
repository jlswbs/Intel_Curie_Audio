// Simple 3 operator FM synth //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define TABLE_SIZE  1024

int time;
const int oneSecInUsec = 1000000;

float sineTable[TABLE_SIZE];

float phaseC = 0.0f, phaseIncC = 0.0f, freqC = 440.0f;
float envC = 1.0f, decayC = 0.9999f;

float phaseM1 = 0.0f, phaseIncM1 = 0.0f, freqM1 = 220.0f;
float fmIndex1 = 3.0f;
float envM1 = 1.0f, decayM1 = 0.997f;

float phaseM2 = 0.0f, phaseIncM2 = 0.0f, freqM2 = 110.0f;
float fmIndex2 = 1.0f;
float envM2 = 1.0f, decayM2 = 0.9999f;


void sample() {

  int idxM2 = (int)phaseM2 & (TABLE_SIZE - 1);
  float modVal2 = sineTable[idxM2] * envM2;
  phaseM2 += phaseIncM2;
  if (phaseM2 >= TABLE_SIZE) phaseM2 -= TABLE_SIZE;
  envM2 *= decayM2;

  int idxM1 = (int)phaseM1 & (TABLE_SIZE - 1);
  float modVal1 = sineTable[idxM1] * envM1;
  phaseM1 += phaseIncM1 + (fmIndex2 * phaseIncM2 * modVal2);
  if (phaseM1 >= TABLE_SIZE) phaseM1 -= TABLE_SIZE;
  envM1 *= decayM1;

  phaseC += phaseIncC + (fmIndex1 * phaseIncM1 * modVal1) + (fmIndex2 * phaseIncM2 * modVal2);
  if (phaseC >= TABLE_SIZE) phaseC -= TABLE_SIZE;

  int idxC = (int)phaseC & (TABLE_SIZE - 1);
  float val = sineTable[idxC] * envC;
  envC *= decayC;

  uint16_t out = (uint16_t)((1.0f + val) * 32767.0f);
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

  freqC  = random(220, 1600);
  phaseIncC = (freqC * TABLE_SIZE) / SAMPLE_RATE;
  envC = 1.0f;

  freqM1 = random(100, 880);
  phaseIncM1 = (freqM1 * TABLE_SIZE) / SAMPLE_RATE;
  envM1 = 1.0f;

  freqM2 = random(50, 440);
  phaseIncM2 = (freqM2 * TABLE_SIZE) / SAMPLE_RATE;
  envM2 = 1.0f;

  int tempo = 60000 / BPM;
  delay(tempo / 4);

}