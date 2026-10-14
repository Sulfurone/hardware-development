




#include "Time_32.h"

void Time32_Init(uint32_t timer,long time32_count,void (*intHandler)(void))
{
	Timer32_initModule(timer,TIMER32_PRESCALER_1,TIMER32_32BIT,TIMER32_PERIODIC_MODE);
	Timer32_setCount(timer,24*time32_count);//设置1US进一次中断
	if(timer == TIMER32_0_BASE)
	{
		Interrupt_registerInterrupt(INT_T32_INT1,intHandler);//设置中断函数入口
		Interrupt_enableInterrupt(INT_T32_INT1);//打开TIME32的中断
	}
	if(timer == TIMER32_1_BASE)
	{
		Interrupt_registerInterrupt(INT_T32_INT2,intHandler);//设置中断函数入口
		Interrupt_enableInterrupt(INT_T32_INT2);//打开TIME32的中断
	}
	Interrupt_enableMaster();//打开总中断
	
	Timer32_startTimer(timer,0);//TIME32开始计时
}
	

