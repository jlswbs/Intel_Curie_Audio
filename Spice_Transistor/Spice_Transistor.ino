// Spice transistor RC phase-shift oscillator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120

float R = 10000.0f;
float C = 0.000000047f;
float Vcc = 5.0f;
float Vt = 0.026f;
float Is = 1e-14f;
float Rb = 100000.0f;
float Gain = 3.0f;
float Bias = (Vcc * 0.7f) / (Rb / 10000.0f + 1.0f);

float vc1 = 0.1f; 
float vc2 = 0.0f;
float vc3 = 0.0f;

float dt = 1.0f / SAMPLE_RATE;

int time;
const int oneSecInUsec = 1000000;

float randomf(float minf, float maxf) { return minf + random(1UL << 31)*(maxf - minf) / (1UL << 31); }

void sample() {

  float v_be = Bias + (vc3 * Gain); 
  float ic = Is * (expf(v_be / Vt) - 1.0f);
  float v_coll = Vcc - (ic * R);
  
  if (v_coll < 0.2f) v_coll = 0.2f;
  if (v_coll > Vcc)  v_coll = Vcc;

  float feedback = v_coll - (Vcc / 2.0f);

  float dvc1 = (feedback - vc1) / (R * C);
  float dvc2 = (vc1 - vc2) / (R * C);
  float dvc3 = (vc2 - vc3) / (R * C);

  vc1 += dvc1 * dt;
  vc2 += dvc2 * dt;
  vc3 += dvc3 * dt;

  uint16_t out = (uint16_t)(32767.0f * (1.0f + ((v_coll / Vcc) * 0.4)));
  analogWrite(AUDIO, out);

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

  R = randomf(5000, 15000); 
  
  int tempo = 60000 / BPM;
  delay(tempo / 3);

}