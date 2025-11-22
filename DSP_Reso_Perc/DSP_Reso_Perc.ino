// Resonant filter percussion generator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120

float burst_ms = 1.5;
int   burst_len = int(SAMPLE_RATE * (burst_ms / 1000.0f));
float Q = 80.0f;

float b0, b1, b2, a1, a2;
float xn1 = 0.0f, xn2 = 0.0f;
float yn1 = 0.0f, yn2 = 0.0f;

int burstCounter = 0;
bool burstActive = false;

unsigned long rngState = 1234567;

float randNoise() {

  rngState = 1664525UL * rngState + 1013904223UL;
  return ((int32_t)(rngState >> 1)) / 1073741824.0f - 1.0f;

}

void trigger(float freq) {

  float omega0 = 2.0f * PI * freq / SAMPLE_RATE;
  float alpha  = sin(omega0) / (2.0f * Q);

  float a0 = 1.0f + alpha;
  b0 =  alpha / a0;
  b1 =  0.0f   / a0;
  b2 = -alpha / a0;
  a1 = -2.0f * cos(omega0) / a0;
  a2 = (1.0f - alpha) / a0;


  burstCounter = 0;
  burstActive  = true;

}

float processSample() {

  float in = 0.0f;

  if (burstActive) {

    in = randNoise();
    burstCounter++;
    if (burstCounter >= burst_len) burstActive = false;
  
  }

  float y = b0*in + b1*xn1 + b2*xn2 - a1*yn1 - a2*yn2;

  xn2 = xn1; xn1 = in;
  yn2 = yn1; yn1 = y;

  static float peak = 1e-6f;
  if (fabs(y) > peak) peak = fabs(y);

  return (y / peak) * 0.9f;

}

const int oneSecInUsec = 1000000;
int time;

void sample() {

  float out = processSample();

  uint16_t dacVal = (uint16_t)((out * 32767.0f) + 32768.0f); 
  analogWrite(AUDIO, dacVal);

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

  float freq = 50.0f + (randNoise() * 0.5f + 0.5f) * 4000.0f;

  trigger(freq);

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}