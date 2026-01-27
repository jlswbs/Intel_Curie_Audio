// MIM resonator (mass interaction model) //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120

int time;
const int oneSecInUsec = 1000000;

class MIMResonator {
private:
  float x = 0.0f;
  float v = 0.0f;
  float force = 0.0f;
  float dt;
  float m = 1.0f;
  float k_lin = 0.0f;
  float k_nonlin = 0.0f;
  float damping = 50.0f;
  float nonlin = 1e6;
    
public:
  MIMResonator(float sampleRate) {
    dt = 1.0f / sampleRate;
  }

  void trigger(float freq) {

    x = 0.0f;
    v = 0.0f;
    force = random(10000, 50000); 
    k_lin = m * powf(2.0f * PI * freq, 2.0f);
    k_nonlin = k_lin * nonlin;

  }

  float process() {

    float f_spring = -(k_lin * x + k_nonlin * (x * x * x));
    float f_damping = damping * v;
    float a = (force + f_spring - f_damping) / m;

    v += a * dt;
    x += v * dt;

    force = 0.0f;

    return x;

  } 

};

MIMResonator drum(SAMPLE_RATE);

void sample() {

  float sample = tanhf(drum.process() * 1000.0f);
  uint16_t out = (uint16_t)((sample + 1.0f) * 32767.0f);
  
  analogWrite(AUDIO, out);

  CurieTimerOne.restart(time);

}

void setup() {

  pinMode(AUDIO, OUTPUT);

  analogWriteResolution(16);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);

  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

}

void loop() {

  drum.trigger(random(55, 880));

  int tempo = 60000 / BPM;
  delay(tempo / 2);

}