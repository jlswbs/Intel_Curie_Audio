// FM karplus-strong oscillator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define MAX_DELAY   512

float delayLine[MAX_DELAY];
int writeIndex = 0;
float phase = 0.0f;
int delayLength = 0;

float damping = 0.75f;
float modIndex = 5.0f;
float gain = 1.3f;

int time;
const int oneSecInUsec = 1000000;

void setFrequency(float freq) {

  int len = (int)(SAMPLE_RATE / freq);
  if (len >= MAX_DELAY) len = MAX_DELAY - 1;
  if (len < 2) len = 2;
  delayLength = len;
  writeIndex = 0;

}

float randomf(float minf, float maxf) { return minf + random(1UL << 31)*(maxf - minf) / (1UL << 31); }

void sample() {

  float feedbackVal = delayLine[writeIndex];
  float readOffset = feedbackVal * modIndex;
  int readPos = (writeIndex + (int)readOffset);

  while (readPos < 0) readPos += delayLength;
  readPos = readPos % delayLength;

  float newValue = delayLine[readPos] * damping;
  newValue = tanhf(newValue * gain);

  delayLine[writeIndex] = -newValue;
  writeIndex = (writeIndex + 1) % delayLength;

  uint16_t out = 32767.0f * (1.0f + newValue);
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
  
  for (int i = 0; i < delayLength; i++) delayLine[i] = sinf(PI * (float)i / (float)delayLength);

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}