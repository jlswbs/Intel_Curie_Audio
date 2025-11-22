// ZX Spectrum Phaser1 1-bit music engine + reverb //

/*

  http://shiru.untergrund.net/files/zx/phaser1.zip
  https://github.com/ESPboy-edu/ESPboy_phaser1

*/

#include "CurieTimerOne.h"
#include "music_data.h"
#include "drum_sample_data.h"

#define AUDIO       5
#define SAMPLE_RATE 23972 // original Phaser1 runs at 23972 but uses sample interleaving
#define MUSIC_FRAME 1024  // now many samples in one tempo unit 2048
#define COMPILE_ADR 40000 // address for compiled Beepola module, default is 40000
#define BUFF_SIZE   2048

  unsigned int buff_pos = 0;
  float buff[BUFF_SIZE];

  int time;
  const int oneSecInUsec = 1000000;

float echo_verb(float sample, float decay) {

  float old_sample = buff[buff_pos];
  float new_sample = (sample + (old_sample * decay)) / 2.0f;
  buff[buff_pos] = new_sample;
  buff_pos = buff_pos + 1;
  if (buff_pos == BUFF_SIZE) buff_pos = 0; 
  return new_sample;

}

#define NOTE_C(octave)  ((274334351/SAMPLE_RATE)>>(6-octave))
#define NOTEhC(octave)  ((290646917/SAMPLE_RATE)>>(6-octave))
#define NOTE_D(octave)  ((307929415/SAMPLE_RATE)>>(6-octave))
#define NOTEhD(octave)  ((326240174/SAMPLE_RATE)>>(6-octave))
#define NOTE_E(octave)  ((345639485/SAMPLE_RATE)>>(6-octave))
#define NOTE_F(octave)  ((366192230/SAMPLE_RATE)>>(6-octave))
#define NOTEhF(octave)  ((387967221/SAMPLE_RATE)>>(6-octave))
#define NOTE_G(octave)  ((411037204/SAMPLE_RATE)>>(6-octave))
#define NOTEhG(octave)  ((435478855/SAMPLE_RATE)>>(6-octave))
#define NOTE_A(octave)  ((461373440/SAMPLE_RATE)>>(6-octave))
#define NOTEhA(octave)  ((488808120/SAMPLE_RATE)>>(6-octave))
#define NOTE_B(octave)  ((517873991/SAMPLE_RATE)>>(6-octave))

const PROGMEM uint16_t note_table[5 * 12] = {
  NOTE_C(1), NOTEhC(1), NOTE_D(1), NOTEhD(1), NOTE_E(1), NOTE_F(1), NOTEhF(1), NOTE_G(1), NOTEhG(1), NOTE_A(1), NOTEhA(1), NOTE_B(1),
  NOTE_C(2), NOTEhC(2), NOTE_D(2), NOTEhD(2), NOTE_E(2), NOTE_F(2), NOTEhF(2), NOTE_G(2), NOTEhG(2), NOTE_A(2), NOTEhA(2), NOTE_B(2),
  NOTE_C(3), NOTEhC(3), NOTE_D(3), NOTEhD(3), NOTE_E(3), NOTE_F(3), NOTEhF(3), NOTE_G(3), NOTEhG(3), NOTE_A(3), NOTEhA(3), NOTE_B(3),
  NOTE_C(4), NOTEhC(4), NOTE_D(4), NOTEhD(4), NOTE_E(4), NOTE_F(4), NOTEhF(4), NOTE_G(4), NOTEhG(4), NOTE_A(4), NOTEhA(4), NOTE_B(4),
  NOTE_C(5), NOTEhC(5), NOTE_D(5), NOTEhD(5), NOTE_E(5), NOTE_F(5), NOTEhF(5), NOTE_G(5), NOTEhG(5), NOTE_A(5), NOTEhA(5), NOTE_B(5)
};

uint32_t channel_active;
uint32_t channel_1a_acc;
uint32_t channel_1a_add;
uint32_t channel_1b_acc;
uint32_t channel_1b_add;
uint32_t channel_2_acc;
uint32_t channel_2_add;
uint32_t channel_1_out;
uint32_t channel_2_out;
uint32_t output_state;
uint32_t drum_ptr;
uint32_t drum_sample;
volatile uint32_t parser_sync;
uint32_t order_list_off;
uint32_t order_pos;
uint32_t order_loop;
uint32_t order_length;
uint32_t pattern_ptr;
uint32_t instrument_ptr;

