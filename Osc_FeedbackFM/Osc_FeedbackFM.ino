// Simple feedback FM LUT oscillator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define TABLE_SIZE  1024

float sineTable[TABLE_SIZE];

float lastVal = 0.0f;
float phase = 0.0f;
float phaseInc = 0.0f;
float freq = 440.0f;
float fmIndex = 0.95f;

int time;
const int oneSecInUsec = 1000000;

void sample() {

  phase += phaseInc + (fmIndex * lastVal * phaseInc);
  if (phase >= TABLE_SIZE) phase -= TABLE_SIZE;

  int idx = (int)phase & (TABLE_SIZE - 1);
  float val = sineTable[idx];
  lastVal = val;

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

  freq = random(100, 880);
  phaseInc = (freq * TABLE_SIZE) / SAMPLE_RATE;

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}