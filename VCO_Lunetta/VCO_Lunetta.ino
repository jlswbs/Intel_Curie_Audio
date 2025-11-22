// Lunetta (schmitt trigger) oscillator + filter //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120

  float R_min = 5000.0f;
  float R_max = 20000.0f;
  float F_min = 200.0f;
  float F_max = 5000.0f;

  float Vc = 0.0f;
  int out_state = 1;
  float Vcc = 5.0f;
  float VTH = 3.3f;
  float VTL = 1.7f;
  float R = (R_min + R_max) / 2.0f;
  float C = 1e-6f;
  float tau = R * C;
  float exp_factor = expf(-1.0f / (tau * 44100.0f));

  float fc = (F_min + F_max) / 2.0f;
  float alpha = (1.0f / SAMPLE_RATE) / (1.0f/(2.0f * PI * fc) + 1.0f/SAMPLE_RATE);

  float prev_out = 0.0f;
  float output;

  int time;
  const int oneSecInUsec = 1000000;

void sample() {

  if (out_state && Vc >= VTH) out_state = 0;
  else if (!out_state && Vc <= VTL) out_state = 1;

  float target = out_state ? Vcc : 0.0f;
  Vc = target + (Vc - target) * exp_factor;

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

  R = random(R_min, R_max);
  tau = R * C;
  exp_factor = expf(-1.0f / (tau * SAMPLE_RATE));

  fc = random(F_min, F_max);
  alpha = (1.0f / SAMPLE_RATE) / (1.0f/(2.0f * PI * fc) + 1.0f/SAMPLE_RATE);

  int tempo = 60000 / BPM;
  delay(tempo / 3);
  
}