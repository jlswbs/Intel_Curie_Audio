// Simple comb oscillator - fractional delay //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define MAX_DELAY   512

float delayLine[MAX_DELAY];
int writeIndex = 0;
float delayLength = 0;
float feedback = 0.98f;

int time;
const int oneSecInUsec = 1000000;

float randomf(float minf, float maxf) { return minf + random(1UL << 31)*(maxf - minf) / (1UL << 31); }

void setFrequency(float freq) {

  float len = (float)SAMPLE_RATE / freq;
  
  if (len >= MAX_DELAY) len = (float)MAX_DELAY - 1.0f;
  if (len < 2.0f) len = 2.0f;
  
  delayLength = len;

}

void sample() {

  float readPos = (float)writeIndex - delayLength;
  if (readPos < 0) readPos += (float)delayLength;

  int i0 = (int)readPos;
  int i1 = (i0 + 1) % (int)delayLength;
  float frac = readPos - (float)i0;
  float delayedSample = delayLine[i0] * (1.0f - frac) + delayLine[i1] * frac;
  float noise = randomf(-0.02f, 0.02f); 
  float output = noise + delayedSample * feedback;

  delayLine[writeIndex] = -output;
  
  writeIndex = (writeIndex + 1) % (int)delayLength;
  output = tanhf(output);

  uint16_t out = 32767.0f * (1.0f + output);
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

  float freq = randomf(110, 880); 
  setFrequency(freq);

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}