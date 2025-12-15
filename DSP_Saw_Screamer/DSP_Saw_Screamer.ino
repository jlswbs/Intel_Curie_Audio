// Saw wave tube screamer shaper //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define TABLE_SIZE  1024

int time;
const int oneSecInUsec = 1000000;

float sawTable[TABLE_SIZE];

float phase = 0.0f;
float phaseInc = 0.0f;
float freq = 440.0f;

float prevOut = 0.0f;
float prevIn = 0.0f;

float tubeScreamer(float in) {

    float hp = in - 0.95f * prevIn;
    prevIn = in;
    float mid = hp * 0.8f;

    float pos = tanhf(2.0f * mid);
    float neg = tanhf(3.0f * mid);
    float out = (mid >= 0.0f) ? pos : neg;

    out = 0.7f * out + 0.3f * prevOut;
    prevOut = out;

    return out;

}

void sample() {

  int idx = (int)phase & (TABLE_SIZE - 1);
  float val = sawTable[idx];
  float shaped = tubeScreamer(val);

  uint16_t out = (uint16_t)((1.0f + shaped) * 32767.0f);
  analogWrite(AUDIO, out);

  phase += phaseInc;
  if (phase >= TABLE_SIZE) phase -= TABLE_SIZE;

  CurieTimerOne.restart(time);

}

void setup() {

  pinMode(AUDIO, OUTPUT);

  analogWriteResolution(16);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);

  for (int i = 0; i < TABLE_SIZE; i++) sawTable[i] = (2.0f * i / TABLE_SIZE) - 1.0f;

  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

}

void loop() {

  freq = random(50, 440);
  phaseInc = (freq * TABLE_SIZE) / SAMPLE_RATE;

  int tempo = 60000 / BPM;
  delay(tempo / 4);

}