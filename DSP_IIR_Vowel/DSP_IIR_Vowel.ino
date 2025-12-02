// IIR vowel resonator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120

float freq = 100.0f;
unsigned long sampleCounter = 0;

float vowels[5][3] = {

  {800.0f, 1150.0f, 2900.0f}, // A
  {400.0f, 1900.0f, 2600.0f}, // E
  {300.0f, 2200.0f, 3000.0f}, // I
  {500.0f, 800.0f, 2600.0f},  // O
  {350.0f, 600.0f, 2400.0f}   // U

};

struct Resonator {

  float f;
  float r;
  float omega;
  float cos_omega;
  float y_prev1;
  float y_prev2;

};

Resonator res[3];
int vowel = 0;

void setupResonators(int vowelIndex) {

  for (int i = 0; i < 3; i++) {

    res[i].f = vowels[vowelIndex][i];
    res[i].r = 0.99f;
    res[i].omega = 2.0f * PI * res[i].f / (float)SAMPLE_RATE;
    res[i].cos_omega = cosf(res[i].omega);
    res[i].y_prev1 = 0.0f;
    res[i].y_prev2 = 0.0f;

  }

}

float processResonator(Resonator &r, float x) {

  float y = x + 2.0f * r.r * r.cos_omega * r.y_prev1 - (r.r * r.r) * r.y_prev2;
  r.y_prev2 = r.y_prev1;
  r.y_prev1 = y;
  return y;

}

float sourceSignal(unsigned long n, float f0) {

  unsigned long period = SAMPLE_RATE / (unsigned long)f0;
  unsigned long pos = n % period;
  
  if (pos < period/2) return 1.0f;
  else return -1.0f;

}

int time;
const int oneSecInUsec = 1000000;

void sample() {

  float x = sourceSignal(sampleCounter++, freq);

  float y = 0.0f;
  for (int i = 0; i < 3; i++) y += processResonator(res[i], x);
  y *= 0.0005f;

  analogWrite(AUDIO, (uint16_t)((y + 1.0f) * 32767.0f));

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

  vowel = random(0, 5);
  freq = random(100, 400);
  setupResonators(vowel);

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}