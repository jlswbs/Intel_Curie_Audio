// Spice Hi-Hat (opamp base) generator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
  
  float dt = 1.0f / SAMPLE_RATE;

  float R_noise = 10e2f;
  float C_min   = 2e-7f;
  float C_max   = 8e-9f;
  float C_noise = (C_min + C_max) / 2.0f;
  float tau_noise = R_noise * C_noise;

  float R_hpf = 3.3e3f;
  float C_hpf = 7.5e-9f;
  float tau_hpf = R_hpf * C_hpf;

  float R_env = 10e3f;
  float C_env = 1e-6f;
  float tau_env = R_env * C_env;
  float Venv = 0.0f;
  int charge_time = (int)(0.001f * SAMPLE_RATE);
  int charge_remaining = 0;

  float A_ol_dc = 5e4f;
  float f_pole = 8000.0f;
  float omega_p = 2.0f * PI * f_pole;
  float slew_rate = 2e6f;
  float Vrail_pos = 15.0f, Vrail_neg = -15.0f;
  float Vout_headroom = 1.5f;
  float Vout_max = Vrail_pos - Vout_headroom;
  float Vout_min = Vrail_neg + Vout_headroom;
  float soft_sat_alpha = 0.5f;

  float Vc_noise = 0.0f;
  float x_prev_hpf = 0.0f;
  float y_prev_hpf = 0.0f;
  float v_ol_state = 0.0f;
  float Vout = 0.0f;

float soft_saturation(float v, float vmax, float vmin, float alpha) {
  float center = 0.5f * (vmax + vmin);
  float span   = 0.5f * (vmax - vmin);
  return center + span * tanhf(alpha * (v - center) / span);
}

float limit_slew(float v_target, float v_prev, float dt, float SR) {
  float dv = v_target - v_prev;
  float dv_max = SR * dt;
  if (dv > dv_max) dv = dv_max;
  if (dv < -dv_max) dv = -dv_max;
  return v_prev + dv;
}

float randNormal() {
  float u1 = (float)random(1, 32767) / 32767.0f;
  float u2 = (float)random(1, 32767) / 32767.0f;
  return sqrtf(-2.0f * logf(u1)) * cosf(2.0f * PI * u2);
}

  int time;
  const int oneSecInUsec = 1000000;

void sample() {

  float inoise = randNormal() * 1.0e-5f;
  Vc_noise += (inoise * R_noise - Vc_noise) / tau_noise * dt;

  float RC = tau_hpf;
  float alpha = RC / (RC + 1.0f / SAMPLE_RATE);
  float x_hpf = Vc_noise;
  float y_hpf = alpha * (y_prev_hpf + x_hpf - x_prev_hpf);
  x_prev_hpf = x_hpf;
  y_prev_hpf = y_hpf;

  float dv_ol = omega_p * (A_ol_dc * y_hpf - v_ol_state) * dt;
  v_ol_state += dv_ol;
  float Vsat = soft_saturation(v_ol_state, Vout_max, Vout_min, soft_sat_alpha);
  Vout = limit_slew(Vsat, Vout, dt, slew_rate);

  float Icharge;
  if (charge_remaining > 0) {
    Icharge = 1e-5f;
    charge_remaining--;
  } else {
    Icharge = 0.0f;
  }
  float dV = (-Venv / tau_env + Icharge / C_env) * dt;
  Venv += dV;

  float output = tanhf(0.9f * Venv * Vout);

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

  float r = (float)random(5000, 9700) / 10000.0f;
  C_noise = C_min + r * (C_max - C_min);
  tau_noise = R_noise * C_noise;
  charge_remaining = charge_time; 

  int tempo = 60000 / BPM;
  delay(tempo / 3);
  
}