// Vult DSP float - Saw moog oscillator //

#include "CurieTimerOne.h"
#include "Sawmoog.h"

Sawmoog_process_type sawmoog;

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120

float randomf(float minf, float maxf) {return minf + random(1UL << 31)*(maxf - minf) / (1UL << 31);} 

  int time;
  const int oneSecInUsec = 1000000;
  
  float cv;   // control voltage
  float cut;  // filter cutoff
  float res;  // filter resonance

void sample(){

  int16_t output = 24576.0f * Sawmoog_process(sawmoog, cv, cut, res);

  analogWrite(AUDIO, 32768 + output);
    
  CurieTimerOne.restart(time);
  
}

void setup(){

  pinMode(AUDIO, OUTPUT);

  analogWriteResolution(16);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);
  
  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

  Sawmoog_process_init(sawmoog);
 
}

void loop(){

  cv = randomf(0.2f, 0.6f);
  cut = randomf(0.3f, 0.9f);
  res = randomf(0.2f, 0.9f);

  int tempo = 60000 / BPM;
  delay(tempo / 3);
  
}