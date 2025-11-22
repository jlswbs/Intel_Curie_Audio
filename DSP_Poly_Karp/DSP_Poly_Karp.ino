// Poly karplus-strong with distortion + echoverb //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BUFF_SIZE   2048
#define SIZE        256
#define POLY        2

float randomf(float minf, float maxf) {return minf + random(1UL << 31)*(maxf - minf) / (1UL << 31);}
float dist_filter(float input, float peak){ input = input * 4.0f / 3.0f; return clip(input, peak); }
float clip(float cval, float peak){ return (cval > 0) ? ((cval < peak) ? cval : peak) : ((cval > (-1.0f * peak)) ? cval : (-1.0f * peak)); }

  unsigned int buff_pos = 0;
  float buff[BUFF_SIZE];

  float r[POLY];
  float x[POLY];
  float b[POLY];
  float out[POLY];
  float last[POLY];
  float curr[POLY];
  float delaymem[POLY][SIZE];
  uint16_t locat[POLY];
  uint16_t bound[POLY];
  float accum[POLY];
  float lowpass[POLY];
  float nx[POLY];
  float lim = 0.1f;

  int time;
  const int oneSecInUsec = 1000000;

float echo_verb(float sample, float decay) {

  float old_sample = buff[buff_pos];
  float new_sample = (sample + (old_sample * decay)) / 2.0f;
  buff[buff_pos] = new_sample;
  buff_pos = buff_pos + 1;
  if (buff_pos == BUFF_SIZE) buff_pos = 0; 
  return new_sample;

}

void sample(){

  for (int j = 0; j < POLY; j++) {
  
    delaymem[j][locat[j]++] = out[j];
    if (locat[j] >= bound[j]) locat[j] = 0;
    curr[j] = delaymem[j][locat[j]];
    out[j] = 0.5f * accum[j];
    accum[j] = accum[j] - (b[j] * (accum[j] - (last[j] + curr[j]))); 
    last[j] = curr[j];

  }

  float outs = 0.0f;
  for (int l = 0; l < POLY; l++) outs += out[l];
  outs = outs / POLY;
  int16_t sample = 32767.0f * echo_verb(dist_filter(outs, lim), 0.7f);

  analogWrite(AUDIO, 32768 + sample);
    
  CurieTimerOne.restart(time);
  
}

void setup(){

  pinMode(AUDIO, OUTPUT);

  analogWriteResolution(16);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);
  
  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

  x[0] = 0.1f;
  x[1] = 0.1f;
  r[0] = 3.8f;
  r[1] = 3.9f;

}

void loop(){

  for (int k = 0; k < POLY; k++) {

    nx[k] = x[k];
    x[k] = r[k] * nx[k] * (1.0f - nx[k]);

  }

  for (int i = 0; i < SIZE; i++) delaymem[0][i] = randomf(-1.0f, 1.0f);
  bound[0] = map(SIZE * x[0], 0, SIZE, 16, SIZE);
  b[0] = x[1];

  int tempo = 60000 / 120;

  delay(tempo/2);

  for (int i = 0; i < SIZE; i++) delaymem[1][i] = randomf(-1.0f, 1.0f);
  bound[1] = map(SIZE * x[1], 0, SIZE, 16, SIZE);
  b[1] = x[0];

  delay(tempo/4);
  
}