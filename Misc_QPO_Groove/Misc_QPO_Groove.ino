// QPO (quantum phase oscillator) groove engine //

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

float p1 = 0.33f, p2 = 0.33f, p3 = 0.34f;
float phi1 = 0.0f, phi2 = 2.0f, phi3 = -1.0f;

float collapseProb = 0.01f;
int   collapseHoldSamples = SAMPLE_RATE / 100;
int   collapseCounter = 0;
int   collapsedVoice = -1;

static inline float softNorm(float x) {

  return x / (1.0f + fabsf(x));

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

  if (collapseCounter > 0) {
    collapseCounter--;
  } else {
    if ((float)random(0, 10000) / 10000.0f < collapseProb) {
      float r = (float)random(0, 10000) / 10000.0f;
      float c1 = p1;
      float c2 = p1 + p2;
      if (r < c1) collapsedVoice = 1;
      else if (r < c2) collapsedVoice = 2;
      else collapsedVoice = 3;
      collapseCounter = collapseHoldSamples;
    } else {
      collapsedVoice = -1;
    }
  }

  float w1, w2, w3;
  if (collapsedVoice == 1) {
    w1 = 1.0f; w2 = 0.0f; w3 = 0.0f;
  } else if (collapsedVoice == 2) {
    w1 = 0.0f; w2 = 1.0f; w3 = 0.0f;
  } else if (collapsedVoice == 3) {
    w1 = 0.0f; w2 = 1.0f; w3 = 0.0f;
  } else {
    w1 = softNorm(p1) * cosf(phi1);
    w2 = softNorm(p2) * cosf(phi2);
    w3 = softNorm(p3) * cosf(phi3);
  }

  float mix = (w1 * in1 + w2 * in2 + w3 * in3);
  float norm = fabsf(w1) + fabsf(w2) + fabsf(w3) + 1e-4f;
  float outVal = mix / norm;

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

  freq1 = 440;
  phaseInc1 = (freq1 * TABLE_SIZE) / SAMPLE_RATE;

  freq2 = (freq1/2) * random(1, 11);
  phaseInc2 = (freq2 * TABLE_SIZE) / SAMPLE_RATE;

  freq3 = freq1 * 20;
  phaseInc3 = (freq3 * TABLE_SIZE) / SAMPLE_RATE;

  p1 += (random(-5, 6) / 1000.0f);
  p2 += (random(-5, 6) / 1000.0f);
  p3 += (random(-5, 6) / 1000.0f);

  if (p1 < 0.02f) p1 = 0.02f; if (p2 < 0.02f) p2 = 0.02f; if (p3 < 0.02f) p3 = 0.02f;
  float ps = p1 + p2 + p3;
  p1 /= ps; p2 /= ps; p3 /= ps;

  phi1 += random(-5, 6) / 1000.0f;
  phi2 += random(-5, 6) / 700.0f;
  phi3 += random(-5, 6) / 500.0f;

  if (phi1 > PI) phi1 -= 2.0f * PI; if (phi1 < -PI) phi1 += 2.0f * PI;
  if (phi2 > PI) phi2 -= 2.0f * PI; if (phi2 < -PI) phi2 += 2.0f * PI;
  if (phi3 > PI) phi3 -= 2.0f * PI; if (phi3 < -PI) phi3 += 2.0f * PI;

  int tempo = 60000 / BPM;
  delay(tempo / 2);

}