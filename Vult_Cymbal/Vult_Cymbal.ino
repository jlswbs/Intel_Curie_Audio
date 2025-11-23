// Vult DSP float - Cymbal oscillator //

#include "CurieTimerOne.h"
#include "Cymbal.h"

Cymbal_process_type cymbal;

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120

float randomf(float minf, float maxf) {return minf + random(1UL << 31)*(maxf - minf) / (1UL << 31);} 

  int time;
  const int oneSecInUsec = 1000000;
  
  float gate, decay, pitch;

void sample(){

  int16_t output = 24576.0f * Cymbal_process(cymbal, gate, decay, pitch);

  analogWrite(AUDIO, 32768 + output);
    
  CurieTimerOne.restart(time);
  
}

void setup(){

  pinMode(AUDIO, OUTPUT);

  analogWriteResolution(16);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);
  
  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

  Cymbal_process_init(cymbal);
 
}

void loop(){

  decay = randomf(0.001f, 0.05f);
  pitch = randomf(0.1f, 1.0f);
  gate = 1.0f;

  delay(1);
  
  gate = 0.0f;
  
  int tempo = 60000 / BPM;
  delay(tempo / 3);
  
}