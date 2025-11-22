// Float karplus-strong + wrap filter + echoverb //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BUFF_SIZE   2048
#define SIZE        512
#define BPM         120

float randomf(float minf, float maxf) {return minf + random(1UL << 31)*(maxf - minf) / (1UL << 31);}

  unsigned int buff_pos = 0;
  float buff[BUFF_SIZE];

  float r = 3.5699456f;
  float x = 0.1f;
  float b = 0.0f;
  float out = 0.0f;
  float last = 0.0f;
  float curr = 0.0f;
  float delaymem[SIZE];
  uint16_t locat = 0;
  uint16_t bound = SIZE;
  float accum = 0.0f;
  float lowpass = 0.0f;
  float lim = 0.25f;

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

float wrap(float x, float lim) {
  
  float out = x;
  while (out > lim || out < -lim) {
    if (out > lim) out = -lim + (out-lim);
    else if (out < -lim) out = lim - (-out-lim);
  }
  return out;

}

void sample(){

  delaymem[locat++] = wrap(out, lim);
  if (locat >= bound) locat = 0;
  curr = delaymem[locat];
  out = 0.5f * accum;
  accum = accum - (lowpass * (accum - (last + curr))); 
  last = curr;

  float y = echo_verb(out, 0.5f);

  static float peak = 1e-6f;
  if (fabs(y) > peak) peak = fabs(y);
  y = (y / peak) * 0.9f;

  int16_t sample = 32767.0f * y;

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

  float nx = x;
  x = r * nx * (1.0f - nx);

  lowpass = randomf(0.049f, x);

  for (int i = 0; i < SIZE; i++) delaymem[i] = randomf(-1.0f, 1.0f);
  bound = map(SIZE * x, 0, SIZE, 16, SIZE);

  int tempo = 60000 / BPM;
  delay(tempo / 3);
  
}