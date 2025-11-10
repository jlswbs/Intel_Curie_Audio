// Floating point Granular noise synth with delay //

#include "CurieTimerOne.h"

#define AUDIO 5
#define SAMPLERATE 44100
#define BUFFER_SIZE 512
#define DELAY_BUFFER_SIZE 2048

float audioBuffer[BUFFER_SIZE];
float delayBuffer[DELAY_BUFFER_SIZE];
volatile float grainPosition = 0.0f;
volatile float grainLength = 32.0f;
volatile float grainSpeed = 1.0f;
volatile float outputSample = 0.0f;
volatile uint16_t delayIndex = 0;

volatile float feedbackGain = 0.5f;

int time;
const int oneSecInUsec = 1000000;

float randomf(float minVal, float maxVal) {
    return minVal + (maxVal - minVal) * ((float)rand() / (float)RAND_MAX);
}

void sample() {

    grainPosition += grainSpeed;

    if (grainPosition >= grainLength) 
        grainPosition = randomf(0.0f, (float)(BUFFER_SIZE) - grainLength);

    int index = (int)grainPosition;
    outputSample = audioBuffer[index];

    float delayedSample = delayBuffer[delayIndex];
    float mixedSample = -outputSample * (1.0f - feedbackGain) + delayedSample * feedbackGain;
    delayBuffer[delayIndex] = mixedSample;
    delayIndex = (delayIndex + 1) % DELAY_BUFFER_SIZE;

    outputSample = 0.5f * (outputSample + delayedSample);

    analogWrite(AUDIO, (uint16_t)((outputSample+1.0f) * 32767.0f));

    CurieTimerOne.restart(time);

}

void setup() {

    pinMode(AUDIO, OUTPUT);
    analogWriteResolution(16);
    analogWriteFrequency(AUDIO, SAMPLERATE);

    time = oneSecInUsec / SAMPLERATE;
    CurieTimerOne.start(time, &sample);

    for (int i = 0; i < BUFFER_SIZE; i++) {
        float t = (float)i / BUFFER_SIZE * 2.0f * PI;
        audioBuffer[i] = sinf(t);
    }

}

void loop() {

    grainSpeed = randomf(1.0f, 16.0f);
    grainLength = randomf(16.0f, BUFFER_SIZE - 1.0f);
    feedbackGain = randomf(0.0f, 0.99f);

    delay(random(1, 100));

}