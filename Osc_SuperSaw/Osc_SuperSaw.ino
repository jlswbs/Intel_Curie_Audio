// SuperSaw polyblep oscillator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120

int time;
const int oneSecInUsec = 1000000;

float freq = 220;

float poly_blep(float t, float dt) {

  if (t < dt) {
    t /= dt;
    return t + t - t * t - 1.0f;
  } else if (t > 1.0f - dt) {
    t = (t - 1.0f) / dt;
    return t * t + t + t + 1.0f;
  }
  return 0.0f;

}

float get_saw(float phase, float phase_inc) {

  float naive_saw = 2.0f * phase - 1.0f;
  return naive_saw - poly_blep(phase, phase_inc);

}

struct SuperSaw {

  float phases[7];
  float detune_offsets[7] = { -0.05f, -0.03f, -0.01f, 0.0f, 0.01f, 0.03f, 0.05f };

  float process(float base_freq, float sample_rate, float mix, float detune_amount) {
    float output = 0;
    for (int i = 0; i < 7; ++i) {
      float freq = base_freq * (1.0f + detune_offsets[i] * detune_amount);
      float phase_inc = freq / sample_rate;
            
      output += get_saw(phases[i], phase_inc) * (i == 3 ? 1.0f : mix);
            
      phases[i] += phase_inc;
      if (phases[i] >= 1.0f) phases[i] -= 1.0f;
    }
    return output / (1.0f + 6.0f * mix);
  }

};

SuperSaw osc;

void sample() {

  float sample = osc.process(freq, SAMPLE_RATE, 0.5f, 0.5f);

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

  for(int i=0; i<7; i++) osc.phases[i] = (float)random(0, 1000) / 1000.0f;

}

void loop() {

  freq = random(50, 440);

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}