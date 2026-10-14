#ifndef __Time_32_H__
#define __Time_32_H__

#include "timer32.h"
#include "interrupt.h"

#define Time32_Init_us(timer,time32_us,p)   Time32_Init(timer,time32_us,p)//Time32_Init_us(TIMER32_0_BASE,1000,T32_INT1_IRQnHandler)
																													//初始化Time32模块，并设置定时时间(单位为 us)，设置中断函数名为T32_INT1_IRQnHandler
																													
																													
#define Time32_Init_ms(timer,time32_ms,p)   Time32_Init(timer,time32_ms*1000,p)//Time32_Init_us(TIMER32_0_BASE,1000,T32_INT1_IRQnHandler)
																													//初始化Time32模块，并设置定时时间(单位为 ms)，设置中断函数名为T32_INT1_IRQnHandler

void Time32_Init(uint32_t timer,long time32_count,void (*intHandler)(void));//内部调用




#endif

