#include "xgo.h"
#include <math.h>
#include <stdlib.h>
#include <driver/uart.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_flash.h>
#include <esp_heap_caps.h>
#include <esp_system.h>
#include "xgo_action.h"


Motor motor[MOTOR_NUM];
uint16_t zero_buffer[MOTOR_NUM] = {2400,600,2400,600,1500};
uint16_t zero_buffer_default[MOTOR_NUM] = {2400,600,2400,600,1500};
uint16_t motor_speed = 0;
uint8_t Action_ID = 0;
uint8_t actionLoop_FLAG = 0;
uint8_t serial_lock = 0;
float vx = 0.0;
float vyaw = 0.0;
int calibrate_mode = 0;
int init_flag = 0;
float l_p[][5] = {{3*PI/4.0, 3*PI/4.0, 3*PI/4.0, 3*PI/4.0, PI/4.0}, //F
                  {-PI/4.0, -PI/4.0, -PI/4.0, -PI/4.0, PI/4.0}, //B
                  {3*PI/4.0, -PI/4.0,  -PI/4.0, 3*PI/4.0, PI/4.0},
                  {-PI/4.0, 3*PI/4.0,  3*PI/4.0, -PI/4.0, PI/4.0}};


void set_action_loop_flag(uint8_t flag){
    if(flag==1){
        Action_ID = 1;
        actionLoop_FLAG = 1;
    }else{
        Action_ID = 255;
        actionLoop_FLAG = 0;
    }
}
void WriteZeroPos(){
    uint32_t data[MOTOR_NUM];
    for(int i=0;i<MOTOR_NUM;i++){
        data[i] = motor[i].FbPos;
        motor[i].ZeroPos = motor[i].FbPos;
        printf("write zeropos [%d]: %d\r\n", i, motor[i].FbPos);
    }
    esp_err_t err = esp_flash_erase_region(NULL, FLASH_ZERO_POS_ADDR, 4096);
    if (err != ESP_OK) {
        return;
    }
    err = esp_flash_write(NULL, data, FLASH_ZERO_POS_ADDR, sizeof(data));
    if (err != ESP_OK) {
        return;
    }
}

bool ReadZeroPos(){
    uint32_t data[MOTOR_NUM] = {0};    
    esp_err_t err = esp_flash_read(NULL, data, FLASH_ZERO_POS_ADDR, sizeof(data));
    for(int i=0;i<MOTOR_NUM;i++){
        printf("zeropos [%d]: %ld\r\n", i, data[i]);
    }
    if (err != ESP_OK) {
        for(int i=0;i<MOTOR_NUM;i++){
            motor[i].ZeroPos = zero_buffer_default[i];
        }
        return false;
    }
    for(int i=0;i<MOTOR_NUM;i++){  
        if(data[i]<200||data[i]>2800){
            return false;
        }else{
            motor[i].ZeroPos = data[i];
        }
    }
    return true;
}

void InitZeroPos(){
    bool res;
    res = ReadZeroPos();
    for(int i=0;i<MOTOR_NUM;i++){
        motor[i].ID = i+1;
    }
    if(res){
        for(int i=0;i<MOTOR_NUM;i++){
            motor[i].Load = 1;
        }
    }else{
        calibrate_mode = 1;
    }
    init_flag = 1;
}

void SendMotorCommand(uint8_t *pData,uint16_t size)
{
    if(serial_lock){
		return;
	}else{
		serial_lock = 1;
	}
	uart_write_bytes(UART_NUM_2,pData,size);
    uart_wait_tx_done(UART_NUM_2, pdMS_TO_TICKS(50));
	serial_lock = 0;
}

void SetMotorPos(uint8_t ID,uint8_t addr,short pos,short vel){
	uint8_t bBuf[11];
	uint8_t checkSum = 0x00;
	bBuf[0] = 0xff;
	bBuf[1] = 0xff;
	bBuf[2] = ID;
	bBuf[3] = 0x07;
	bBuf[4] = 0x03;
	bBuf[5] = addr;
	bBuf[6] = pos & 0xff;
	bBuf[7] = pos>>8;
	bBuf[8] = vel & 0xff;
	bBuf[9] = vel>>8;
	checkSum = ID + 0x07 +0x03 + addr + bBuf[6] + bBuf[7] + bBuf[8] + bBuf[9];
	bBuf[10] = 0xff - checkSum;
	SendMotorCommand(bBuf, 11);
}

