// Simple DDS sine LUT oscillator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define TABLE_SIZE  1024
#define LUT_BITS    10

float sineTable[TABLE_SIZE];
uint32_t phaseAcc = 0;
uint32_t phaseInc = 0;

float freq = 440.0f;

int time;
const int oneSecInUsec = 1000000;

void sample() {

  uint32_t idx = phaseAcc >> (32 - LUT_BITS);
  uint32_t nextIdx = (idx + 1) & (TABLE_SIZE - 1);
  uint32_t fracBits = 32 - LUT_BITS;
  uint32_t fracMask = (fracBits == 32) ? 0xFFFFFFFFu : ((1u << fracBits) - 1u);
  uint32_t fracRaw = phaseAcc & fracMask;
  float frac = (float)fracRaw / (float)(1u << fracBits);

  float a = sineTable[idx];
  float b = sineTable[nextIdx];
  float val = a + (b - a) * frac;

  uint16_t out = 32767.0f * (1.0f + val);
  analogWrite(AUDIO, out);

  phaseAcc += phaseInc;

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
  phaseInc = (uint32_t)((freq * (double)UINT32_MAX) / (double)SAMPLE_RATE);

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}