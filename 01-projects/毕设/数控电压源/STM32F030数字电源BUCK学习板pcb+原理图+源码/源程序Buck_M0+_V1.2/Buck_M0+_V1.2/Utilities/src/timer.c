#include "timer.h"



/**
  * @brief  Tim3 configuration
  * @param  None
  * @retval None
  */
void TIM3_Config(void)
{
	uint16_t u_period=0;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	/* TIM3 Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);

	u_period = TIM3_PERIOD;
	TIM3_CHECK_PERIOD(u_period);
	
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = u_period;
	TIM_TimeBaseStructure.TIM_Prescaler = TIM3_PRESCALER;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	/* TIM3 IRQ configuration */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);
}


/**
  * @brief  Tim3 configuration
  * @param  None
  * @retval None
  */
void TIM14_Config(void)
{
	uint16_t u_period=0;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	/* TIM14 Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14 , ENABLE);
	
	u_period = TIM14_PERIOD;
	TIM14_CHECK_PERIOD(u_period);

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = u_period;
	TIM_TimeBaseStructure.TIM_Prescaler = TIM14_PRESCALER;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);
	
	/* TIM14 IRQ configuration */
	NVIC_InitStructure.NVIC_IRQChannel = TIM14_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE);
	
	/* TIM14 enable counter */
	TIM_Cmd(TIM14, ENABLE);
}















