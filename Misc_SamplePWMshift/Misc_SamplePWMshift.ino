// PWM sample mangler shifter //

#include "CurieTimerOne.h"
#include "wavetable.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define TABLE_SIZE  22050

float phase = 0.0f;
float pwmPhaseInc = 0.0f;
uint8_t pw = 0;
float startOffset = 0;
float pwmFreq = 1.0f;

int time;
const int oneSecInUsec = 1000000;

void sample() {

  int idx = (int)phase % TABLE_SIZE;
  float val = wavetable[idx];

  uint16_t out = 32767.0f * (1.0f + val);
  analogWrite(AUDIO, out);

  phase += pwmPhaseInc;
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

  pw = random(8, 128);

  startOffset = ((float)pw / 255.0f) * (TABLE_SIZE - 1);
  phase = startOffset;
  pwmPhaseInc = (pwmFreq * TABLE_SIZE) / SAMPLE_RATE;

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}