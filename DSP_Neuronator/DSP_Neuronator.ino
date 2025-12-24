// Delay neuron resonator //

#include "CurieTimerOne.h"

#define AUDIO       5
#define SAMPLE_RATE 44100
#define BPM         120
#define N_NEURONS   6
#define MAX_DELAY   4

#define LEAK  0.01f
#define GAIN  1.08f
#define BURST 0.01f

float states[N_NEURONS];
float delay_buffer[N_NEURONS][MAX_DELAY];
int delay_idx = 0;

float weights[N_NEURONS][N_NEURONS];
int delays[N_NEURONS][N_NEURONS];

int time;
const int oneSecInUsec = 1000000;

float randFloat() { return ((float)rand() / RAND_MAX) * 2.0f - 1.0f; }

void random_resonant_weights() {
  float w[N_NEURONS][N_NEURONS];
  float max_val = 0.0f;

  for(int i=0;i<N_NEURONS;i++){
    for(int j=0;j<N_NEURONS;j++){
      w[i][j] = randFloat();
      float a = fabs(w[i][j]);
      if(a > max_val) max_val = a;
    }
  }

  for(int i=0;i<N_NEURONS;i++){
    for(int j=0;j<N_NEURONS;j++){
      weights[i][j] = (w[i][j] / max_val) * GAIN;

      delays[i][j] = 1 + (int)(fabs(randFloat()) * (MAX_DELAY - 1));
      if(delays[i][j] < 1) delays[i][j] = 1;
      if(delays[i][j] >= MAX_DELAY) delays[i][j] = MAX_DELAY - 1;
    }
  }
}


void sample() {

  float x[N_NEURONS] = {0};

  for(int i=0;i<N_NEURONS;i++){
    for(int j=0;j<N_NEURONS;j++){
      int d = delays[i][j];
      int idx = (delay_idx - d + MAX_DELAY) % MAX_DELAY;
      x[i] += weights[i][j] * delay_buffer[j][idx];
    }
  }

  float val = 0.0f;
  for(int i=0;i<N_NEURONS;i++){
    states[i] = LEAK * states[i] + tanhf(x[i]);
    delay_buffer[i][delay_idx] = states[i];
    val += states[i];
  }

  val /= N_NEURONS;

  uint16_t out = 32767.0f * (1.0f + val);
  analogWrite(AUDIO, out);

  delay_idx = (delay_idx + 1) % MAX_DELAY;

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

  random_resonant_weights();

  for(int k = 1; k < MAX_DELAY; k++) {
    int d = (delay_idx + k) % MAX_DELAY;
    delay_buffer[0][d] += randFloat() * BURST;
  }

  int tempo = 60000 / BPM;
  delay(tempo / 4);

}