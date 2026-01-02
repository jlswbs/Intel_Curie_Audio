// Simple comb oscillator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define MAX_DELAY   512

float delayLine[MAX_DELAY];
int writeIndex = 0;
int delayLength = 0;
float feedback = 0.98f;

int time;
const int oneSecInUsec = 1000000;

float randomf(float minf, float maxf) { return minf + random(1UL << 31)*(maxf - minf) / (1UL << 31); }

void setFrequency(float freq) {

  int len = (int)(SAMPLE_RATE / freq);
  if (len >= MAX_DELAY) len = MAX_DELAY - 1;
  if (len < 2) len = 2;
  delayLength = len;

}

void sample() {

  int readIndex = (writeIndex + 1) % delayLength;
  float delayedSample = delayLine[readIndex];
  float noise = randomf(-0.02f, 0.02f); 
  float output = noise + delayedSample * feedback;
  delayLine[writeIndex] = -output;
  writeIndex = (writeIndex + 1) % delayLength;
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

  float freq = random(110, 880);
  setFrequency(freq);

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}