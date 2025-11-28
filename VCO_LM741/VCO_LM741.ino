// LM741 (opamp RC relaxation) oscillator + filter //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120

  float R_min = 1000.0f;
  float R_max = 10000.0f;
  float F_min = 200.0f;
  float F_max = 5000.0f;

  float Vcc = 5.0f;
  float Vhigh = 4.0f;
  float Vlow = 1.0f;
  float GBW = 1e6f;
  float AO = 200000.0f;
  float SR = 0.5e6f;
  float VTH = 3.1f;
  float VTL = 1.9f;
  float R = (R_min + R_max) / 2.0f;
  float C = 1e-6f;

  float fp = GBW / AO;
  float tau = 1.0f / (2.0f * PI * fp);

  float dt = 1.0f / SAMPLE_RATE;
  float max_step = SR * dt;

  float Vc = 0.0f;
  float Vout_int = Vlow;
  int out_state = 1;

  float fc = (F_min + F_max) / 2.0f;
  float alpha = dt / (1.0f / (2.0f * PI * fc) + dt);
  float output = 0.0f;
  float prev_out = 0.0f;

  inline float soft_clip(float x, float limit) { return limit * tanhf(x / limit); }

  int time;
  const int oneSecInUsec = 1000000;

void sample() {

  float dVc = out_state ? (Vcc - Vc) / (R * C) : (0.0f - Vc) / (R * C);
  Vc += dVc * dt;

  if (out_state && Vc >= VTH) out_state = 0;
  else if (!out_state && Vc <= VTL) out_state = 1;

  float target = out_state ? 1.0f : -1.0f;
  float diff = target * A0;
  float dV = (diff - Vout_int) * (dt / tau);
  Vout_int += dV;

  Vout_int = (Vout_int < prev_out - max_step) ? prev_out - max_step : (Vout_int > prev_out + max_step) ? prev_out + max_step : Vout_int;

  float Vout_clipped = soft_clip(Vout_int, 1.0f);
  float Vout_real = (Vout_clipped + 1.0f) * 0.5f * (Vhigh - Vlow) + Vlow;
  float input = (Vout_real - Vlow) / (Vhigh - Vlow) * 2.0f - 1.0f;

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
  fc = random(F_min, F_max);
  alpha = dt / (1.0f / (2.0f * PI * fc) + dt);

  int tempo = 60000 / BPM;
  delay(tempo / 3);
  
}