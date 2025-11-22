// Snare and Hi-Hat percussion generator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120

int attack_len = (int)(0.0015f * SAMPLE_RATE);
bool triggerFlag = false;
int sampleIndex = 0;
float env = 0.0f;
float envAlpha;

unsigned long rngState = 1234567;

float randNoise() {

  rngState = 1664525UL * rngState + 1013904223UL;
  return ((int32_t)(rngState >> 1)) / 1073741824.0f - 1.0f;

}

float prevX = 0.0f, prevY = 0.0f;
float hpfAlpha;

float b0,b1,b2,a1,a2;
float xn1=0.0f,xn2=0.0f,yn1=0.0f,yn2=0.0f;

void setupFilters(float fc, float f0, float Q, float gain_db) {

  float RC = 1.0f / (2.0f * PI * fc);
  hpfAlpha = RC / (RC + 1.0f/SAMPLE_RATE);

  float A = powf(10.0f, gain_db/40.0f);
  float w0 = 2.0f*PI*f0/SAMPLE_RATE;
  float alpha = sinf(w0)/(2.0f*Q);

  float bb0 = 1 + alpha*A;
  float bb1 = -2*cosf(w0);
  float bb2 = 1 - alpha*A;
  float aa0 = 1 + alpha/A;
  float aa1 = -2*cosf(w0);
  float aa2 = 1 - alpha/A;

  b0 = bb0/aa0;
  b1 = bb1/aa0;
  b2 = bb2/aa0;
  a1 = aa1/aa0;
  a2 = aa2/aa0;

}

void triggerEnvelope(float tau_ms) {
  
  envAlpha = expf(-1.0f / ((tau_ms/1000.0f) * SAMPLE_RATE ));
  triggerFlag = true;
  
}

const int oneSecInUsec = 1000000;
int time;

void sample() {

  if (triggerFlag) {

    triggerFlag = false;
    sampleIndex = 0;
    env = 1.0f;
    prevX = 0.0f;
    prevY = 0.0f;
    xn1 = xn2 = 0.0f;
    yn1 = yn2 = 0.0f;

  }

  if (sampleIndex < attack_len) env *= 1.02f;
  else env *= envAlpha;

  float noise = randNoise();
  float x = noise * env;

  float y_hpf = hpfAlpha * (prevY + x - prevX);
  prevX = x; prevY = y_hpf;
  float y_hat = b0*y_hpf + b1*xn1 + b2*xn2 - a1*yn1 - a2*yn2;
  xn2 = xn1; xn1 = y_hpf;
  yn2 = yn1; yn1 = y_hat;

  y_hat = tanhf(0.7f * y_hat) * 0.9f;
  sampleIndex++;

  analogWrite(AUDIO, (uint16_t)((y_hat + 1.0f) * 32767.0f));

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

  float freq = 50.0f + (randNoise() * 0.5f + 0.5f) * 8000.0f;
  setupFilters(freq, freq, 1.9f, 18.0f);

  float env = 1 + (randNoise() * 0.5f + 0.5f) * 15.0f;
  triggerEnvelope(env);

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}