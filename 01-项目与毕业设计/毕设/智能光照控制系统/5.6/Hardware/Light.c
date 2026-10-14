#include "stm32f10x.h"                  // Device header
#include "PWM.h"

void Motor_Init(void)
{
	// 直接调用PWM初始化函数，PWM.c中已经包含了所有必要的初始化
	PWM_Init();
}

void Motor_SetSpeed(uint16_t Speed)
{
	// 确保速度值在有效范围内
	if(Speed > 99) Speed = 99;
	PWM_SetCompare(Speed);
}
