// AM karplus-strong oscillator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define MAX_DELAY   512

float delayLine[MAX_DELAY];
int writeIndex = 0;
int delayLength = 0;

float damping = 0.8f;
float amIntensity = 0.7f;
float gain = 1.2f;

int time;
const int oneSecInUsec = 1000000;

void setFrequency(float freq) {

  int len = (int)(SAMPLE_RATE / freq);
  if (len >= MAX_DELAY) len = MAX_DELAY - 1;
  if (len < 2) len = 2;
  delayLength = len;
  writeIndex = 0;

}

void sample() {

  float feedbackVal = delayLine[writeIndex];
  float amModulator = 1.0f + (feedbackVal * amIntensity);
  float newValue = feedbackVal * amModulator * damping;

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