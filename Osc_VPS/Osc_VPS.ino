// Simple vector phase LUT oscillator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define TABLE_SIZE  1024

float sineTable[TABLE_SIZE];

float phase1 = 0.0f, phaseInc1 = 0.0f, freq1 = 220.0f;
float phase2 = 0.0f, phaseInc2 = 0.0f, freq2 = 330.0f;
float w1 = 0.5f;
float w2 = 0.5f;

int time;
const int oneSecInUsec = 1000000;

void sample() {

  phase1 += phaseInc1;
  if (phase1 >= TABLE_SIZE) phase1 -= TABLE_SIZE;

  phase2 += phaseInc2;
  if (phase2 >= TABLE_SIZE) phase2 -= TABLE_SIZE;

  float combinedPhase = (w1 * phase1 + w2 * phase2);

  int idx = (int)combinedPhase & (TABLE_SIZE - 1);
  float val = sineTable[idx];

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

  freq1 = random(100, 400);
  phaseInc1 = (freq1 * TABLE_SIZE) / SAMPLE_RATE;

  freq2 = random(200, 600);
  phaseInc2 = (freq2 * TABLE_SIZE) / SAMPLE_RATE;

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}