// Grain distorted sample mangler //

#include "CurieTimerOne.h"
#include "sample.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define SIZE        100

  int add = 2;
  int len = SAMPLE_COUNT;
  int cnt = 0;

  const unsigned int d_size = SIZE;
  int d_time, i, j;
  bool rev = true;
  float delay_1[d_size + 1];
  float delay_2[d_size + 1];

  int time;
  const int oneSecInUsec = 1000000;


float diode_clip(float input, float thres) {
  
  float in, out;
  float buf = input;
  in = fabs(input) / thres;
  if (in <= (1.0f / 3.0f)) out = 2.0f * in;
  else if (in <= (2.0f / 3.0f)) out = (-3.0f * (in*in)) + (4.0f * in) - (1.0f / 3.0f);
  else out = 1.0f;
  out = out * thres;
  if (buf <= 0.0f) out = -1.0f * out;
  return out;

}

float grain(float d_in) {

  float d_out;
  i++; if (i  > d_time) i = 0;
  delay_1[i] = d_in;
  if (i == d_time) j = 0;
  delay_2[i]  = delay_1[i];
  j++; if (j > d_time) j = 0;
  if (!rev) d_out = delay_2[j];
  if (rev) d_out = delay_2[d_time - j];
  return d_out;

}

void sample(){

  uint16_t out = 32767.0f * (1.0f + diode_clip( grain(audio_table[cnt]), 0.35f));

  analogWrite(AUDIO, out);

  if(cnt >= len){
    
    cnt = random(0, len-1);
    d_time = random(8, SIZE);
    add = random(1, 20);

  }

  cnt = cnt + add;
 
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