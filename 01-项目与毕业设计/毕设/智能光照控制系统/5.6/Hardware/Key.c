#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "KEY.h"

/**
  * @brief  初始化按键IO
  * @param  无
  * @retval 无
  */
void KEY_Init(void)
{
	// 使能按键端口时钟
	RCC_APB2PeriphClockCmd(KEY_PORT_CLK, ENABLE);
	
	// 配置按键IO为输入上拉模式
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
	GPIO_InitStructure.GPIO_Pin = KEY_MODE_PIN | KEY_BRIGHTNESS_UP_PIN | KEY_BRIGHTNESS_DOWN_PIN | KEY_POWER_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY_PORT, &GPIO_InitStructure);
}

/**
  * @brief  扫描按键状态
  * @param  无
  * @retval 按键值: KEY_MODE, KEY_BRIGHT_UP, KEY_BRIGHT_DOWN, KEY_POWER, KEY_NONE
  */
uint8_t KEY_Scan(void)
{
	static uint8_t key_up = 1;  // 按键松开标志
	
	if (key_up && (GPIO_ReadInputDataBit(KEY_PORT, KEY_MODE_PIN) == 0 || 
				  GPIO_ReadInputDataBit(KEY_PORT, KEY_BRIGHTNESS_UP_PIN) == 0 || 
				  GPIO_ReadInputDataBit(KEY_PORT, KEY_BRIGHTNESS_DOWN_PIN) == 0 || 
				  GPIO_ReadInputDataBit(KEY_PORT, KEY_POWER_PIN) == 0))
	{
		Delay_ms(10);  // 消抖
		
		// 二次检查按键状态，防止抖动
		if (GPIO_ReadInputDataBit(KEY_PORT, KEY_MODE_PIN) == 0 || 
			GPIO_ReadInputDataBit(KEY_PORT, KEY_BRIGHTNESS_UP_PIN) == 0 || 
			GPIO_ReadInputDataBit(KEY_PORT, KEY_BRIGHTNESS_DOWN_PIN) == 0 || 
			GPIO_ReadInputDataBit(KEY_PORT, KEY_POWER_PIN) == 0)
		{
			key_up = 0;
			
			if (GPIO_ReadInputDataBit(KEY_PORT, KEY_MODE_PIN) == 0)
				return KEY_MODE;
			else if (GPIO_ReadInputDataBit(KEY_PORT, KEY_BRIGHTNESS_UP_PIN) == 0)
				return KEY_BRIGHT_UP;
			else if (GPIO_ReadInputDataBit(KEY_PORT, KEY_BRIGHTNESS_DOWN_PIN) == 0)
				return KEY_BRIGHT_DOWN;
			else if (GPIO_ReadInputDataBit(KEY_PORT, KEY_POWER_PIN) == 0)
				return KEY_POWER;
		}
	}
	else if (GPIO_ReadInputDataBit(KEY_PORT, KEY_MODE_PIN) == 1 && 
			 GPIO_ReadInputDataBit(KEY_PORT, KEY_BRIGHTNESS_UP_PIN) == 1 && 
			 GPIO_ReadInputDataBit(KEY_PORT, KEY_BRIGHTNESS_DOWN_PIN) == 1 && 
			 GPIO_ReadInputDataBit(KEY_PORT, KEY_POWER_PIN) == 1)
	{
		key_up = 1;  // 所有按键松开
	}
	
	return KEY_NONE;  // 无按键按下
}
