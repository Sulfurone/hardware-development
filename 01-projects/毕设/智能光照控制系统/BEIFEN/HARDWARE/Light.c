#include "stm32f10x.h"                  // Device header
#include "PWM.h"
int keyflag=0;

void LIGHT(uint16_t Speed)
{
	// 确保速度值在有效范围内
	if(Speed > 99) Speed = 99;
	if(keyflag==1)
	{PWM_SetCompare(Speed);}
	
}
