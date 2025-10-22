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

void InitZeroPos();
void SendMotorCommand(uint8_t *pData, uint16_t size);
void SetMotorPos(uint8_t ID, uint8_t addr, short pos, short vel);
void move();
void xgo_control();
void EnableMotor(uint8_t ID, uint8_t mode);
void EnableAllMotor(int mode);
void xgo_rx();
void WriteZeroPos();
bool ReadZeroPos();
void set_action_loop_flag(uint8_t flag);

extern uint8_t Action_ID;
extern uint8_t actionLoop_FLAG;
extern Motor motor[MOTOR_NUM];
extern float vx;
extern float vyaw;
extern uint16_t motor_speed;
extern int calibrate_mode;
#endif
