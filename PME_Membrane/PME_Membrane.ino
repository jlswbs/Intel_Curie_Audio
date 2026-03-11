// PME membrane resonator //

#include "CurieTimerOne.h"
#include "PME.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define NEURONSIZE  128
#define NEURONS     48

Neurons net;

int idx = 1;
int bound = 0;
float decay = 0.0f;

int time;
const int oneSecInUsec = 1000000;

uint8_t modelCurr[NEURONSIZE];
uint8_t modelNext[NEURONSIZE];

float randomf(float minf, float maxf) {return minf + random(1UL << 31)*(maxf - minf) / (1UL << 31);}

void sample() {

  int ncr, aif, cat;
  
  net.readNeuron(idx, &ncr, modelCurr, &aif, &cat);

  int nextIdx = idx + 1;
  if (nextIdx >= bound) nextIdx = 1;

  net.readNeuron(nextIdx, &ncr, modelNext, &aif, &cat);

  uint8_t avgVal = (uint8_t)(((modelCurr[0] + modelNext[0]) / 2.0f) * decay);

  modelCurr[0] = -avgVal;

  net.writeNeuron(idx, modelCurr, rand() % NEURONSIZE);

  analogWrite(AUDIO, avgVal);

  idx++;
  if (idx >= bound) idx = 1;

  CurieTimerOne.restart(time);

}


void setup() {

  net.begin();
  net.clearNeurons();

  pinMode(AUDIO, OUTPUT);
  analogWriteResolution(8);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);

  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

}

void loop() {

  net.forget();

  idx = 1;
  bound = random(8, NEURONS);
  decay = randomf(0.8f, 0.99f);

  uint8_t pattern[NEURONSIZE];

  for (int i = 1; i < bound; i++) {

    pattern[i] = rand();
    net.learn(pattern, bound, 1);

  }

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}