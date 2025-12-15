// Sine wave chili overdrive shaper //

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

float prevOut = 0.0f;
float drive = 5.0f;

float chiliOverdrive(float in, float drive) {

  float boosted = in * drive;
  float fuzzed = (boosted >= 0.0f) ? 1.0f : -1.0f;
  float out = 0.7f * fuzzed + 0.3f * boosted;

  out = 0.5f * out + 0.5f * prevOut;
  prevOut = out;

  return out;

}

void sample() {

  int idx = (int)phase & (TABLE_SIZE - 1);
  float val = sineTable[idx];
  float shaped = chiliOverdrive(val, drive);

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

  for (int i = 0; i < TABLE_SIZE; i++) sineTable[i] = sinf(2.0f * PI * i / TABLE_SIZE);

  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

}

void loop() {

  freq = random(50, 880);
  phaseInc = (freq * TABLE_SIZE) / SAMPLE_RATE;

  int tempo = 60000 / BPM;
  delay(tempo / 4);

}