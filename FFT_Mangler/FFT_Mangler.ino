// Glitch mangler base a FFT/IFFT synthesis //

#define FIX32_FFT_IMPLEMENTATION 

#define SAMPLE_RATE 44100
#define AUDIO 5
#define FFT   256
#define SIZE  2048
#define BPM   120

#include "fix32_fft.h"
#include "table.h"

int32_t vReal[FFT];
int32_t vImag[FFT];
uint16_t table_ptr = 0;
unsigned long trig = 0;
uint16_t pitch = 0;
uint16_t glitch = 1;
bool inverse = true;
uint16_t mstime = 60000 / BPM / 2;


void setup() {

  pinMode(AUDIO, OUTPUT);
  analogWriteResolution(16);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);

}

void loop() {

 if (millis() - trig >= mstime) {

    table_ptr = 0;
    pitch = random(1, 150);
    glitch = random(0, 8);
    trig = millis();

  }

  for (int i = 0; i < FFT; i++) {

    uint16_t idx = (table_ptr + i) % SIZE;
    vReal[i] = (int32_t)wavetable_real[idx] << 12; 
    vImag[i] = (int32_t)wavetable_imag[idx] << 12;

  }

  table_ptr += glitch;
  if (table_ptr >= SIZE) table_ptr = SIZE;

  fix32_fft(vReal, vImag, 8, inverse);

  for (int i = 0; i < FFT; i++) {

    int32_t out = 32768 + (vReal[i] >> 4);
    analogWrite(AUDIO, (uint16_t)out);
    delayMicroseconds(pitch);

  }

}