void ReadMotorState(uint8_t ID){
	uint8_t bBuf[8];
	uint8_t CheckSum = 0;
	bBuf[0] = 0xff;
	bBuf[1] = 0xff;
	bBuf[2] = ID;
	bBuf[3] = 0x04;
	bBuf[4] = 0x02;
	bBuf[5] = 0x48;
	bBuf[6] = 0x06;
	CheckSum = ID + 0x04 + 0x02 + 0x48 + 0x06;
	bBuf[7] = ~CheckSum;
	SendMotorCommand(bBuf, 8);
}

void EnableMotor(uint8_t ID, uint8_t mode){
	uint8_t bBuf[8];
    uint8_t CheckSum = 0;
	bBuf[0] = 0xff;
	bBuf[1] = 0xff;
	bBuf[2] = ID;
	bBuf[3] = 0x04;
	bBuf[4] = 0x03;
	bBuf[5] = 0x30;
	bBuf[6] = mode;
	CheckSum = ID + 0x04 + 0x03 + 0x30 + mode;
	bBuf[7] = ~CheckSum;
	SendMotorCommand(bBuf, 8);
}

void EnableAllMotor(int mode){ 
    for(int i=0;i<MOTOR_NUM;i++){
        motor[i].Load = mode;
    }
    vTaskDelay(pdMS_TO_TICKS(200));
    for(int j=0;j<10;j++){
        for(int i=0;i<MOTOR_NUM;i++){
            EnableMotor(i+1, mode);
        }
    }
}

void move(){
    float ratio = 0.0;
    float step = 0.0;
    static float pace_t = 0.0;
    int x_index = 0;
    int yaw_index = 2;
    if(pace_t > 2.0*PI){
        pace_t = 0.0;
    }
    pace_t += 0.2;
    step = sqrt(vx*vx+vyaw*vyaw);
            
    if(vx>0){
        x_index = 0;
    }else{
        x_index = 1;
    }

    if(vyaw>0){
        yaw_index = 3;
    }else{
        yaw_index = 2;
    }
    if(Action_ID==0){
        if(abs(vx)>15 || abs(vyaw)>15){
            ratio = abs(vx)/(abs(vx) + abs(vyaw));
            motor[0].DesPos = motor[0].ZeroPos - 700 + (short)(step*cos(pace_t + ratio*l_p[x_index][0] + (1-ratio)*l_p[yaw_index][0])); 
            motor[1].DesPos = motor[1].ZeroPos + 700 + (short)(step*cos(pace_t + ratio*l_p[x_index][1] + (1-ratio)*l_p[yaw_index][1])); 
            motor[2].DesPos = motor[2].ZeroPos - 700 - (short)(step*cos(pace_t + ratio*l_p[x_index][2] + (1-ratio)*l_p[yaw_index][2]));
            motor[3].DesPos = motor[3].ZeroPos + 700 - (short)(step*cos(pace_t + ratio*l_p[x_index][3] + (1-ratio)*l_p[yaw_index][3]));
            motor[4].DesPos = motor[4].ZeroPos + (short)(step*1.5*cos(pace_t + ratio*l_p[x_index][4] + (1-ratio)*l_p[yaw_index][4]));
        }else{
            motor[0].DesPos = motor[0].ZeroPos - 600; 
            motor[1].DesPos = motor[1].ZeroPos + 600; 
            motor[2].DesPos = motor[2].ZeroPos - 600;
            motor[3].DesPos = motor[3].ZeroPos + 600;
            motor[4].DesPos = motor[4].ZeroPos;
        }
    }else{
        xgo_action();
    }    
} 

