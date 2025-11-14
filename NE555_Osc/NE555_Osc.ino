// NE555 (astable multivibrator) oscillator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 22050
#define BPM         120

  float Vcc = 5.0f;

  float R1_min = 1000.0f;
  float R1_max = 10000.0f;
  float R2_min = 1000.0f;
  float R2_max = 10000.0f;

  float C = 1e-6f;

  const float dt = 1.0f / SAMPLE_RATE;

  float F_min = 200.0f;
  float F_max = 5000.0f;

  float alpha = 0.0f;
  float out_val = 0.0f;

  float Vc = 0.0f;
  int out_state = 1;

  float R1, R2, fc;
  float VTH = (2.0f / 3.0f) * Vcc;
  float VTL = (1.0f / 3.0f) * Vcc;

  int time;
  const int oneSecInUsec = 1000000;

void sample() {

  if (out_state) {

    float dVc = ((Vcc - Vc) / ((R1 + R2) * C)) * dt;
    Vc += dVc;

    if (Vc >= VTH) out_state = 0;

  } else {

    float dVc = ((0.0f - Vc) / (R2 * C)) * dt;
    Vc += dVc;

    if (Vc <= VTL) out_state = 1;
  
  }

  float out_sample = out_state ? 1.0f : -1.0f;
  out_val = alpha * out_sample + (1.0f - alpha) * out_val;

  analogWrite(AUDIO, (uint16_t)((out_val + 1.0f) * 32767.0f));

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

  R1 = random(R1_min, R1_max);
  R2 = random(R2_min, R2_max);
  fc = random(F_min, F_max);
  alpha = dt / (1.0f / (2.0f * PI * fc) + dt);

  int tempo = 60000 / BPM;
  delay(tempo / 3);
  
}