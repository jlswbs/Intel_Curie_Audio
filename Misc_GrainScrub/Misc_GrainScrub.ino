// Grain scrub sample oscillator with feedback //

#include "CurieTimerOne.h"
#include "wavetable.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define TABLE_SIZE  22050
#define FB_SIZE     2048
#define BPM         120

float fb_buff[FB_SIZE];
int fbWrite = 0;
float feedback = 0.9f;

float phaseInc = 0.0f;
float grainPos = 0.0f;
float grainSize = 0.0f;
float grainPhase = 0.0f;
float grainPosTarget = 0.0f;
float glideCoeff = 0.001f; 

int time;
const int oneSecInUsec = 1000000;

void sample() {

  grainPos += (grainPosTarget - grainPos) * glideCoeff;

  float readPos = grainPos + grainPhase;

  int i0 = (int)readPos;
  int i1 = i0 + 1;

  i0 &= (TABLE_SIZE - 1);
  i1 &= (TABLE_SIZE - 1);

  float frac = readPos - (int)readPos;
  float s0 = wavetable[i0];
  float s1 = wavetable[i1];
  float val = s0 + frac * (s1 - s0);

  float fb_out = val + fb_buff[fbWrite] * feedback;
  fb_buff[fbWrite] = fb_out;
  fbWrite = (fbWrite + 1) & (FB_SIZE - 1);

  uint16_t out = 32767.0f * (1.0f + (0.2f * fb_out));
  analogWrite(AUDIO, out);

  grainPhase += phaseInc;
  if (grainPhase >= grainSize) grainPhase = 0;

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

  float freq = random(1, 60); 
  phaseInc = (freq * TABLE_SIZE) / SAMPLE_RATE;

  grainPosTarget = random(0, TABLE_SIZE);
  grainSize = random(1, 8192);

  int tempo = 60000 / BPM;
  delay(tempo);

}