// MPO (memristor phase oscillator) groove engine //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define TABLE_SIZE  1024

float sineTable[TABLE_SIZE];

float phase1 = 0.0f, phaseInc1 = 0.0f, freq1 = 220.0f;
float phase2 = 0.0f, phaseInc2 = 0.0f, freq2 = 330.0f;
float phase3 = 0.0f, phaseInc3 = 0.0f, freq3 = 440.0f;

float env1 = 0.0f, env2 = 0.0f, env3 = 0.0f;
float decay1 = 0.0004f;
float decay2 = 0.00002f;
float decay3 = 0.0006f;

int time;
const int oneSecInUsec = 1000000;

float M1 = 0.5f, M2 = 0.5f, M3 = 0.5f;
float alpha = 0.01f;
float beta  = 0.001f;

void triggerEnvelopes(int trig) {

  switch (trig) {
    case 0: env1 = 1.0f; break;
    case 1: env2 = 1.0f; break;
    case 2: env3 = 1.0f; break;
    default: break;
  }

}

void sample() {

  phase1 += phaseInc1; if (phase1 >= TABLE_SIZE) phase1 -= TABLE_SIZE;
  phase2 += phaseInc2; if (phase2 >= TABLE_SIZE) phase2 -= TABLE_SIZE;
  phase3 += phaseInc3; if (phase3 >= TABLE_SIZE) phase3 -= TABLE_SIZE;

  float in1 = sineTable[(int)phase1 & (TABLE_SIZE - 1)];
  float in2 = sineTable[(int)phase2 & (TABLE_SIZE - 1)];
  float in3 = sineTable[(int)phase3 & (TABLE_SIZE - 1)];

  env1 -= decay1; if (env1 < 0.0f) env1 = 0.0f;
  env2 -= decay2; if (env2 < 0.0f) env2 = 0.0f;
  env3 -= decay3; if (env3 < 0.0f) env3 = 0.0f;

  in1 *= env1;
  in2 *= env2;
  in3 *= env3;

  M1 += alpha * in1 - beta * M1;
  M2 += alpha * in2 - beta * M2;
  M3 += alpha * in3 - beta * M3;

  if (M1 < 0.0f) M1 = 0.0f; if (M1 > 1.0f) M1 = 1.0f;
  if (M2 < 0.0f) M2 = 0.0f; if (M2 > 1.0f) M2 = 1.0f;
  if (M3 < 0.0f) M3 = 0.0f; if (M3 > 1.0f) M3 = 1.0f;

  float outVal = (M1*in1 + M2*in2 + M3*in3) / (M1+M2+M3 + 0.0001f);
  uint16_t out = (uint16_t)(32767.0f * (1.0f + outVal));

  analogWrite(AUDIO, out);

  CurieTimerOne.restart(time);

}

void setup() {

  pinMode(AUDIO, OUTPUT);
  analogWriteResolution(16);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);

  for (int i = 0; i < TABLE_SIZE; i++) sineTable[i] = sinf(2.0f * PI * (float)i / (float)TABLE_SIZE);

  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

}

void loop() {

  triggerEnvelopes(random(0, 3));

  freq1 = 110;
  phaseInc1 = (freq1 * TABLE_SIZE) / SAMPLE_RATE;

  freq2 = freq1 * random(1, 10);
  phaseInc2 = (freq2 * TABLE_SIZE) / SAMPLE_RATE;

  freq3 = freq1 * 80;
  phaseInc3 = (freq3 * TABLE_SIZE) / SAMPLE_RATE;

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}