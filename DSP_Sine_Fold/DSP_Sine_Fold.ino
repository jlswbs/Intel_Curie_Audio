// Fold sine wave //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define WTB_LEN     2048
#define BPM         120

float randomf(float minf, float maxf) {return minf + random(1UL << 31)*(maxf - minf) / (1UL << 31);}

  float table[WTB_LEN];

  int time;
  const int oneSecInUsec = 1000000;

class Synth {
public:
	float pointer = 0.0f;
  float vol = 0.0f;
	float pitch = 0.0f;
  float lim = 0.0f;
  bool trig = false;
  float decay = 0.0f;
  float d = 0.0f;

	float calculate();
};

float Synth::calculate() {

	int a, b;
	float da, db;

	float wtb_incr = WTB_LEN * (pitch) / SAMPLE_RATE;
	pointer = pointer + wtb_incr;

	if (pointer > WTB_LEN) pointer = pointer - WTB_LEN;

	a = pointer;
	da = pointer - a;
	b = a + 1;
	db = b - pointer;

	if (b >= WTB_LEN) b = 0;

  if (trig == true) {
    d = 1.0f;
    trig = false;
  } else d = d;

  d = d - decay;
  if (d <= 0.0f) d = 0.0f;

	float osc = db * (table[a]) + da * (table[b]);
  
  while (osc > lim || osc < -lim) {
    
    if (osc > lim) osc = lim - (osc - lim);
    else if (osc < -lim) osc = -lim + (-osc-lim);
    
  }

	return vol * (d * osc);

}

Synth osc1;

void setup(){

  pinMode(AUDIO, OUTPUT);

  analogWriteResolution(16);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);
  
  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

  for (int i = 0; i < WTB_LEN; i++) table[i] = sinf(TWO_PI * (i / (float) WTB_LEN));

}


void sample(){

  int16_t sample = 32767.0f * osc1.calculate();
  analogWrite(AUDIO, 32768 + sample);

  CurieTimerOne.restart(time);
  
}


void loop(){

  osc1.trig = true;
  osc1.vol = randomf(0.1f, 0.9f);
  osc1.lim = randomf(0.1f, 0.5f);
  osc1.decay = randomf(0.1f, 0.9f) / SAMPLE_RATE * 10;
  osc1.pitch = randomf(50, 880);

  int tempo = 60000 / BPM;
  delay(tempo / 4);

}