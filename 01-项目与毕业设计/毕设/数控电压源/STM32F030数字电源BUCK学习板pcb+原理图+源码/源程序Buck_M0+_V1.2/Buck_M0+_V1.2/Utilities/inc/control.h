#ifndef __CONTROL_H
#define __CONTROL_H

#include "stm32f0xx.h"
#include "pwm.h"

#define CV_MODE						0x00	//恒压模式
#define CC_MODE						0x01	//恒流模式

#define PWM_FRE						PWM_FREQUENCY	//Hz.开关频率

#define RESET_FAULT_TIME 			15000u	//允许故障停止时间
#define PW_VOLTAGV_IN_MAX			56.5f	//V.最大输入电压	
#define PW_VOLTAGV_IN_MIN			11.0f	//V.最小输入电压	
#define PW_VOLTAGV_OUT_MAX			50.5f	//V.最大输出电压	
#define PW_CURRENT_IN_MAX			8.50f	//A.最大输入电流
#define PW_CURRENT_OUT_MAX			8.50f	//A.最大输出电流

#define VOLTAGE_CFRE				(0.2*PWM_FRE)	//Hz.PID调节频率,为开关频率的0.05~0.2倍比较合适
#define CURRENT_CFRE				(0.2*PWM_FRE)	//Hz.PID调节频率,为开关频率的0.05~0.2倍比较合适

typedef struct  
{
	uint32_t recoveryCnt; //故障计数
	uint32_t faultTime;   //允许故障时间
	float controlPWM;  //pwm增量
	uint8_t controlMode;  //输出模式
	uint8_t iovp;         //输入过压保护
	uint8_t iuvp;         //输入欠压保护
	uint8_t iocp;         //输入过流保护
	uint8_t oocp;         //输出过流保护
	uint8_t oovp;         //输出过压保护
	uint8_t outputState;
}_uDeviceFault_Type;
extern _uDeviceFault_Type	device;


void PID_Config(void);
void ProtectReset(void);

#endif















