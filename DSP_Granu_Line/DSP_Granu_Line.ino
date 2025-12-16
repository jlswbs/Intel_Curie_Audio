// Linear grain space resonator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BUFFER_SIZE 2048
#define DECAY       0.996f

int time;
const int oneSecInUsec = 1000000;

float buffer[BUFFER_SIZE];
int idx = 0;

uint32_t rng_state = 2463534242;

inline uint32_t fastRand() {

  rng_state ^= rng_state << 13;
  rng_state ^= rng_state >> 17;
  rng_state ^= rng_state << 5;
  return rng_state;

}

inline float fastRandFloat() { return (fastRand() & 0x7FFFFFFF) / 2147483647.0f; }

void sample() {

  float out = buffer[idx];

  int next = (idx + 1) % BUFFER_SIZE;
  float avg = DECAY * 0.5f * (buffer[idx] + buffer[next]);
  buffer[idx] = avg;

  if (fastRandFloat() < 0.0001f) {
    int burst_len = 1 + (fastRand() % 25);
    int pos = fastRand() % (BUFFER_SIZE - burst_len);
    for (int i = 0; i < burst_len; i++) {
      float noise = ((fastRand() & 0xFFFF) / 32768.0f - 1.0f) * 0.5f;
      buffer[(pos + i) % BUFFER_SIZE] += noise;
    }
  }

  uint16_t out16 = (uint16_t)((out + 1.0f) * 32767.0f);
  analogWrite(AUDIO, out16);

  idx++;
  if (idx >= BUFFER_SIZE) idx = 0;

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

}