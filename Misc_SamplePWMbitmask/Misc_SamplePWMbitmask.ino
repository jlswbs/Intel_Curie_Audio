// PWM sample mangler bitmask //

#include "CurieTimerOne.h"
#include "wavetable.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define TABLE_SIZE  22050

float phase = 0.0f;
float phaseInc = 0.0f;

uint8_t pw = 128;
uint32_t indexMask = 0xFFFF;

float freq = 0.1f;

int time;
const int oneSecInUsec = 1000000;

void sample() {

  uint32_t rawIdx = (uint32_t)phase;
  uint32_t idx = rawIdx & indexMask;
  if (idx >= TABLE_SIZE) idx %= TABLE_SIZE;

  float val = wavetable[idx];
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

  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

}

void loop() {

  pw = random(1, 256);
  freq = random(1, 10) / 10.0f;

  uint8_t bits = map(pw, 0, 255, 1, 15);
  indexMask = (1UL << bits) - 1;

  phaseInc = (freq * TABLE_SIZE) / SAMPLE_RATE;

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}