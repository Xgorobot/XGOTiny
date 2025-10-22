#ifndef XGO_ACTION_H
#define XGO_ACTION_H
#include "xgo.h"

#define TS 100
#define ACTION_NUMBER 0x0F 
#define Wave_ID       0x01 
#define Naughty_ID    0x02 
#define Lookup_ID     0x03 
#define Swing_ID      0x04 
#define Rolling_ID    0x05 
#define Angry_ID      0x06 
#define Swimming_ID   0x07 
#define Pee_ID        0x08 
#define Stretch_ID    0x09 
#define Bouncing_ID   0x0A 
#define Shaking_ID    0x0B 
#define Sit_ID        0x0C 
#define Scratch_ID    0x0D 
#define Hug_ID        0x0E 
#define reset_ID 255


void xgo_action();
void normal_state();
void Updated_Counter();
void Clear_State(uint8_t mode);


void Wave();
void Naughty();
void Swing();
void Lookup();
void Rolling();
void Angry();
void Swimming();
void Pee();
void Stretch();
void Bouncing();
void Shaking();
void Sit();
void Scratch();
void Hug();

#endif