uint8_t rxFlag = 0;
uint8_t rxLen = 0;
uint8_t rxDataLen = 0;
uint8_t id = 0;
uint8_t rxBuffer[30] = {0};
void xgo_rx(){
    uint8_t tempBuf[10];
    uint8_t res = 0; 
    uint8_t checkSum = 0;
    uint16_t POS_LOW_Byte = 0;
    uint16_t POS_HIGH_Byte = 0;
    uint16_t VEL_LOW_Byte = 0;
    uint16_t VEL_HIGH_Byte = 0;
    uint16_t TOR_LOW_Byte = 0;
    uint16_t TOR_HIGH_Byte = 0;
    while(uart_read_bytes(UART_NUM_2, tempBuf, 1, 5) > 0){
        res = tempBuf[0];
        switch(rxFlag)
        {
            case 0:
                if(res == 0xFF)
                    {rxFlag = 1;rxBuffer[0] = 0xFF;}
                    break;
            case 1:
                if(res == 0xFF)
                    {rxFlag = 2;rxBuffer[1] = 0xFF;}
                else{
                    rxFlag = 0;
                }
                break;
            case 2:
                    rxBuffer[2] = res;
                    id = res;
                    rxFlag = 3;
                    break;
            case 3:
                if(res == 0x08||res == 0x0B)
                    {					
                        rxFlag = 4; 
                        rxBuffer[3] = res;
                        rxLen = 0;
                        rxDataLen = res;
                        checkSum = 0;
                    }
                else{
                    rxFlag = 0;
                }                    
                break;
            case 4:
                rxBuffer[4+rxLen] = res;
                rxLen++;
                if(rxLen==rxDataLen){
                    for(int i=0; i<1+rxDataLen; i++){
                        checkSum += rxBuffer[2+i];
                    }
                    checkSum = ~checkSum;
                    if(checkSum == rxBuffer[3+rxDataLen]){          
                        POS_LOW_Byte =  rxBuffer[rxDataLen - 1];
                        POS_HIGH_Byte =  rxBuffer[rxDataLen];
                        VEL_LOW_Byte =  rxBuffer[rxDataLen - 3];
                        VEL_HIGH_Byte =  rxBuffer[rxDataLen - 2];
                        TOR_LOW_Byte =  rxBuffer[rxDataLen + 1];
                        TOR_HIGH_Byte =  rxBuffer[rxDataLen + 2];
                        if(id>0&&id<=MOTOR_NUM){
                            id = id - 1;
                            motor[id].FbPos = POS_LOW_Byte | (POS_HIGH_Byte << 8);
                            motor[id].FbSpd = VEL_LOW_Byte | (VEL_HIGH_Byte << 8);
                            motor[id].FbTor = TOR_LOW_Byte | (TOR_HIGH_Byte << 8);
                            id = id + 1;
                        }	 
                    }
                    checkSum = 0;
                    rxFlag = 0;                  			
                }
                break;
            default:
                rxFlag = 0;
                break;
        }		
    }
}


// 检测按钮在1秒内按下三次的函数
void detect_triple_click() {
    static int click_count = 0;           // 点击次数计数器
    static uint32_t first_click_time = 0; // 第一次点击的时间
    static bool button_pressed = false;   // 按钮按下状态标志
    if(calibrate_mode == 0){
        return;
    }
    int level = gpio_get_level(GPIO_NUM_0);
    uint32_t current_time = esp_timer_get_time() / 1000; // 毫秒    

    if (level == 0 && !button_pressed) {
        button_pressed = true;
        
        if (click_count == 0) {
            first_click_time = current_time;
            click_count = 1;
        } else {
            if (current_time - first_click_time <= 1000) {
                click_count++;
                
                if (click_count >= 3) {
                    WriteZeroPos();
                    calibrate_mode = 0;                   
                    click_count = 0;
                    first_click_time = 0;
                    EnableAllMotor(1);
                }
            } else {
                click_count = 1;
                first_click_time = current_time;
            }
        }
    }
    
    if (level == 1 && button_pressed) {
        button_pressed = false;
    }
    
    if (click_count > 0 && (current_time - first_click_time) > 1000) {
        click_count = 0;
        first_click_time = 0;
    }
}


// 自定义舵机控制函数 - 您可以在这里添加自己的舵机指令
void xgo_control() { 
    if(init_flag == 0){
        return;
    }
    static uint32_t counter = 0;
    static uint32_t counter2 = 0;
    static uint8_t read_id = 1;
    int time = esp_timer_get_time()/1000;
    counter++; 
    counter2++;
    move();

    for(int i=0;i<5;i++){
        if(motor[i].Load){    
            SetMotorPos(i+1, 0x35, motor[i].DesPos, motor_speed);            
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    if(counter2%10 == 0){
        detect_triple_click();
    }

    if(counter%10 == 0){
        ReadMotorState(read_id);
        counter = 0;
        read_id++;
        if(read_id > 6){
            read_id = 1;
        }
    }
}



