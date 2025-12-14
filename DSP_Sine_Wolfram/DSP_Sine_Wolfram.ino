// Sine wave cellular automaton shaper //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define TABLE_SIZE  1024
#define CA_SIZE     12
#define RULE        110

int time;
const int oneSecInUsec = 1000000;

float sineTable[TABLE_SIZE];

float phase = 0.0f;
float phaseInc = 0.0f;
float freq = 440.0f;

uint8_t ca[CA_SIZE];
uint8_t next[CA_SIZE];

static inline uint8_t applyRule(uint8_t left, uint8_t center, uint8_t right) {

  uint8_t pattern = (left << 2) | (center << 1) | right;
  return (RULE >> (7 - pattern)) & 1;

}

static inline float caShape(float bias) {

  int idx = (int)((bias + 1.0f) * 0.5f * (CA_SIZE - 1));

  for (int i = 0; i < CA_SIZE; i++) {
    uint8_t left   = (i == 0) ? ca[CA_SIZE-1] : ca[i-1];
    uint8_t center = ca[i];
    uint8_t right  = (i == CA_SIZE-1) ? ca[0] : ca[i+1];
    next[i] = applyRule(left, center, right);
  }
  for (int i = 0; i < CA_SIZE; i++) ca[i] = next[i];

  return ca[idx] ? 1.0f : -1.0f;

}

void sample() {

  int idx = (int)phase & (TABLE_SIZE - 1);
  float val = sineTable[idx];
  float shaped = caShape(val);

  uint16_t out = (uint16_t)((1.0f + shaped) * 32767.0f);
  analogWrite(AUDIO, out);

  phase += phaseInc;
  if (phase >= TABLE_SIZE) phase -= TABLE_SIZE;

  CurieTimerOne.restart(time);

}

void setup() {

  pinMode(AUDIO, OUTPUT);

  analogWriteResolution(16);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);

  for (int i = 0; i < TABLE_SIZE; i++) sineTable[i] = sinf(2.0f * PI * i / TABLE_SIZE);
  for (int i = 0; i < CA_SIZE; i++) ca[i] = random(0,2);

  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

}

void loop() {

  freq = random(100, 1000);
  phaseInc = (freq * TABLE_SIZE) / SAMPLE_RATE;

  int tempo = 60000 / BPM;
  delay(tempo / 4);

}