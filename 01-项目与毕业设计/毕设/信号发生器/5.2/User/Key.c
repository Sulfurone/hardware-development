#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_12 | GPIO_Pin_9;  // 拨码开关
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_8;  // 按键
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

uint8_t Key_GetNum(void)
{
	uint8_t KeyNum = 0;
	
	// 检测拨码开关
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_13) == 0)
	{
		KeyNum = 1;  // 拨码开关1
	}
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12) == 0)
	{
		KeyNum = 2;  // 拨码开关2
	}
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9) == 0)
	{
		KeyNum = 3;  // 拨码开关3
	}
	
	// 检测按键
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0)
	{
		Delay_ms(20);  // 消抖
		while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0);  // 等待按键释放
		Delay_ms(20);  // 消抖
		KeyNum = 4;  // 按键1
	}
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 0)
	{
		Delay_ms(20);  // 消抖
		while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 0);  // 等待按键释放
		Delay_ms(20);  // 消抖
		KeyNum = 5;  // 按键2
	}
	
	return KeyNum;
}
