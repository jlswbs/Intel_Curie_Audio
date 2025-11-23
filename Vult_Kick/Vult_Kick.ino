// Vult DSP float - Kick 909 oscillator //

#include "CurieTimerOne.h"
#include "Kick.h"

Kick_process_type kick;

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120

float randomf(float minf, float maxf) {return minf + random(1UL << 31)*(maxf - minf) / (1UL << 31);} 

  int time;
  const int oneSecInUsec = 1000000;
  
  float gate, decay, pitch, swept, noise;

void sample(){

  int16_t output = 24576.0f * Kick_process(kick, gate, decay, pitch, swept, noise);

  analogWrite(AUDIO, 32768 + output);
    
  CurieTimerOne.restart(time);
  
}

void setup(){

  pinMode(AUDIO, OUTPUT);

  analogWriteResolution(16);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);
  
  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

  Kick_process_init(kick);
 
}

void loop(){

  decay = randomf(0.01f, 0.2f);
  pitch = randomf(0.05f, 0.4f);
  swept = randomf(0.1f, 0.6f);
  noise = randomf(0.2f, 0.9f);
  gate = 1.0f;

  delay(1);
  
  gate = 0.0f;
  
  int tempo = 60000 / BPM;
  delay(tempo / 2);
  
}