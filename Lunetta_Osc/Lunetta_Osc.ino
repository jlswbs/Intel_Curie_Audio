// Lunetta (schmitt trigger) oscillator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 22050
#define BPM         120

  float Vcc = 5.0f;
  float VTH = 3.3f;
  float VTL = 1.7f;
  float R_min = 5000.0f;
  float R_max = 20000.0f;
  float C = 1.0e-6;

  float dt = 1.0f / SAMPLE_RATE;

  float F_min = 200.0f;
  float F_max = 5000.0f;

  float Vc = 0.0f;
  bool out_state = true;
  float out_val = 0.0f;
  float alpha = 0.0f;
  float R = R_min;
  float fc = F_min;

  int time;
  const int oneSecInUsec = 1000000;

void sample() {

  float dVc;

  if (out_state) {
    dVc = (Vcc - Vc) / (R * C) * dt;
  } else {
    dVc = (0.0f - Vc) / (R * C) * dt;
  }

  Vc += dVc;

  if (out_state && Vc >= VTH) {
    out_state = false;
  } else if (!out_state && Vc <= VTL) {
    out_state = true;
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

  R = random(R_min, R_max);
  fc = random(F_min, F_max);
  alpha = dt / (1.0f / (2.0f * PI * fc) + dt);

  int tempo = 60000 / BPM;
  delay(tempo / 3);
  
}