// Floating point Waveguide synthesis //

#include "CurieTimerOne.h"

#define AUDIO 5
#define SAMPLERATE 44100
#define SIZE 512
#define BPM 120

float leftWave[SIZE];
float rightWave[SIZE];

float lowpass = 0.9f;
float reflectL = -0.95f;
float reflectR = 0.95f;

float fractionalIdx = 0.0f;
float step = 1.0f;

uint16_t N = 128;

int time;
const int oneSecInUsec = 1000000;

float lp1 = 0.0f, lp2 = 0.0f;
inline float lowpass2(float in) {
    lp1 = lp1 + lowpass * (in - lp1);
    lp2 = lp2 + lowpass * (lp1 - lp2);
    return lp2;
}

float readWave(float *buffer, float idx) {
    int i0 = (int)idx;
    int i1 = (i0 + 1) % SIZE;
    float frac = idx - i0;
    return buffer[i0]*(1.0f-frac) + buffer[i1]*frac;
}

void sample() {

    float right = readWave(rightWave, fractionalIdx);
    float left  = readWave(leftWave, fractionalIdx);

    float out = right + left;

    float nextRight = lowpass2(left * reflectL);
    float nextLeft  = lowpass2(right * reflectR);

    int idx = (int)fractionalIdx;
    rightWave[idx] = nextRight;
    leftWave[idx]  = nextLeft;

    fractionalIdx += step;
    if (fractionalIdx >= N) fractionalIdx -= N;

    analogWrite(AUDIO, uint16_t(32767*(out+1.0f)));

    CurieTimerOne.restart(time);

}

enum ExciteType { NOISE, SINE };

void excite(float freq, ExciteType type = NOISE) {

    N = (uint16_t)(SAMPLERATE / freq);
    if (N >= SIZE) N = SIZE - 1;
    if (N < 4) N = 4;

    step = 1.0f;            
    fractionalIdx = 0.0f;    

    for (int i = 0; i < N; i++) {
        float s = 0.0f;
        switch(type) {
            case NOISE:
                s = (float)random(-32768, 32767) / 65536.0f;
                break;
            case SINE:
                s = sinf(2.0f * 3.14159265f * i / N);
                break;
        }
        rightWave[i] = s;
        leftWave[i]  = 0.0f;
    }

}

void setup() {

    pinMode(AUDIO, OUTPUT);
    analogWriteResolution(16);
    analogWriteFrequency(AUDIO, SAMPLERATE);

    time = oneSecInUsec / SAMPLERATE;
    CurieTimerOne.start(time, &sample);

}

void loop() {

    float freqTable[] = {110.0f, 146.8f, 196.0f, 246.9f, 329.6f, 392.0f, 440.0f, 523.3f};
    float f = freqTable[random(0, 8)];

    ExciteType type = (ExciteType)random(0, 2);
    excite(f, type);

    int tempo = 60000 / BPM;
    delay(tempo / 3);

}