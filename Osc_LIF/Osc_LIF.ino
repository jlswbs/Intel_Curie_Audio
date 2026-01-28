// Two LIF model neuron oscillator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120

int time;
const int oneSecInUsec = 1000000;

class NeuronOscillator {

public:
  float v1, v2;
  float dt, tau;
  float v_threshold, v_reset;
  float i_ext, w_inhib;
  float last_out = 0;
  float alpha = 0.1f;

  NeuronOscillator(float sampleRate) {
    dt = 1.0f / sampleRate;
    tau = 0.001f;
    v_threshold = 1.0f;
    v_reset = 0.0f;
    i_ext = 1.5f;
    w_inhib = -2.5f;
    v1 = 0.0f;
    v2 = 0.5f;
  }

  float process() {
    
    float raw_out = 0.0f;
    float dv1 = (-v1 + i_ext) * (dt / tau);
    float dv2 = (-v2 + i_ext) * (dt / tau);

    v1 += dv1;
    v2 += dv2;

    if (v1 >= v_threshold) {
      v1 = v_reset;
      v2 += w_inhib;
      raw_out = 1.0f;
    }
        
    if (v2 >= v_threshold) {
      v2 = v_reset;
      v1 += w_inhib;
      raw_out = -1.0f;
    }

    float filtered_out = (alpha * raw_out) + ((1.0f - alpha) * last_out);
    last_out = filtered_out;

    return filtered_out;

  }

  void setFrequency(float f) {

    float targetF = f * 2.0f; 
    i_ext = v_threshold / (1.0f - exp(-1.0f / (targetF * tau)));

  }

};

NeuronOscillator osc(SAMPLE_RATE);

void sample() {

  float gain = 1.0f / osc.alpha; 
  float sample = osc.process() * gain;

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

  osc.setFrequency(random(50, 440));

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}