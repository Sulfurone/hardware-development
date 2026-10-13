#include "led.h"  


/**
  * @brief  led configuration
  * @param  None
  * @retval None
  */
void LED_Config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;

	/* GPIOB Periph clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	
	/* led */
	GPIO_SetBits(GPIOB, GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}


