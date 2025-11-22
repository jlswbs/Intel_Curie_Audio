// IIR fold sine filter drums //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120

float randomf(float minf, float maxf) {return minf + random(1UL << 31)*(maxf - minf) / (1UL << 31);}

  float lim = 0.0f;

  int time;
  const int oneSecInUsec = 1000000;

class Synth {
public:

  float osc = 0.0f;
  float A = 1.975f;
  float y[3] = {0, 0, 0};
  float acc = 0.0f;
	float pitch = 0.0f;
  float lim = 0.0f;
  bool gate = 0;
  bool type = 0;
  float decay = 0.0f;
  float d = 0.0f;

	float calculate();

};

float Synth::calculate() {

  y[0] = ((A+pitch) * y[1]) - y[2];
	y[2] = y[1];
	y[1] = y[0];
	
  if (gate == 1){

    d = 1.0f;
    y[1] = acc;

  }

  d = d - decay;
  if (d <= 0.0f) d = 0.0f;

  if (type) osc = y[rand()%3];
  else osc = y[2];

  while (osc > lim || osc < -lim){
    
    if (osc > lim) osc = lim - (osc - lim);
    else if (osc < -lim) osc = -lim + (-osc-lim);
    
  }

	return d * osc;

}

Synth drum;

void setup(){

  pinMode(AUDIO, OUTPUT);

  analogWriteResolution(16);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);
  
  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

}


void sample(){

  int16_t sample = 32767.0f * drum.calculate();
  analogWrite(AUDIO, 32768 + sample);

  CurieTimerOne.restart(time);
  
}


void loop(){

  drum.gate = 1;
  drum.type = rand()%2;
  drum.acc = randomf(0.5f, 1.0f);
  drum.lim = randomf(0.2f, 0.9f);
  drum.decay = randomf(1.0f, 1.5f) / SAMPLE_RATE * 10;
  drum.pitch = randomf(0.01f, 0.025f);

  delay(1);

  drum.gate = 0;

  int tempo = 60000 / BPM;
  delay((tempo / 3) - 1);

}