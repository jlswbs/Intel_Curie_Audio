// NE555 (astable multivibrator) oscillator + filter //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120

  float R1_min = 1000.0f;
  float R1_max = 10000.0f;
  float R2_min = 1000.0f;
  float R2_max = 10000.0f;
  float F_min = 200.0f;
  float F_max = 5000.0f;

  float Vc = 0.0f;
  int out_state = 1;
  float Vcc = 5.0f;
  float VTH = (2.0f / 3.0f) * Vcc;
  float VTL = (1.0f / 3.0f) * Vcc;
  float R1 = (R1_min + R1_max) / 2.0f;
  float R2 = (R2_min + R2_max) / 2.0f;
  float C = 1e-6f;

  float dt = 1.0f / SAMPLE_RATE;

  float fc = (F_min + F_max) / 2.0f;
  float alpha = dt / (1.0f / (2.0f * PI * fc) + dt);
  float prev_out = 0.0f;
  float output;

  int time;
  const int oneSecInUsec = 1000000;

void sample() {

  float dVc;

  if (out_state) {
    dVc = (Vcc - Vc) / ((R1 + R2) * C) * dt;
    Vc += dVc;
    if (Vc >= VTH) {
      out_state = 0;
    }
  } else {
    dVc = (0.0f - Vc) / (R2 * C) * dt;
    Vc += dVc;
    if (Vc <= VTL) {
      out_state = 1;
    }
  }

  float input = out_state ? 1.0f : -1.0f;

  output = alpha * input + (1.0f - alpha) * prev_out;
  prev_out = output;

  analogWrite(AUDIO, (uint16_t)((output + 1.0f) * 32767.0f));

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