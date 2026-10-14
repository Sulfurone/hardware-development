#include "body_hw.h"



void BODY_HW_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		
		RCC_APB2PeriphClockCmd (BODY_HW_GPIO_CLK, ENABLE );	// 启用人体传感器DO 的单片机引脚端口时钟
		GPIO_InitStructure.GPIO_Pin = BODY_HW_GPIO_PIN;			// 配置人体传感器DO 的单片机引脚模式
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;			// 配置为上拉输入
		
		GPIO_Init(BODY_HW_GPIO_PORT, &GPIO_InitStructure);				// 初始化 
	
}

uint16_t BODY_HW_GetData(void)
{
	uint16_t tempData;
	uint8_t i;
	uint8_t count = 0;
	
	// 进行多次采样，实现简单的去抖动
	for(i = 0; i < 5; i++)
	{
		if(GPIO_ReadInputDataBit(BODY_HW_GPIO_PORT, BODY_HW_GPIO_PIN) == 0)
		{
			count++;  // 计算低电平次数
		}
		delay_ms(2);  // 短暂延时
	}
	
	// 如果大部分时间为低电平，则认为检测到人
	if(count >= 3)
	{
		tempData = 0;  // 返回低电平，表示检测到人
	}
	else
	{
		tempData = 1;  // 返回高电平，表示未检测到人
	}
	
	return tempData;
}



