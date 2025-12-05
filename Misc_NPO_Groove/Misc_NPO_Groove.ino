// NPO (neural phase oscillator) groove engine //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         90
#define TABLE_SIZE  1024

float sineTable[TABLE_SIZE];

float phase1 = 0.0f, phaseInc1 = 0.0f, freq1 = 220.0f;
float phase2 = 0.0f, phaseInc2 = 0.0f, freq2 = 330.0f;
float phase3 = 0.0f, phaseInc3 = 0.0f, freq3 = 440.0f;

float w11 = 0.5f, w12 = 0.3f, w13 = 0.2f, b1 = 0.0f;
float w21 = 0.2f, w22 = 0.5f, w23 = 0.3f, b2 = 0.0f;

float fb1 = 0.0f;
float fb2 = 0.0f;

float v1   = 0.6f;
float v2   = 0.8f;
float bOut = 0.0f;
float outFeedback = 0.5f;

float env1 = 0.0f, env2 = 0.0f, env3 = 0.0f;
float decay1 = 0.0004f;
float decay2 = 0.0001f;
float decay3 = 0.0006f;

float h1_state = 0.0f;
float h2_state = 0.0f;
float out_state = 0.0f;

int time;
const int oneSecInUsec = 1000000;

static inline float fastTanh(float x) {

  if (x < -3.0f) return -1.0f;
  if (x >  3.0f) return  1.0f;
  float x2 = x * x;
  return x * (27.0f + x2) / (27.0f + 9.0f * x2);

}

void triggerEnvelopes(int trig) {

  switch (trig) {
    case 0:
      env1 = 1.0f;
      break;
    case 1:
      env2 = 1.0f;
      break;
    case 2:
      env3 = 1.0f;
      break;
    default:
      break;
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

  float h1 = fastTanh(w11*in1 + w12*in2 + w13*in3 + fb1*h1_state + b1);
  float h2 = fastTanh(w21*in1 + w22*in2 + w23*in3 + fb2*h2_state + b2);

  h1_state = h1;
  h2_state = h2;

  float outVal = fastTanh(v1*h1 + v2*h2 + outFeedback*out_state + bOut);
  out_state = outVal;

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

  w11 = (random(-1000, 1000)) / 1000.0f;
  w12 = (random(-1000, 1000)) / 1000.0f;
  w13 = (random(-1000, 1000)) / 1000.0f;

  w21 = (random(-1000, 1000)) / 1000.0f;
  w22 = (random(-1000, 1000)) / 1000.0f;
  w23 = (random(-1000, 1000)) / 1000.0f;

  fb1 = random(-1000, 1000) / 1000.0f;
  fb2 = random(-1000, 1000) / 1000.0f;
  b1 = random(-500, 500) / 1000.0f;
  b2 = random(-500, 500) / 1000.0f;

  freq1 = 110;
  phaseInc1 = (freq1 * TABLE_SIZE) / SAMPLE_RATE;

  freq2 = freq1 * random(1, 11);
  phaseInc2 = (freq2 * TABLE_SIZE) / SAMPLE_RATE;

  freq3 = freq1 * 100;
  phaseInc3 = (freq3 * TABLE_SIZE) / SAMPLE_RATE;

  int tempo = 60000 / BPM;
  delay(tempo / 4);

}