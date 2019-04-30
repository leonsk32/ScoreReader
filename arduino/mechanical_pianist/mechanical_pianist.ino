//一定時間でリニアに振幅減衰する正弦波
//#include "score.h"
#include "jupiter.h"
//#include "when_you_wish_upon_a_star.h"
#include <MozziGuts.h>
#include <Oscil.h>
#include <avr/pgmspace.h>
#include <tables/sin8192_int8.h>
#define CONTROL_RATE 64 // コントロールレートの設定
#define KEY A0
#define VEL A1
#define PART 2//パート数(1 or 2 or 3 or 4)

Oscil <SIN8192_NUM_CELLS, AUDIO_RATE> Sin(SIN8192_DATA);
#if (PART > 1)
Oscil <SIN8192_NUM_CELLS, AUDIO_RATE> b_Sin(SIN8192_DATA);
#endif
#if (PART > 2)
Oscil <SIN8192_NUM_CELLS, AUDIO_RATE> c_Sin(SIN8192_DATA);
#endif
#if (PART > 3)
Oscil <SIN8192_NUM_CELLS, AUDIO_RATE> d_Sin(SIN8192_DATA);
#endif

unsigned char gain = 255;//エンベロープ用のゲイン
unsigned char tempo = 5;//テンポを何倍にするか、120の約数
float freq_amp = 1;//周波数を何倍にするか
int length = tempo*20;//音符長さ、始めにkeyを取得する時間をとるために初期値を設定している
int counter = 0;//経過時間
int i=0;//何番目の記号か
int n_notes = sizeof(score_data) / sizeof(*score_data);//記号数
int key=0;//調決定用センサー値
int vel=0;//テンポ決定用センサー値
int pitch;

#if (PART > 1)
unsigned char b_gain = 255;
int b_length = tempo*20;
int b_counter = 0;
int b_i = 0;
int b_n_notes = sizeof(b_score_data) / sizeof(*b_score_data);
int b_pitch;
int b_flag = 0;//aと同じかどうか
#endif
#if (PART > 2)
unsigned char c_gain = 255;
int c_length = tempo*20;
int c_counter = 0;
int c_i = 0;
int c_n_notes = sizeof(c_score_data) / sizeof(*c_score_data);
int c_pitch;
int c_flag=0;
#endif
#if (PART > 3)
unsigned char d_gain = 255;
int d_length = tempo*20;
int d_counter = 0;
int d_i = 0;
int d_n_notes = sizeof(d_score_data) / sizeof(*d_score_data);
int d_pitch;
int d_flag=0;
#endif

void setup(){
  startMozzi(CONTROL_RATE);// ()になにも入れない場合は初期値 64、この時点でupdateが始まる
  Sin.setFreq(0);
#if (PART > 1)
  b_Sin.setFreq(0);
#endif
#if (PART > 2)
  c_Sin.setFreq(0);
#endif
#if (PART > 3)
  d_Sin.setFreq(0);
#endif
}

void updateControl(){
  key=mozziAnalogRead(KEY);//バックグラウンドでセンサー値取得
  vel=mozziAnalogRead(VEL);
  freq_amp=key/(float)341;//0～3にマッピング
  freq_amp=(float)pow(2,freq_amp);//1～8
  tempo = map(vel,0,1023,1,30);
  switch(tempo){//120の約数に変換
    case 7: case 9: case 11: case 13: case 16: case 21: case 25: tempo--; break;
    case 14: case 19: case 23: case 29: tempo++; break;
    case 17: case 26: tempo-=2; break;
    case 28: case 18: tempo+=2; break;
    case 27: tempo-=3; break;
  }
  
  if(counter >= length){
    counter = 0;
    pitch = pgm_read_word(score_data + i++);
    Sin.setFreq(pitch * freq_amp);
    length = pgm_read_word(score_data + i++) * 120;//(なるべく)tempoの倍数になるように
    if(i==n_notes) i=0;
  }
  counter += tempo;
  gain = map(counter, 0, length, 255, 0);
  
#if (PART > 1)
  if(b_counter >= b_length){
    b_counter = 0;
    b_pitch = pgm_read_word(b_score_data + b_i++);
    if(b_pitch == pitch) b_flag=1;
    else b_flag=0;
    b_Sin.setFreq(b_pitch * freq_amp);
    b_length = pgm_read_word(b_score_data + b_i++) * 120;
    if(b_i == b_n_notes) b_i=0;
  }
  b_counter += tempo;
  b_gain = map(b_counter, 0, b_length, 255, 0);
#endif

#if (PART > 2)
  if(c_counter >= c_length){
    c_counter = 0;
    c_pitch = pgm_read_word(c_score_data + c_i++);
    if(c_pitch == pitch) c_flag=1;
    else c_flag=0;
    c_Sin.setFreq(c_pitch * freq_amp);
    c_length = pgm_read_word(c_score_data + c_i++) * 120;
    if(c_i==c_n_notes) c_i=0;
  }
  c_counter += tempo;
  c_gain = map(c_counter, 0, c_length, 255, 0);
#endif

#if (PART > 3)
  if(d_counter >= d_length){
    d_pitch = pgm_read_word(d_score_data + d_i++);
    if(d_pitch == pitch) d_flag=1;
    else d_flag=0;
    d_Sin.setFreq(d_pitch * freq_amp);
    d_length = pgm_read_word(d_score_data + d_i++) * 120;
    if(d_i == d_n_notes) d_i=0;
  }
  d_counter += tempo;
  d_gain = map(d_counter, 0, d_length, 255, 0);
#endif
}

int updateAudio(){
#if (PART == 1)
  int volume = Sin.next() * gain;
#endif
#if (PART == 2)
  int volume;
  if(b_flag)
    volume = Sin.next() * gain;
  else  
    volume = Sin.next() * gain/2 + b_Sin.next()*b_gain/2;
#endif
#if (PART == 3)
  int volume;
  if(b_flag && c_flag)
    volume = Sin.next() * gain;
  else
    volume = Sin.next() * gain / 3 + b_Sin.next() * b_gain / 3 + c_Sin.next() * c_gain / 3;
#endif
#if (PART == 4)
  int volume;
  if(b_flag && c_flag && d_flag)
    volume = Sin.next() * gain;
  else
    int volume = Sin.next() * d_gain/4 + b_Sin.next() * b_gain/4 + c_Sin.next()*c_gain/4 + d_Sin.next()*d_gain/4;
#endif
  return (volume>>8);
//正弦波aSinに振幅増減値のgainをかける。最大値255をかけるため、出力オーバーにならないよう2^8で割っておく(>>8)
}

void loop(){
    audioHook();
}
