// Rainy noise glitch + echoverb //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BUFF_SIZE   2048

  unsigned int buff_pos = 0;
  float buff[BUFF_SIZE];

  int time;
  const int oneSecInUsec = 1000000;

class Synth {

  public:
  int16_t lp1, lp2, bp2, lowns, sah, lfrt;
  uint16_t tempo = 1023;
  uint16_t gen = 128;
  uint16_t fil = 9;
  int16_t calculate();

};

int16_t Synth::calculate() {

  int16_t rndm = rand();
    
  if (lfrt-- == 0) {
    lfrt = (rndm & 256) + tempo;
    sah = rndm;
  }
    
  bp2 = lp1/4 + (sah/5 - bp2/gen - lp2) / 2 + bp2;
  lp2 = bp2/2 + lp2 + sah;                   
  lowns += (rndm-lowns) / 5  + (rndm / 32);
  lp1 += (rndm/8 - lp1) / fil;

  return (lp1 + bp2/3 + lowns/16);

}

Synth rainy;

float echo_verb(float sample, float decay) {

  float old_sample = buff[buff_pos];
  float new_sample = (sample + (old_sample * decay)) / 2.0f;
  buff[buff_pos] = new_sample;
  buff_pos = buff_pos + 1;
  if (buff_pos == BUFF_SIZE) buff_pos = 0; 
  return new_sample;

}

void sample(){

  int16_t sample = echo_verb(rainy.calculate(), 0.3f);
  analogWrite(AUDIO, 32768 + sample);

  CurieTimerOne.restart(time);
  
}

void setup(){

  pinMode(AUDIO, OUTPUT);

  analogWriteResolution(16);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);
  
  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

}

void loop(){

}