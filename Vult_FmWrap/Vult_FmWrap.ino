// Vult DSP float - FM sine wrap oscillator //

#include "CurieTimerOne.h"
#include "Fmwrap.h"

Fmwrap_process_type fmwrap;

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120

float randomf(float minf, float maxf) {return minf + random(1UL << 31)*(maxf - minf) / (1UL << 31);} 

  int time;
  const int oneSecInUsec = 1000000;
  
  float cv;   // control voltage
  float rst;  // reset oscillator
  float wrap; // wrap distortion

void sample(){

  int16_t output = 24576.0f * Fmwrap_process(fmwrap, cv, rst, wrap);

  analogWrite(AUDIO, 32768 + output);
    
  CurieTimerOne.restart(time);
  
}

void setup(){

  pinMode(AUDIO, OUTPUT);

  analogWriteResolution(16);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);
  
  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

  Fmwrap_process_init(fmwrap);
 
}

void loop(){

  cv = randomf(0.05f, 0.7f);
  wrap = randomf(0.1f, 0.9f);
  rst = 0.0;

  int tempo = 60000 / BPM;
  delay(tempo / 3);
  
}