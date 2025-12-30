// LC resonant analog drum //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120

float L = 0.001f;
float C = 0.000001f;
float R = 10.0f;

float Vcc = 1.0f; 
float Vc = 0.0f;
float Il = 0.0f;

float dt = 1.0f / SAMPLE_RATE;

int time;
const int oneSecInUsec = 1000000;

float randomf(float minf, float maxf) {return minf + random(1UL << 31)*(maxf - minf) / (1UL << 31);}  

float LCOscillator() {

  float L_eff = L * (1.0f + 0.5f * powf(Il / 0.05f, 2.0f));

  if (L_eff < L*0.5f) L_eff = L*0.5f;
  if (L_eff > L*2.0f) L_eff = L*2.0f;

  Il += ((Vc - R * Il) / L_eff) * dt;

  float Vc_nl = Vc + 0.2f * powf(Vc / Vcc, 3);

  Vc += randomf(-1.0f, 1.0f) * 0.0025f;
  Vc += (-Il / C) * dt;

  float out = tanh(Vc_nl);
  return out;

}

void sample(){

  float val = LCOscillator();

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

  Vc += randomf(0.6f, 1.4f);
  L = randomf(0.001f, 0.099f);

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}