#ifndef __PWM_H
#define __PWM_H
#include "stm32f0xx.h"   

#define PWM_Limit_Max(a,b)		a=(a>b)?(b):(a+0)
#define PWM_Limit_Min(a,b)		a=(a<b)?(b):(a+0)

//PWM
#define PWM_COUNT_FRE			48000000		
#define PWM_FREQUENCY			100000	
#define PWM_PERIOD_VAL			(PWM_COUNT_FRE / PWM_FREQUENCY - 1)
#define PWM_DEAD_TIME			(0x00)  //2*64nS

#define PWM_PULSE				TIM1->CCR1

typedef struct  
{
	uint16_t halfPeriod;
	uint16_t period;
	int16_t limitMin;
	int16_t limitMax;
	uint8_t stop;
}_uPWM_Type;
extern _uPWM_Type	pwm;


void PWM_Config(void);
uint8_t PWM_Update(const float duty);
void PWM_Stop(void);
void PWM_Start(void);


#endif
















