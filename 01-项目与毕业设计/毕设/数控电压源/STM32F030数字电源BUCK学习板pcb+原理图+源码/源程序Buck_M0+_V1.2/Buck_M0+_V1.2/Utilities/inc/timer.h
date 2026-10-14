#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f0xx.h"

#define SYSTEMP_COLOCK			48000000u	//Hz.系统时钟
#define TIM3_COLOCK				10000		//Hz.定时器时钟
#define TIM3_xTIMER				0.002f		//S.定时时间
#define TIM3_PRESCALER			(uint16_t)(SYSTEMP_COLOCK/TIM3_COLOCK - 1)	//预分频值
#define TIM3_PERIOD				(uint16_t)(TIM3_xTIMER*TIM3_COLOCK)			//计数值
#define TIM3_CHECK_PERIOD(a)	a=(a<1)?(0):(a-1) //防止参数设置错误


#define TIM14_COLOCK			1000000		//Hz.定时器时钟
#define TIM14_xTIMER			0.0001f	//S.定时时间
#define TIM14_PRESCALER			(uint16_t)(SYSTEMP_COLOCK/TIM14_COLOCK - 1)	//预分频值
#define TIM14_PERIOD			(uint16_t)(TIM14_xTIMER*TIM14_COLOCK)		//计数值
#define TIM14_CHECK_PERIOD(a)	a=(a<1)?(0):(a-1) //防止参数设置错误


void TIM3_Config(void);
void TIM14_Config(void);


#endif















