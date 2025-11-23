// Vult DSP float - Additive sine fold oscillator //

#include "CurieTimerOne.h"
#include "Addfold.h"

Addfold_process_type addfold;

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120

float randomf(float minf, float maxf) {return minf + random(1UL << 31)*(maxf - minf) / (1UL << 31);} 

  int time;
  const int oneSecInUsec = 1000000;
  
  float cv1;  // control voltage osc1
  float cv2;  // control voltage osc2
  float rst;  // reset oscillator
  float fold; // fold distortion

void sample(){

  int16_t output = 24576.0f * Addfold_process(addfold, cv1, cv2, rst, fold);

  analogWrite(AUDIO, 32768 + output);
    
  CurieTimerOne.restart(time);
  
}

void setup(){

  pinMode(AUDIO, OUTPUT);

  analogWriteResolution(16);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);
  
  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

  Addfold_process_init(addfold);
 
}

void loop(){

  cv1 = randomf(0.1f, 0.5f);
  cv2 = randomf(0.3f, 0.8f);
  fold = randomf(0.1f, 0.9f);
  rst = 0.0;

  int tempo = 60000 / BPM;
  delay(tempo / 3);
  
}