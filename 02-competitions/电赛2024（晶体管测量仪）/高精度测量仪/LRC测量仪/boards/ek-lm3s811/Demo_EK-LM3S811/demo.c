
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/adc.h"
#include "inc/lm3s811.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"

unsigned char TIME=0;
#define New_Edition	

#define Blink_First			0
#define Blink_Second		2
#define Blink_Third			4

#define Blue_Blink_Fast		6
#define Green_Blink_Fast	7
#define Red_Blink_Fast		8

#define Blue_Blink_Slow		15
#define Green_Blink_Slow	20
#define Red_Blink_Slow		25

#define All_LED_Close		80

#define LED_Close_Status_A		100
#define LED_Close_Status_B		200

void delay() //程序延时
{
	int	i=300000;
    while(i>0)	
   	{
		i--;
  	}
}
void O_D2()
{
 	  GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5,GPIO_PIN_5);
}
void C_D2()
{
	  GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5,0);
}
void O_L3()
{	  
#ifdef New_Edition
 	  GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0,~0);
#else
	  GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0,0);
#endif
}
void C_L3()
{
#ifdef New_Edition
	  GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0,~GPIO_PIN_0);
#else
	  GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0,GPIO_PIN_0);
#endif
}
void O_L4()
{
#ifdef New_Edition
 	  GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1,~0);
#else
	  GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1,0);
#endif
}
void C_L4()
{
#ifdef New_Edition
	  GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1,~GPIO_PIN_1);
#else
	  GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1,GPIO_PIN_1);
#endif
}
void O_L5()
{	 
#ifdef New_Edition
	  GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0,~0);
#else
 	  GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0,0);
#endif
}
void C_L5()
{
#ifdef New_Edition
	  GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0,~GPIO_PIN_0);
#else
	  GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0,GPIO_PIN_0);
#endif
}
void O_L6()
{
#ifdef New_Edition
 	  GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1,~0);
#else
	  GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1,0);
#endif
}
void C_L6()
{
#ifdef New_Edition
	  GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1,~GPIO_PIN_1);
#else
	  GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1,GPIO_PIN_1);
#endif
}
void O_L7()
{
#ifdef New_Edition
 	  GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0,~0);
#else
	  GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0,0);
#endif
}
void C_L7()
{
#ifdef New_Edition
	  GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0,~GPIO_PIN_0);
#else
	  GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0,GPIO_PIN_0);
#endif
}
void O_L8()
{
#ifdef New_Edition
 	  GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1,~0);
#else
	  GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1,0);
#endif
}
void C_L8()
{
#ifdef New_Edition
	  GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1,~GPIO_PIN_1);
#else
	  GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1,GPIO_PIN_1);
#endif
}
void OA()
{
	  O_L3();O_L4();O_L5();O_D2();O_L6();O_L7();O_L8();
}
void CA()
{
	  C_L3();C_L4();C_L5();C_D2();C_L6();C_L7();C_L8();
}

void
Timer0IntHandler(void)
{
    //
    // 清除 Timer 中断
    //
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TIME++;
	if(TIME == LED_Close_Status_B)	 // 状态在 LED_Close_Status_A 与 LED_Close_Status_B 之间，LED 始终为灭的状态
		TIME = LED_Close_Status_A;


}
void
SysTickIntHandler(void)
{
	if(GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_4)==0x00)//检测 USER 键是否被按下
		TIME=0;	//将控制 LED 状态的标志归零
}
int main(void)
{
	
	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_6MHZ);//配置系统时钟
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB | SYSCTL_PERIPH_GPIOC | SYSCTL_PERIPH_GPIOD | SYSCTL_PERIPH_GPIOE);//启用 GPIO B、C、D 和 E
	
	SysTickPeriodSet(SysCtlClockGet() / 20);//配置系统最小时间中断
    SysTickIntEnable();//启用最小时间中断

	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);//配置PB0、PB1为输出
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_5 );//配置PC5为输出
	GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_4 );//配置PC4为输入
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1);//配置PD0、PD1为输出
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1);//配置PE0、PE1为输出
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);//启用 Timer0
	
    TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);//配置 Timer0 为 32 位的定时器
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()/2);//配置 Timer0 的时间为 0.5 秒

    //
    // 启用 Timer0/SysTick
    //
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	SysTickEnable();
	TimerEnable(TIMER0_BASE, TIMER_A);
	
	CA();//先关闭所有的 LED 灯

	while(1)
	{
	   //开机闪三下
	   if(TIME == Blink_First)
		{			 	
			OA();
		}
		if(TIME == Blink_First+1)
		{			 	
			CA();
		}
	    if(TIME == Blink_Second)
		{			 	
			OA();
		}
		if(TIME == Blink_Second+1)
		{			 	
			CA();
		}
		if(TIME == Blink_Third)
		{			 	
			OA();
		}
		if(TIME == Blink_Third+1)
		{			 	
			CA();
		}
		//进入对角快闪状态
		if((TIME == Blue_Blink_Fast)||(TIME == Blue_Blink_Fast+3)||(TIME == Blue_Blink_Fast+6))
		{			 	
			O_L3();O_L6();C_L5();C_L8();
		}
		if((TIME == Green_Blink_Fast)||(TIME == Green_Blink_Fast+3)||(TIME == Green_Blink_Fast+6))
		{			 	
			O_L4();O_L7();C_L3();C_L6();
		}
		if((TIME == Red_Blink_Fast)||(TIME == Red_Blink_Fast+3)||(TIME == Red_Blink_Fast+6))
		{			 	
			O_L5();O_L8();C_L4();C_L7();
		}
		//进入对角慢闪状态
		if((TIME == Blue_Blink_Slow)||(TIME == Blue_Blink_Slow+15)||(TIME == Blue_Blink_Slow+30)||(TIME == Blue_Blink_Slow+45))
		{			 	
			O_L3();O_L6();delay();C_L5();C_L8();
		}
		if((TIME == Green_Blink_Slow)||(TIME == Green_Blink_Slow+15)||(TIME == Green_Blink_Slow+30)||(TIME == Green_Blink_Slow+45))
		{			 	
			O_L4();O_L7();delay();C_L3();C_L6();
		}
		if((TIME == Red_Blink_Slow)||(TIME == Red_Blink_Slow+15)||(TIME == Red_Blink_Slow+30)||(TIME == Red_Blink_Slow+45))
		{			 	
			O_L5();O_L8();delay();C_L4();C_L7();
		}
		//超时，未按下 USER 键，则关闭所有的 LED 灯
		if(TIME > All_LED_Close)
		{			 	
			CA();
		}

	};


}
