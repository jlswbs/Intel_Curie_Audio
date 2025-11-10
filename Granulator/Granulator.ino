// Floating point Granulator engine //

#include "CurieTimerOne.h"

#define AUDIO 5
#define SAMPLERATE 44100

#define BUFFER_SIZE       512
#define DELAY_BUFFER_SIZE 2048
#define CROSSFADE_SIZE    32

#define SPEED_MIN         1.0f
#define SPEED_MAX         16.0f
#define SPEED_PERIOD      1000

#define LENGTH_MIN        16.0f
#define LENGTH_MAX        BUFFER_SIZE
#define LENGTH_PERIOD     500

#define FEEDBACK_MIN      0.0f
#define FEEDBACK_MAX      1.0f
#define FEEDBACK_PERIOD   800

#define GRAIN_REVERSE     true

float audioBuffer[BUFFER_SIZE];
float delayBuffer[DELAY_BUFFER_SIZE];
volatile float grainPosition = 0.0f;
volatile float grainLength = 32.0f;
volatile float grainSpeed = 1.0f;
volatile float outputSample = 0.0f;
volatile uint16_t delayIndex = 0;
volatile float feedbackGain = 0.5f;

float lastGrainSample = 0.0f;
float nextGrainSample = 0.0f;
bool crossfadeActive = false;
int crossfadeCounter = 0;

float targetGrainSpeed = 1.0f;
float grainSpeedIncrement = 0.0f;
int speedChangeCounter = 0;

float targetGrainLength = 32.0f;
float grainLengthIncrement = 0.0f;
int lengthChangeCounter = 0;

float targetFeedbackGain = 0.5f;
float feedbackIncrement = 0.0f;
int feedbackCounter = 0;

float randomf(float minVal, float maxVal) {
    return minVal + ((float)rand() / (float)RAND_MAX) * (maxVal - minVal);
}

int time;
const int oneSecInUsec = 1000000;

void sample() {

    if(speedChangeCounter <= 0) {
        targetGrainSpeed = randomf(SPEED_MIN, SPEED_MAX);
        grainSpeedIncrement = (targetGrainSpeed - grainSpeed) / SPEED_PERIOD;
        speedChangeCounter = SPEED_PERIOD;
    }
    grainSpeed += grainSpeedIncrement;
    speedChangeCounter--;

    if(lengthChangeCounter <= 0) {
        targetGrainLength = randomf(LENGTH_MIN, LENGTH_MAX);
        grainLengthIncrement = (targetGrainLength - grainLength) / LENGTH_PERIOD;
        lengthChangeCounter = LENGTH_PERIOD;
    }
    grainLength += grainLengthIncrement;
    lengthChangeCounter--;

    if(feedbackCounter <= 0) {
        targetFeedbackGain = randomf(FEEDBACK_MIN, FEEDBACK_MAX);
        feedbackIncrement = (targetFeedbackGain - feedbackGain) / FEEDBACK_PERIOD;
        feedbackCounter = FEEDBACK_PERIOD;
    }
    feedbackGain += feedbackIncrement;
    feedbackCounter--;

    int grainDirection = GRAIN_REVERSE ? -1 : 1;
    grainPosition += grainDirection * grainSpeed;

    if(grainPosition >= grainLength || grainPosition < 0) {
        grainPosition = grainDirection > 0 ? 0 : grainLength - 1;
        crossfadeActive = true;
        crossfadeCounter = 0;
        lastGrainSample = nextGrainSample;
    }

    uint16_t index = (uint16_t)grainPosition;
    float frac = grainPosition - index;
    float sample1 = audioBuffer[index];
    float sample2 = audioBuffer[(index + 1) % BUFFER_SIZE];
    nextGrainSample = sample1 * (1.0f - frac) + sample2 * frac;

    float finalGrainSample;
    if(crossfadeActive) {
        float alpha = (float)crossfadeCounter / CROSSFADE_SIZE;
        finalGrainSample = lastGrainSample * (1.0f - alpha) + nextGrainSample * alpha;
        crossfadeCounter++;
        if(crossfadeCounter >= CROSSFADE_SIZE) crossfadeActive = false;
    } else {
        finalGrainSample = nextGrainSample;
    }

    float delayedSample = delayBuffer[delayIndex];
    float mixedSample = finalGrainSample * (1.0f - feedbackGain) + delayedSample * feedbackGain;
    delayBuffer[delayIndex] = mixedSample;
    delayIndex = (delayIndex + 1) % DELAY_BUFFER_SIZE;

    float outSample = (finalGrainSample + delayedSample) * 0.5f;
    analogWrite(AUDIO, (uint16_t)((outSample+1.0f) * 32767.0f));

    CurieTimerOne.restart(time);

}

void setup() {

    pinMode(AUDIO, OUTPUT);
    analogWriteResolution(16);
    analogWriteFrequency(AUDIO, SAMPLERATE);

    time = oneSecInUsec / SAMPLERATE;
    CurieTimerOne.start(time, &sample);

    for(int i = 0; i < BUFFER_SIZE; i++) {
        float t = (float)i / BUFFER_SIZE * 2.0f * PI;
        audioBuffer[i] = sinf(t);
    }

}

void loop() {

}