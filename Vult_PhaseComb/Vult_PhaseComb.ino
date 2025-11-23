// Vult DSP float - Phase comb oscillator //

#include "CurieTimerOne.h"
#include "Phasecomb.h"

Phasecomb_process_type phasecomb;

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120

float randomf(float minf, float maxf) {return minf + random(1UL << 31)*(maxf - minf) / (1UL << 31);} 

  int time;
  const int oneSecInUsec = 1000000;
  
  float cv;     // control voltage
  float detune; // detune
  float tones;  // comb tone
  float res;    // comb resonance

void sample(){

  int16_t output = 24576.0f * Phasecomb_process(phasecomb, cv , detune, tones, res);

  analogWrite(AUDIO, 32768 + output);
    
  CurieTimerOne.restart(time);
  
}

void setup(){

  pinMode(AUDIO, OUTPUT);

  analogWriteResolution(16);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);
  
  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

  Phasecomb_process_init(phasecomb);
  res = 1.0f;
 
}

void loop(){

  cv = randomf(0.1f, 0.5f);
  detune = randomf(0.1f, 0.6f);
  tones = randomf(0.4f, 0.9f);

  int tempo = 60000 / BPM;
  delay(tempo / 3);
  
}