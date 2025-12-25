// Simple additive LUT oscillator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define TABLE_SIZE  1024
#define HARMONICS   8

float harmonics[HARMONICS];
float sineTable[TABLE_SIZE];
float phase = 0.0f;
float phaseInc = 0.0f;
float freq = 440.0f;

int time;
const int oneSecInUsec = 1000000;

void sample() {

  int idx = (int)phase & (TABLE_SIZE - 1);
  float val = sineTable[idx];

  uint16_t out = 32767.0f * (1.0f + val);

  analogWrite(AUDIO, out);

  phase += phaseInc;
  if (phase >= TABLE_SIZE) phase -= TABLE_SIZE;

  CurieTimerOne.restart(time);

}

void setup() {

  pinMode(AUDIO, OUTPUT);

  analogWriteResolution(16);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);

  for (int i = 0; i < HARMONICS; i++) harmonics[i] = 1.0f / (i + 1);

  for (int i = 0; i < TABLE_SIZE; i++) {

    float phase = 2.0f * PI * i / TABLE_SIZE;
    float sum = 0.0f;
    for (int h = 0; h < HARMONICS; h++) sum += harmonics[h] * sinf((h + 1) * phase);
    sineTable[i] = sum;

  }

  for (int i = 0; i < TABLE_SIZE; i++) sineTable[i] /= HARMONICS;

  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

}

void loop() {

  freq = random(100, 1000);
  phaseInc = (freq * TABLE_SIZE) / SAMPLE_RATE;

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}