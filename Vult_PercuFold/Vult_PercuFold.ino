// Vult DSP float - Percussion fold oscillator //

#include "CurieTimerOne.h"
#include "Percus.h"

Percus_process_type percus;

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120

float randomf(float minf, float maxf) {return minf + random(1UL << 31)*(maxf - minf) / (1UL << 31);} 

  int time;
  const int oneSecInUsec = 1000000;
  
  float gate, decay, pitch, fold, noise;

void sample(){

  int16_t output = 24576.0f * Percus_process(percus, gate, decay, pitch, fold, noise);

  analogWrite(AUDIO, 32768 + output);
    
  CurieTimerOne.restart(time);
  
}

void setup(){

  pinMode(AUDIO, OUTPUT);

  analogWriteResolution(16);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);
  
  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

  Percus_process_init(percus);
 
}

void loop(){

  decay = randomf(0.001f, 0.005f);
  pitch = randomf(0.01f, 0.5f);
  fold = randomf(0.1f, 1.0f);
  noise = randomf(0.1f, 1.0f);
  gate = 1.0f;

  delay(1);
  
  gate = 0.0f;
  
  int tempo = 60000 / BPM;
  delay(tempo / 3);
  
}