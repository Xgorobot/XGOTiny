#ifndef __XGO_H
#define __XGO_H
#include <driver/uart.h>
#include <driver/gpio.h>

#define FLASH_ZERO_POS_ADDR 0xFFF000
#define MOTOR_NUM 5
#define PI 3.14159

typedef struct 
{
	uint8_t ID;
    short DesPos;
    float DesSpd;
	short DesTor;
    short FbPos;
    float FbSpd;
    short FbTor;
	short ZeroPos;
	uint8_t Load;
} Motor;

//Zero Position Functions
void InitZeroPos();
void WriteZeroPos();
bool ReadZeroPos();
//Motor Control Functions
void EnableMotor(uint8_t ID, uint8_t mode);
void EnableAllMotor(int mode);
void SetMotorPos(uint8_t ID, uint8_t addr, short pos, short vel);
void SendMotorCommand(uint8_t *pData, uint16_t size);
//Movement & Control Functions
void move();
void xgo_control();
void xgo_rx();
//Action & Behavior Functions
void set_action_loop_flag(uint8_t flag);

extern float vx;
extern float vyaw;
extern uint16_t motor_speed;
extern int calibrate_mode;
extern uint8_t Action_ID;
extern uint8_t actionLoop_FLAG;
extern Motor motor[MOTOR_NUM];
extern float angle1;
extern float angle2;
extern float angle3;
extern float angle4;
extern float angle5;
extern int control_mode;
#endif
