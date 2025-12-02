// Biquad vowel resonator //

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

float vowelBW[5][3] = {

  {80.0f, 90.0f, 150.0f},   // A
  {70.0f, 100.0f, 150.0f},  // E
  {60.0f, 100.0f, 200.0f},  // I
  {80.0f, 90.0f, 150.0f},   // O
  {70.0f, 90.0f, 150.0f}    // U

};

struct Biquad {

  float b0, b1, b2;
  float a1, a2;
  float x1, x2;
  float y1, y2;

};

Biquad res[3];
int vowel = 0;

void setupFormant(int idx, float f0, float BW) {

  float omega = 2.0f * PI * f0 / SAMPLE_RATE;
  float Q = f0 / BW;
  float alpha = sinf(omega) / (2.0f * Q);

  float b0 = alpha;
  float b1 = 0.0f;
  float b2 = -alpha;
  float a0 = 1.0f + alpha;
  float a1 = -2.0f * cosf(omega);
  float a2 = 1.0f - alpha;

  res[idx].b0 = b0 / a0;
  res[idx].b1 = b1 / a0;
  res[idx].b2 = b2 / a0;
  res[idx].a1 = a1 / a0;
  res[idx].a2 = a2 / a0;

  res[idx].x1 = res[idx].x2 = 0.0f;
  res[idx].y1 = res[idx].y2 = 0.0f;

}

void setupResonators(int vowelIndex) {

  for (int i = 0; i < 3; i++) setupFormant(i, vowels[vowelIndex][i], vowelBW[vowelIndex][i]);

}

float processBiquad(Biquad &r, float x) {

  float y = r.b0*x + r.b1*r.x1 + r.b2*r.x2 - r.a1*r.y1 - r.a2*r.y2;

  r.x2 = r.x1; r.x1 = x;
  r.y2 = r.y1; r.y1 = y;

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
  for (int i = 0; i < 3; i++) y += processBiquad(res[i], x);

  y *= 0.4f;

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
  freq = random(50, 400);
  setupResonators(vowel);

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}