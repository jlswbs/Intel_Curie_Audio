// Vult DSP float - Phase distortion oscillator //

#include "CurieTimerOne.h"
#include "Phasedist.h"

Phasedist_process_type phasedist;

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120

float randomf(float minf, float maxf) {return minf + random(1UL << 31)*(maxf - minf) / (1UL << 31);} 

  int time;
  const int oneSecInUsec = 1000000;
  
  float cv;      // control voltage
  float detune;  // detune

void sample(){

  int16_t output = 24576.0f * Phasedist_process(phasedist, cv , detune);

  analogWrite(AUDIO, 32768 + output);
    
  CurieTimerOne.restart(time);
  
}

void setup(){

  pinMode(AUDIO, OUTPUT);

  analogWriteResolution(16);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);
  
  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

  Phasedist_process_init(phasedist);
 
}

void loop(){

  cv = randomf(0.1f, 0.4f);
  detune = randomf(0.1f, 0.9f);

  int tempo = 60000 / BPM;
  delay(tempo / 3);
  
}