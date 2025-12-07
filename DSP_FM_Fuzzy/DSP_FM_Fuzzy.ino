// FM fuzzy logic shaper //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define TABLE_SIZE  1024

int time;
const int oneSecInUsec = 1000000;

float sineTable[TABLE_SIZE];

float phaseC = 0.0f;
float phaseIncC = 0.0f;
float freqC = 440.0f;

float phaseM = 0.0f;
float phaseIncM = 0.0f;
float freqM = 220.0f;
float fmIndex = 5.0f;

float SIGMA = 0.5f;
float RULE_OFFSET = 0.3f;
float RULE_SCALE  = 0.7f;

inline float gaussian(float x, float mu, float sigma) {
  float d = (x - mu) / sigma;
  return expf(-0.5f * d * d);
}

inline float soft_clip(float x, float c) {
  return x / (1.0f + c * fabsf(x));
}

inline float compress(float x, float k) {
  return tanhf(k * x);
}

inline float fuzzyShape(float x) {

  float s_small  = gaussian(x, 0.0f, SIGMA);
  float s_medium = gaussian(fabsf(x), 0.5f, SIGMA * 1.2f);
  float s_large  = 1.0f - gaussian(fabsf(x), 0.0f, SIGMA * 0.8f);

  float w_sum = s_small + s_medium + s_large + 1e-6f;
  s_small  /= w_sum;
  s_medium /= w_sum;
  s_large  /= w_sum;

  float y_lin  = x;
  float y_comp = compress(x, 1.5f);
  float y_clip = soft_clip(x, 1.2f);

  float y = s_small * y_lin + s_medium * y_comp + s_large * y_clip;

  y += RULE_OFFSET * (0.5f * (1.0f - fabsf(x)) * (x >= 0 ? 1.0f : -1.0f));
  y = (1.0f - RULE_SCALE) * x + RULE_SCALE * y;

  return fmaxf(-1.0f, fminf(1.0f, y));

}

void sample() {

  int idxM = (int)phaseM & (TABLE_SIZE - 1);
  float modVal = sineTable[idxM];
  phaseM += phaseIncM;
  if (phaseM >= TABLE_SIZE) phaseM -= TABLE_SIZE;

  phaseC += phaseIncC + (fmIndex * phaseIncM * modVal);
  if (phaseC >= TABLE_SIZE) phaseC -= TABLE_SIZE;

  int idxC = (int)phaseC & (TABLE_SIZE - 1);
  float val = sineTable[idxC];
  float shaped = fuzzyShape(val);

  uint16_t out = (uint16_t)((1.0f + shaped) * 32767.0f);

  analogWrite(AUDIO, out);

  CurieTimerOne.restart(time);

}

void setup() {

  pinMode(AUDIO, OUTPUT);

  analogWriteResolution(16);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);

  for (int i = 0; i < TABLE_SIZE; i++) sineTable[i] = sinf(2.0f * PI * i / TABLE_SIZE);

  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

}

void loop() {

  freqC = random(220, 4400);
  phaseIncC = (freqC * TABLE_SIZE) / SAMPLE_RATE;

  freqM = random(50, 440);
  phaseIncM = (freqM * TABLE_SIZE) / SAMPLE_RATE;

  int tempo = 60000 / BPM;
  delay(tempo / 4);

}