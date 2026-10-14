#ifndef __Time_A_PWM_H__
#define __Time_A_PWM_H__
#include "timer_a.h"
#include "interrupt.h"
#include "gpio.h"
#include "cs.h"

#define Timer_A_PWM_MAX_DUTY      1000    

typedef enum
{
	TA0 ,
	TA1,
	TA2,
	TA3		
} TA_n;
typedef enum
{
	TA_ch0 ,
	TA_ch1,
	TA_ch2,
	TA_ch3,
	TA_ch4,
	TA_ch5
} TA_chn;

/******************
TA0.1(TA0模块的TA_ch1通道)----->  P2.4(对应的输出引脚)
TA0.2										 ----->  P2.5
TA0.3										 ----->  P2.6
TA0.4										 ----->  P2.7

TA1.1										 ----->  P7.7
TA1.2										 ----->  P7.6
TA1.3										 ----->  P7.5
TA1.4										 ----->  P7.4


TA2.1										 ----->  P5.6
TA2.2										 ----->  P5.7

TA3.1										 ----->  P10.5
TA3.2										 ----->  P8.2
TA3.3										 ----->  P9.2
TA3.4										 ----->  P9.3

******************/
void Time_A_Pwm_Init(TA_n  ta_n, TA_chn  ta_chn, unsigned long pwm_freq, unsigned short pwm_duty);

void Time_A_Pwm_Duty(TA_n  ta_n, TA_chn  ta_chn, unsigned short pwm_duty);



#endif

