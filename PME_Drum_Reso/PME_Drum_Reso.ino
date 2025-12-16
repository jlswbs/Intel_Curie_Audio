// PME Lo-Fi resonant drum machine //

#include "CurieTimerOne.h"
#include "PME.h"
#include "kick.h"
#include "snare.h"
#include "clap.h"
#include "hihat.h"

#define AUDIO       5
#define SAMPLE_RATE 8000
#define BPM         120
#define NEURONSIZE  128

Neurons net;

const uint8_t* drums[4] = { kick, snare, clap, hihat };

int idx = 0;
int neuron = 1;
int bound = 0;
float decay = 0.0f;

int time;
const int oneSecInUsec = 1000000;

void sample() {

  uint8_t model[NEURONSIZE];

  int ncr, aif, cat;
  net.readNeuron(neuron, &ncr, model, &aif, &cat);
  uint8_t val = model[idx];
  
  val = val * decay;
  model[idx] = val;
  net.writeNeuron(neuron, model, 1);

  analogWrite(AUDIO, val);

  idx++;
  if (idx >= bound) idx = 0;

  CurieTimerOne.restart(time);

}

void setup() {

  pinMode(AUDIO, OUTPUT);
  analogWriteResolution(8);
  analogWriteFrequency(AUDIO, 8 * SAMPLE_RATE);

  net.begin();
  net.forget();

  uint8_t pattern[NEURONSIZE];
  for (int i = 0; i < NEURONSIZE; i++) pattern[i] = rand();
  net.learn(pattern, NEURONSIZE, 1);

  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

}

void loop() {

  idx = 0;

  neuron = random(1, 5);
  bound = random(1, 128);
  decay = random(500, 1000) / 1000.0f;

  uint8_t pattern[NEURONSIZE];
  
  for (int i = 0; i < bound; i++) pattern[i] = drums[neuron-1][i];
  net.writeNeuron(neuron, pattern, 1);

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}