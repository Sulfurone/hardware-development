#include "stm32f10x.h"
#include "Delay.h"

static uint8_t  fac_us = 0; // us延时倍乘数
static uint16_t fac_ms = 0; // ms延时倍乘数

void Delay_Init(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK); // 72MHz
	fac_us = SystemCoreClock / 1000000; // 1us对应的计数值
	fac_ms = (uint16_t)fac_us * 1000;   // 1ms对应的计数值
}

/**
  * @brief  微秒级延时
  * @param  xus 延时时长，范围：0~233015
  * @retval 无
  */
void Delay_us(uint32_t xus)
{
	uint32_t temp;
	SysTick->LOAD = xus * fac_us; // 设置定时器重装值
	SysTick->VAL = 0x00;          // 清空当前计数值
	SysTick->CTRL = 0x01;         // 使能定时器
	do
	{
		temp = SysTick->CTRL;
	} while ((temp & 0x01) && !(temp & (1 << 16))); // 等待时间到达
	SysTick->CTRL = 0x00; // 关闭定时器
	SysTick->VAL = 0x00;  // 清空计数值
}

/**
  * @brief  毫秒级延时
  * @param  xms 延时时长，范围：0~4294967295
  * @retval 无
  */
void Delay_ms(uint32_t xms)
{
	while (xms--)
	{
		Delay_us(1000);
	}
}
 
/**
  * @brief  秒级延时
  * @param  xs 延时时长，范围：0~4294967295
  * @retval 无
  */
void Delay_s(uint32_t xs)
{
	while (xs--)
	{
		Delay_ms(1000);
	}
} 
