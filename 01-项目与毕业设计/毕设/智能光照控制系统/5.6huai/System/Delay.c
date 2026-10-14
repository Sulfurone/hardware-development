#include "stm32f10x.h"

// 简单的延时函数，不使用SysTick
void Delay_Simple(uint32_t time)
{
	volatile uint32_t i;
	for(i = 0; i < time; i++)
	{
		// 空循环，延时时间取决于时钟频率和编译器优化
	}
}

/**
  * @brief  微秒级延时，简化版
  * @param  xus 延时时长，范围：0~1000
  * @retval 无
  */
void Delay_us(uint32_t xus)
{
	// 在8MHz下，大约对应8个循环/us
	Delay_Simple(xus * 8);
}

/**
  * @brief  毫秒级延时，简化版
  * @param  xms 延时时长，范围：0~1000
  * @retval 无
  */
void Delay_ms(uint32_t xms)
{
	// 在8MHz下，大约对应8000个循环/ms
	Delay_Simple(xms * 8000);
}
 
/**
  * @brief  秒级延时
  * @param  xs 延时时长，范围：0~10
  * @retval 无
  */
void Delay_s(uint32_t xs)
{
	uint32_t i;
	for(i = 0; i < xs; i++)
	{
		Delay_ms(1000);
	}
} 