void sound_init(void){

  order_pos   = 0;
  order_loop  = music_data[0]; //Beepola stores order loop and length <<1
  order_length = music_data[1];
  order_list_off = 4 + music_data[2] + (music_data[3] << 8);
  instrument_ptr = 4;
  output_state = 0;
  parser_sync = 0;

  song_new_pattern();

  channel_active = 0;
  channel_1a_acc = 0;
  channel_1b_acc = 0;
  channel_1a_add = 0;
  channel_1b_add = 0;
  channel_2_acc = 0;
  channel_2_add = 0;
  channel_1_out = 0;
  channel_2_out = 0;
  drum_ptr = 0;
  drum_sample = 0;

}

void song_new_pattern(void){

  pattern_ptr = (music_data[order_list_off + order_pos] + (music_data[order_list_off + order_pos + 1] << 8)) - COMPILE_ADR;
  order_pos += 2;
  if (order_pos >= order_length) order_pos = order_loop;

}

uint8_t song_data_read(void){

  return music_data[pattern_ptr++];

}

void setup(){

  pinMode(AUDIO, OUTPUT);

  analogWriteResolution(16);
  analogWriteFrequency(AUDIO, SAMPLE_RATE);
  
  time = oneSecInUsec / SAMPLE_RATE;
  CurieTimerOne.start(time, &sample);

  sound_init();

}


void sample(){

  int16_t sample;
  if (output_state) sample = 32767;
  else sample = -32767;

  analogWrite(AUDIO, 32768 + echo_verb(sample, 0.3f));

  if (parser_sync) --parser_sync;
    if (!drum_sample){
      if (channel_active){
        channel_1a_acc += channel_1a_add;
      if (channel_1a_acc >= 0x10000){
        channel_1a_acc -= 0x10000;
        channel_1_out ^= 1;
      }
      channel_1b_acc += channel_1b_add;
      if (channel_1b_acc >= 0x10000){
        channel_1b_acc -= 0x10000;
        channel_1_out ^= 1;
      }
      output_state = channel_1_out;
    } else {
      channel_2_acc += channel_2_add;
    if (channel_2_acc >= 0x10000){
      channel_2_acc -= 0x10000;
      channel_2_out ^= 1;
    }
    output_state = channel_2_out;
  }
  channel_active ^= 1;
  } else {
    if (drum_sample_data[drum_ptr >> 1]&drum_sample) output_state = 1; else output_state = 0;
    ++drum_ptr;
    if (drum_ptr >= 1024 * 2) drum_sample = 0;
  }
    
  CurieTimerOne.restart(time);
  
}


void loop(){

  uint16_t tag, channel;
  uint32_t add, off;
  bool done;

  while (1){
    channel = 0;
    done = false;
    while (!done){
      tag = song_data_read();
      if (!tag){
        song_new_pattern();
        continue;
      }
      if (tag & 0x80){
        switch (tag & 0x3f){
          case 60:
            if (!channel){
              channel_1a_add = 0;
              channel_1b_add = 0;
              channel_1_out = 0;
              channel = 1;
            }
            else{
              channel_2_add = 0;
              channel_2_out = 0;
            }
            break;
          case 61:
            instrument_ptr = 4 + (song_data_read() << 1);
            break;
          case 62:
            channel = 1;
            break;
          case 63:
            break;
          default:
          
            add = note_table[tag & 0x3f];
            if (!channel){
              off = music_data[instrument_ptr + 1] + (music_data[instrument_ptr + 2] << 8);
              channel_1a_add = add;
              channel_1b_add = (add << music_data[instrument_ptr + 0]) + off;
              if (tag & 0x40){
                channel_1a_acc = 0;
                channel_1b_acc = music_data[instrument_ptr + 3] << 8;
                channel_1_out = 0;
              }
              channel = 1;
            }
            else{
              channel_2_acc = 0;
              channel_2_add = add;
              channel_2_out = 0;
            }
        }
      }
      else{
        if (tag < 118){
          parser_sync = MUSIC_FRAME * tag;
        }
        else{
          drum_ptr = 0;
          drum_sample = 1 << (tag - 118);
          parser_sync = MUSIC_FRAME * 1;
        }
        done = true;
      }
    }
    while (parser_sync > 0) delay(0);
  }
  
}