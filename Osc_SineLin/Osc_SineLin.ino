// Simple sine LUT oscillator - linear interpolation //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define TABLE_SIZE  1024

float sineTable[TABLE_SIZE];
float phase = 0.0f;
float phaseInc = 0.0f;
float freq = 440.0f;

int time;
const int oneSecInUsec = 1000000;

void sample() {

  int i = (int)phase;
  float frac = phase - i;
  float v1 = sineTable[i];
  float v2 = sineTable[(i + 1) & (TABLE_SIZE - 1)];
  float val = v1 + (v2 - v1) * frac;

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
  
  for (int i = 0; i < TABLE_SIZE; i++) sineTable[i] = sinf(2.0f * PI * i / TABLE_SIZE);

  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

}

void loop() {

  freq = random(110, 880);
  phaseInc = (freq * TABLE_SIZE) / SAMPLE_RATE;

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}