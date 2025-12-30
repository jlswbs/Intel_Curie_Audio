// LC resonant oscillator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120

float L = 0.001f;
float C = 0.0001f;
float R = 0.0f;

float Vc = 0.1f;
float Il = 0.0f;

float dt = 1.0f / SAMPLE_RATE;

int time;
const int oneSecInUsec = 1000000;

float randomf(float minf, float maxf) {return minf + random(1UL << 31)*(maxf - minf) / (1UL << 31);}  

float simulateLCOscillator() {

  Il += ((Vc - R * Il) / L) * dt;
  Vc += (-Il / C) * dt;

  float out = tanh(5.0f * Vc);

  return out;

}

void sample() {

  float val = simulateLCOscillator();

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