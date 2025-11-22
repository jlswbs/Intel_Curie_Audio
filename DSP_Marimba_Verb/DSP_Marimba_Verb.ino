// Marimba percussion + echoverb //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BUFF_SIZE   2048
#define SIZE        512
#define BPM         120

float randomf(float minf, float maxf) {return minf + random(1UL << 31)*(maxf - minf) / (1UL << 31);}
float clipminmaxf(const float min, const float x, const float max){ return (((x)>=max)?max:((x)<=min)?min:(x)); }
float hard_clip(float x, float lim) { return clipminmaxf(-lim, x, lim); }

  unsigned int buff_pos = 0;
  float buff[BUFF_SIZE];
  float lim = 0.0f;

  int time;
  const int oneSecInUsec = 1000000;

class Synth {
public:
	float out = 0.0f;
  float vol = 0.0f;
  float pitch = 0.0f;
  float last = 0.0f;
  float curr = 0.0f;
  float delaymem[SIZE];
  uint16_t locat = 0;
  float accum = 0.0f;
  float lowpass = 0.0f;

  void trigger(float val);
	float calculate();
};

void Synth::trigger(float val) {

  uint16_t lenght = val * SIZE;

  for (int i = 0; i < SIZE; i++) delaymem[i] = 0;
	for (int i = 0; i < lenght; i++) delaymem[i] = randomf(-1.0f, 1.0f);

}

float Synth::calculate() {

  float incr = SIZE * pitch / SAMPLE_RATE;
	uint16_t bound = SIZE / incr;

	delaymem[locat++] = -out;
  if (locat >= bound) locat = 0;
  curr = delaymem[locat];
  out = 0.5f * accum;
  accum = accum - (lowpass * (accum - (last + curr))); 
  last = curr;

	return vol * out;

}

Synth osc1;

float echo_verb(float sample, float decay) {

  float old_sample = buff[buff_pos];
  float new_sample = (sample + (old_sample * decay)) / 2.0f;
  buff[buff_pos] = new_sample;
  buff_pos = buff_pos + 1;
  if (buff_pos == BUFF_SIZE) buff_pos = 0; 
  return new_sample;

}

void setup(){

  pinMode(AUDIO, OUTPUT);

  analogWriteResolution(16);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);
  
  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

}


void sample(){

  int16_t sample = 32767.0f * echo_verb(hard_clip(osc1.calculate(), lim), 0.6f);

  analogWrite(AUDIO, 32768 + sample);
    
  CurieTimerOne.restart(time);
  
}


void loop(){

  lim = randomf(0.05f, 0.5f);

  osc1.trigger(randomf(0.001f, 0.5f));
  osc1.vol = randomf(0.3f, 1.9f);
  osc1.lowpass = randomf(0.01f, 0.5f);
  osc1.pitch = random(110, 880);

  int tempo = 60000 / BPM;
  delay(tempo / 3);
  
}