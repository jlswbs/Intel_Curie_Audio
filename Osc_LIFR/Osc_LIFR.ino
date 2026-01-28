// Ring LIF model neuron oscillator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define NUM_NEURONS 3

int time;
const int oneSecInUsec = 1000000;

class RingNeuronOscillator {
public:
  float v[NUM_NEURONS];
  float dt, tau;
  float v_threshold, v_reset;
  float i_ext, w_inhib;
  float last_out = 0;
  float alpha = 0.1f;

  RingNeuronOscillator(float sampleRate) {

    dt = 1.0f / sampleRate;
    tau = 0.005f;
    v_threshold = 1.0f;
    v_reset = 0.0f;
    i_ext = 1.5f;
    w_inhib = -2.0f;
    
    for(int i=0; i<NUM_NEURONS; i++) {
      v[i] = (float)i * 0.3f;
    }

  }

  float process() {

    float raw_out = 0.0f;

    for (int i = 0; i < NUM_NEURONS; i++) {

      float dv = (-v[i] + i_ext) * (dt / tau);
      v[i] += dv;

      if (v[i] >= v_threshold) {
        v[i] = v_reset;
        int next = (i + 1) % NUM_NEURONS;
        v[next] += w_inhib; 
        raw_out += (i % 2 == 0) ? 1.0f : -1.0f; 
      }

    }

    float filtered_out = (alpha * raw_out) + ((1.0f - alpha) * last_out);
    last_out = filtered_out;
    return filtered_out;

  }

  void setFrequency(float f) {

    float targetF = f * NUM_NEURONS; 
    i_ext = v_threshold / (1.0f - exp(-1.0f / (targetF * tau)));

  }

};

RingNeuronOscillator osc(SAMPLE_RATE);

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