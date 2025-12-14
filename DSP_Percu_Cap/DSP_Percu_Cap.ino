// Percussion with capacitive envelope //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define TABLE_SIZE  1024

int time;
const int oneSecInUsec = 1000000;

float sineTable[TABLE_SIZE];

float phase = 0.0f;
float phaseInc = 0.0f;
float freq = 440.0f;

float envelope = 0.0f;
float R = 1000.0f;
float C = 1e-6f;
float dt = 1.0f / SAMPLE_RATE;
float leakage = 0.9999f;
float tau = R * C;
float alpha = expf(-dt / tau);

void triggerEnvelope() { envelope = 1.0f; }

void sample() {

  int idx = (int)phase & (TABLE_SIZE - 1);
  float val = sineTable[idx];

  envelope *= alpha;
  envelope *= leakage;

  float shaped = val * envelope;

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

  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

}

void loop() {

  R = random(10000, 100000);
  tau = R * C;
  alpha = expf(-dt / tau);

  freq = random(100, 2000);
  phaseInc = (freq * TABLE_SIZE) / SAMPLE_RATE;

  triggerEnvelope();

  int tempo = 60000 / BPM;
  delay(tempo / 2);

}