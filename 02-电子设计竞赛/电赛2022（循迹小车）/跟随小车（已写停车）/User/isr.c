#include "driverlib.h"
int num =0;

extern uint16_t       Pulse_Count;     //速度脉冲计数;
float Speed_Value;
void T32_INT1_IRQnHandler(void)
{
				Timer32_clearInterruptFlag(TIMER32_0_BASE);
        Speed_Value=(((float)Pulse_Count/390)*3.1416*6.5)*50;  //每20ms刷新一次速度   cm/s
        Pulse_Count=0;
				//Timer32_setCount(TIMER32_0_BASE,240000);//设置计数个数


}
void Time_A_IRQnHandler(void)
{
	//Timer32_clearInterruptFlag(TIMER32_0_BASE);
	Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0);
	GPIO_toggleOutputOnPin(GPIO_PORT_P2,GPIO_PIN0);
	
	//Timer32_setCount(TIMER32_0_BASE,240000);//设置计数个数


}

