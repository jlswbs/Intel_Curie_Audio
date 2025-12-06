// Simple hard sync LUT oscillator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define TABLE_SIZE  1024

float sineTable[TABLE_SIZE];

float phaseM = 0.0f;
float phaseIncM = 0.0f;
float freqM = 220.0f;

float phaseS = 0.0f;
float phaseIncS = 0.0f;
float freqS = 440.0f;

int time;
const int oneSecInUsec = 1000000;

void sample() {

  phaseM += phaseIncM;
  if (phaseM >= TABLE_SIZE) {
    phaseM -= TABLE_SIZE;
    phaseS = 0.0f;
  }

  phaseS += phaseIncS;
  if (phaseS >= TABLE_SIZE) phaseS -= TABLE_SIZE;
  float val = sineTable[(int)phaseS & (TABLE_SIZE - 1)];

  uint16_t out = (uint16_t)(32767.0f * (1.0f + val));
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

  freqM = random(50, 220);
  phaseIncM = (freqM * TABLE_SIZE) / SAMPLE_RATE;

  freqS = random(100, 880);
  phaseIncS = (freqS * TABLE_SIZE) / SAMPLE_RATE;

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}