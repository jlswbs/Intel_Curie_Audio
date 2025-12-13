// Sine wave varicap shaper //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define TABLE_SIZE  1024

int time;
const int oneSecInUsec = 1000000;

float sineTable[TABLE_SIZE];

float phase = 0.0f;
float phaseInc = 0.0f;
float freq = 440.0f;

static inline float varicapGainNorm(float bias) {

  float Vj = 1.0f;
  float n  = 0.7f;
  float x  = fabsf(bias);
  float g  = 1.0f / powf(1.0f + x / Vj, n);
  return g;

}

void sample() {

  int idx = (int)phase & (TABLE_SIZE - 1);
  float val = sineTable[idx];
  float shaped = varicapGainNorm(val);

  uint16_t out = 65535.0f * shaped;
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

  freq = random(100, 1000);
  phaseInc = (freq * TABLE_SIZE) / SAMPLE_RATE;

  int tempo = 60000 / BPM;
  delay(tempo / 4);

}