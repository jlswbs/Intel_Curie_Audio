// Transistor RC phase-shift oscillator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120

float R = 10000.0f;
float C = 0.000000047f;
float RC = R * C;

float x1 = 0.01f;
float x2 = 0.0f;
float x3 = 0.0f;

float G = -29.0f;

float dt = 1.0f / SAMPLE_RATE;

int time;
const int oneSecInUsec = 1000000;

float randomf(float minf, float maxf) { return minf + random(1UL << 31)*(maxf - minf) / (1UL << 31); }

void sample() {

  float drive = G * x3;
  float amp = tanhf(drive); 

  x1 += (amp - x1) * (dt / RC);
  x2 += (x1 - x2) * (dt / RC);
  x3 += (x2 - x3) * (dt / RC);

  uint16_t out = 32767.0f * (1.0f + (0.3f * amp));
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
  RC = R * C;

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}