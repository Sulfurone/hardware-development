#include "HCSR04.h"

#include "main.h"



const Timer_A_UpModeConfig upConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_3,          // SMCLK/1 = 3MHz
        50000,                           // 5000 tick period
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,    // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
};

void HCSR04Init(void)
{

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P9, GPIO_PIN2);//tring
	MAP_GPIO_setAsInputPin(GPIO_PORT_P5, GPIO_PIN3); 
	
    MAP_GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P5, GPIO_PIN3);
    
	MAP_GPIO_disableInterrupt(GPIO_PORT_P9, GPIO_PIN2);
    MAP_GPIO_disableInterrupt(GPIO_PORT_P5, GPIO_PIN3);

    MAP_Timer_A_configureUpMode(TIMER_A2_BASE, &upConfig);

    MAP_Interrupt_enableSleepOnIsrExit();
    MAP_Interrupt_enableInterrupt(INT_TA1_0);
    MAP_Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE);
    
    MAP_Interrupt_enableMaster();  

}
void TA1_0_IRQHandler(void)
{

    MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0);
//	Timer_A_getCounterValue(uint32_t timer);
	
}

float Distance(void)
{
	int count=0,i=0,sum=0;
	float distance=0;
	while(i!=5)
	{
	  GPIO_setOutputLowOnPin(GPIO_PORT_P9, GPIO_PIN2);//ÉÏÀ­
		GPIO_setOutputHighOnPin(GPIO_PORT_P9, GPIO_PIN2);//ÉÏÀ­
		delay_us(20);
		GPIO_setOutputLowOnPin(GPIO_PORT_P9, GPIO_PIN2);//ÉÏÀ­
		while(GPIO_getInputPinValue(GPIO_PORT_P5,GPIO_PIN3) == 0 );	//
		TIMER_A_CMSIS(TIMER_A2_BASE)->R = 0;
		while(GPIO_getInputPinValue(GPIO_PORT_P5,GPIO_PIN3) == 1); //
		count=TIMER_A_CMSIS(TIMER_A2_BASE)->R;
		//v = 340m/s = 34000cm/s = 34000cm/10^6us = 0.034cm/us
		//s = vt/2 = t*0.034/2 = t*0.017 ˜ t/58	
		distance=((float)count / 58);	
		i++;
		sum=sum+distance;
	}
		return sum/5;
}
