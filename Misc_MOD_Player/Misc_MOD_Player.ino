// Amiga module player //

#include "CurieTimerOne.h"
#include "hxcmod.h"
#include "fairlight_quake.h"

#define AUDIO 5
#define SAMPLE_RATE 44100

volatile  uint16_t buff[1] ;

modcontext mcontext;
void* tune = (void*) &song;

int time;
const int oneSecInUsec = 1000000;

void player(){

  hxcmod_fillbuffer(&mcontext,(msample*) &buff, 1, 0);
  
  analogWrite(AUDIO, buff[0]);

  CurieTimerOne.restart(time);
  
}

void setup(){

  pinMode(AUDIO, OUTPUT);
  analogWriteResolution(16);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);
  
  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &player);

  hxcmod_init(&mcontext);
  hxcmod_setcfg(&mcontext, SAMPLE_RATE/2, 0, 0);
  hxcmod_load(&mcontext,tune, sizeof(song));
  
}

void loop() {

}