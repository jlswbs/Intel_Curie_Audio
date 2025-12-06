// Simple phase modulation LUT oscillator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define TABLE_SIZE  1024

float sineTable[TABLE_SIZE];
float phaseC = 0.0f;
float phaseIncC = 0.0f;
float freqC = 440.0f;
float phaseM = 0.0f;
float phaseIncM = 0.0f;
float freqM = 220.0f;
float pmIndex = 5.0f;

int time;
const int oneSecInUsec = 1000000;

void sample() {

  int idxM = (int)phaseM & (TABLE_SIZE - 1);
  float modVal = sineTable[idxM];
  phaseM += phaseIncM;
  if (phaseM >= TABLE_SIZE) phaseM -= TABLE_SIZE;

  phaseC += phaseIncC;
  if (phaseC >= TABLE_SIZE) phaseC -= TABLE_SIZE;

  float phaseWithMod = phaseC + pmIndex * modVal * (TABLE_SIZE / (2.0f * PI));
  int idxC = ((int)phaseWithMod) & (TABLE_SIZE - 1);

  float val = sineTable[idxC];

  uint16_t out = 32767.0f * (1.0f + val);
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

  freqC = random(200, 2000);
  phaseIncC = (freqC * TABLE_SIZE) / SAMPLE_RATE;

  freqM = random(50, 500);
  phaseIncM = (freqM * TABLE_SIZE) / SAMPLE_RATE;

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}