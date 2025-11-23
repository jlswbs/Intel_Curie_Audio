// Vult DSP float - Kick 909 oscillator + reverb //

#include "CurieTimerOne.h"
#include "Kick.h"

Kick_process_type kick;

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120

const int FIXED_BITS = 14;
const int FIXED_SCALE = (1 << FIXED_BITS);
const int REVERB_SIZE = 0x500;
const int REVERB_LENGTH = (int)(REVERB_SIZE * 0.9f);
const int REVERB_DECAY = (int)(FIXED_SCALE * 0.7f);

int16_t reverbAddr;
int reverbBuffer[REVERB_SIZE];

float randomf(float minf, float maxf) {return minf + random(1UL << 31)*(maxf - minf) / (1UL << 31);} 

  int time;
  const int oneSecInUsec = 1000000;
  
  float gate, decay, pitch, swept, noise;

void sample(){

  int16_t sample = 24576.0f * Kick_process(kick, gate, decay, pitch, swept, noise);

  int reverb = ((int)reverbBuffer[reverbAddr] * REVERB_DECAY) >> FIXED_BITS;
  reverb += sample;
  reverbBuffer[reverbAddr] = reverb;
  reverbAddr++;
  if (reverbAddr > REVERB_LENGTH) reverbAddr = 0;
  int16_t output = sample + (reverbBuffer[reverbAddr]>>3);

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

  decay = randomf(0.01f, 0.1f);
  pitch = randomf(0.05f, 0.4f);
  swept = randomf(0.1f, 0.7f);
  noise = randomf(0.1f, 0.9f);
  gate = 1.0f;

  delay(1);
  
  gate = 0.0f;
  
  int tempo = 60000 / BPM;
  delay(tempo / 3);
  
}