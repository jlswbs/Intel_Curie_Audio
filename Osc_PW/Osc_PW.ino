// Simple pulse width modulation oscillator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define TABLE_SIZE  1024

float phase = 0.0f;
float phaseInc = 0.0f;
float freq = 440.0f;
float duty = 0.5f;

int time;
const int oneSecInUsec = 1000000;

void sample() {

  float normPhase = phase / TABLE_SIZE;
  float val = (normPhase < duty) ? 1.0f : -1.0f;

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

  freq = random(50, 440);
  duty = random(100, 900) / 1000.0f;
  phaseInc = (freq * TABLE_SIZE) / SAMPLE_RATE;

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}