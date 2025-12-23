// PWM sample mangler window //

#include "CurieTimerOne.h"
#include "wavetable.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define TABLE_SIZE  22050

float phase = 0.0f;
float phaseInc = 0.0f;

uint8_t pw = 128;
float windowSize = 0;
float startOffset = 0;

float freq = 0.1f;

int time;
const int oneSecInUsec = 1000000;

void sample() {

  int idx = (int)phase;
  if (idx >= TABLE_SIZE) idx %= TABLE_SIZE;

  float val = wavetable[idx];
  uint16_t out = 32767.0f * (1.0f + val);
  analogWrite(AUDIO, out);

  phase += phaseInc;

  if (phase >= startOffset + windowSize) {
    phase = startOffset;
  }

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

  pw = random(8, 255);
  freq = random(1, 20) / 10.0f;

  windowSize = ((float)pw / 255.0f) * (TABLE_SIZE - 1);
  if (windowSize < 8) windowSize = 8;

  startOffset = 0;
  phase = startOffset;

  phaseInc = (freq * TABLE_SIZE) / SAMPLE_RATE;

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}