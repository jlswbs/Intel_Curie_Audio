// LC resonant FM oscillator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120

float Lc = 0.01f;
float Cc = 0.00001f;
float Rc = 0.0f;
float Vc_c = 0.1f;
float Il_c = 0.0f;

float Lm = 0.001f;
float Cm = 0.00001f;
float Rm = 0.0f;
float Vc_m = 0.1f;
float Il_m = 0.0f;

float fmIndex = 0.9f;

float dt = 1.0f / SAMPLE_RATE;

int time;
const int oneSecInUsec = 1000000;

float randomf(float minf, float maxf) { return minf + random(1UL << 31) * (maxf - minf) / (1UL << 31); }

inline float LCFMoscillator() {

  Il_m += ((Vc_m - Rm * Il_m) / Lm) * dt;
  Vc_m += (-Il_m / Cm) * dt;

  float fm = Vc_m;
  float VcFM = Vc_c + fmIndex * fm;
  VcFM += tanhf(VcFM);

  Il_c += ((VcFM - Rc * Il_c) / Lc) * dt;
  Vc_c += (-Il_c / Cc) * dt;

  return tanhf(2.0f * Vc_c);

}

void sample() {

  float val = LCFMoscillator();

  uint16_t out = 32767.0f * (1.0f + val);
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

  float freqMul = randomf(0.5f, 2.0f);
  dt = freqMul / SAMPLE_RATE;

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}