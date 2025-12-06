// FPO (fuzzy logic phase oscillator) groove engine //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         90
#define TABLE_SIZE  1024

float sineTable[TABLE_SIZE];

float phase1 = 0.0f, phaseInc1 = 0.0f, freq1 = 220.0f;
float phase2 = 0.0f, phaseInc2 = 0.0f, freq2 = 330.0f;
float phase3 = 0.0f, phaseInc3 = 0.0f, freq3 = 440.0f;

float env1 = 0.0f, env2 = 0.0f, env3 = 0.0f;
float decay1 = 0.00004f;
float decay2 = 0.00001f;
float decay3 = 0.00006f;

int time;
const int oneSecInUsec = 1000000;

float sigmaScale1 = 2.0f, sigmaOffset1 = 0.0f;
float sigmaScale2 = 3.0f, sigmaOffset2 = 0.2f;
float sigmaScale3 = 1.5f, sigmaOffset3 = -0.1f;

static inline float fastSigma(float x, float scale, float offset) {

  return 1.0f / (1.0f + expf(-(x * scale + offset)));

}

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

  float f1 = fastSigma(in1, sigmaScale1, sigmaOffset1);
  float f2 = fastSigma(in2, sigmaScale2, sigmaOffset2);
  float f3 = fastSigma(in3, sigmaScale3, sigmaOffset3);

  float outVal = (f1 + f2 + f3) / 3.0f;
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

  sigmaScale1 = random(50, 200) / 100.0f;
  sigmaOffset1 = random(-100, 100) / 100.0f;

  sigmaScale2 = random(50, 200) / 100.0f;
  sigmaOffset2 = random(-100, 100) / 100.0f;

  sigmaScale3 = random(50, 200) / 100.0f;
  sigmaOffset3 = random(-100, 100) / 100.0f;

  freq1 = 110;
  phaseInc1 = (freq1 * TABLE_SIZE) / SAMPLE_RATE;

  freq2 = freq1 * random(1, 11);
  phaseInc2 = (freq2 * TABLE_SIZE) / SAMPLE_RATE;

  freq3 = freq1 * 80;
  phaseInc3 = (freq3 * TABLE_SIZE) / SAMPLE_RATE;

  int tempo = 60000 / BPM;
  delay(tempo / 4);

}