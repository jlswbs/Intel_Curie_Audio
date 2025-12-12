// Sample hihat varicap shaper //

#include "CurieTimerOne.h"
#include "wavetable.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define TABLE_SIZE  8192

float phase = 0.0f;
float phaseInc = 0.0f;
float freq = 440.0f;

int time;
const int oneSecInUsec = 1000000;

static inline float varicapGainNorm(float bias) {

  float Vj = 1.0f;
  float n  = 0.7f;
  float x  = fabsf(bias);
  float g  = 1.0f / powf(1.0f + x / Vj, n);
  return g;

}


void sample() {

  int idx = (int)phase;
  if (idx >= TABLE_SIZE) { return; }

  float val = wavetable[idx];
  float shaped = varicapGainNorm(val);
  uint16_t out = 65535.0f * shaped;
  analogWrite(AUDIO, out);

  phase += phaseInc;

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

  phase = 0.0f;

  freq = random(10, 50);
  phaseInc = (freq * TABLE_SIZE) / SAMPLE_RATE;

  int tempo = 60000 / BPM;
  delay(tempo / 2);

}