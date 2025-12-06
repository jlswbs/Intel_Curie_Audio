// Simple waveshape LUT oscillator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define TABLE_SIZE  1024

float sineTable[TABLE_SIZE];
float phaseC = 0.0f;
float phaseIncC = 0.0f;
float freqC = 440.0f;
float shapeAmount = 0.8f;

int time;
const int oneSecInUsec = 1000000;

float waveShape(float x) {

  float y = x + shapeAmount * (x * x * x);
  if (y > 1.0f) y = 1.0f;
  if (y < -1.0f) y = -1.0f;

  return y;

}

void sample() {

  phaseC += phaseIncC;
  if (phaseC >= TABLE_SIZE) phaseC -= TABLE_SIZE;

  int idxC = (int)phaseC & (TABLE_SIZE - 1);
  float val = sineTable[idxC];
  float shaped = waveShape(val);

  uint16_t out = (uint16_t)(32767.0f * (1.0f + shaped));
  analogWrite(AUDIO, out);

  CurieTimerOne.restart(time);

}

void setup() {

  pinMode(AUDIO, OUTPUT);

  analogWriteResolution(16);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);

  for (int i = 0; i < TABLE_SIZE; i++) sineTable[i] = sinf(2.0f * PI * i / TABLE_SIZE);

  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

}

void loop() {

  freqC = random(100, 880);
  phaseIncC = (freqC * TABLE_SIZE) / SAMPLE_RATE;

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}