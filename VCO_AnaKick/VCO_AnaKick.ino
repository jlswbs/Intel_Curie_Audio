// RC stages analog kick drum //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120

const int STAGES = 128;
float stages[STAGES];
float outVal = 0.0f;
float lastInput = 0.0f;
float dcBlockState = 0.0f;
float feedback = 0.9997f;
float cutoff = 0.5f;
float gain = 25.0f;
float bias = 0.05f;
bool polarity = true;

int timeUsec;
const int oneSecInUsec = 1000000;

float randomf(float minf, float maxf) { return minf + random(1UL << 31)*(maxf - minf) / (1UL << 31); }

float fastSoftClip(float x) {
  if (x < -3.0f) return -1.0f;
  if (x > 3.0f) return 1.0f;
  return x * (27.0f + x * x) / (27.0f + 9.0f * x * x);
}

void sample() {

  float input = stages[STAGES - 1] * feedback;
  float prev = input;
  
  for (int i = 0; i < STAGES; i++) {
    stages[i] += cutoff * (prev - stages[i]);
    if(polarity == true) prev = -stages[i];
    else prev = stages[i];
  }
  
  float currentStageOutput = stages[STAGES - 1];
  dcBlockState = currentStageOutput - lastInput + (0.995f * dcBlockState);
  lastInput = currentStageOutput;

  float val = fastSoftClip(gain * (dcBlockState + bias));
  
  uint16_t pwmOut = (uint16_t)((val + 1.0f) * 32767.0f);
  analogWrite(AUDIO, pwmOut);

  CurieTimerOne.restart(timeUsec);

}


void setup() {

  pinMode(AUDIO, OUTPUT);
  analogWriteResolution(16);

  analogWriteFrequency(AUDIO, SAMPLE_RATE);

  timeUsec = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(timeUsec, &sample);
  
}

void loop() {

  for (int i = 0; i < STAGES; i++) stages[i] = randomf(-1.0f, 1.0f);
  cutoff = randomf(0.45f, 0.75f);
  
  int tempo = 60000 / BPM;
  delay(tempo / 3);

}