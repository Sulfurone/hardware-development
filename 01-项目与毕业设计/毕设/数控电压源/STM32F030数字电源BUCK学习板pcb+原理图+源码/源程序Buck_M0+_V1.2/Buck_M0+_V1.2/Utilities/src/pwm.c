#include "pwm.h"
#include "stdbool.h"


_uPWM_Type			pwm;


/**
  * @brief  TIM configuration
  * @param  None
  * @retval None
  */
void PWM_Config(void)
{
	uint16_t PrescalerValue = 0;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	//TIM_BDTRInitTypeDef	TIM_BDTRInitStruct;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* GPIOA Clocks enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	/* GPIOA Configuration: Channel 1 and Channel 1N as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_2);

	/* TIM1 Configuration */
	/* Compute the prescaler value */
	PrescalerValue = (uint16_t) (48000000 / PWM_COUNT_FRE) - 1;

	/* TIM1 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);

	/* Time Base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = PWM_PERIOD_VAL;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(TIM1, DISABLE);

	/* Channel 1 Configuration in PWM mode */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	
	TIM_OCInitStructure.TIM_Pulse = PWM_PERIOD_VAL/2;
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);
	
	/* 死区时间和刹车配置 */	
	/*
	TIM_BDTRInitStruct.TIM_OSSRState = TIM_OSSRState_Disable; 
	TIM_BDTRInitStruct.TIM_OSSIState = TIM_OSSIState_Disable;
	TIM_BDTRInitStruct.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
	TIM_BDTRInitStruct.TIM_DeadTime = PWM_DEAD_TIME;
	TIM_BDTRInitStruct.TIM_Break = TIM_Break_Disable;
	TIM_BDTRInitStruct.TIM_BreakPolarity = TIM_BreakPolarity_Low;
	TIM_BDTRInitStruct.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
	TIM_BDTRConfig(TIM1,&TIM_BDTRInitStruct);
	*/
	TIM_ARRPreloadConfig(TIM1,ENABLE);
	/* TIM1 counter enable */
	TIM_Cmd(TIM1, ENABLE);
	
	PWM_Stop();

	/* TIM1 Main Output Enable */
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	
	//获取PWM中值和极限值
	pwm.period	=  PWM_PERIOD_VAL;
	pwm.halfPeriod = pwm.period >> 1;
	pwm.limitMax = 0.95f * pwm.period;
	pwm.limitMin = 0.02f * pwm.period;
	pwm.stop = 1;
	
}

/**
  * @brief  pwm out
  * @param  None
  * @retval None
  */
uint8_t PWM_Update(const float duty)
{
	int16_t _duty=0;
	
	if(pwm.stop!=0)
	{
		return 1;
	}
	
	if(duty<0)
	{
		_duty = 0;
	}
	else
	{
		_duty = (int16_t)duty;
	}
	
	PWM_Limit_Max(_duty, pwm.limitMax);
	PWM_Limit_Min(_duty, pwm.limitMin);
	PWM_PULSE =  (uint16_t)_duty;
	
	return 0;
	
}

/**
  * @brief  pwm close
  * @param  None
  * @retval None
  */
void PWM_Stop(void)
{
	pwm.stop = 1;
	PWM_PULSE = 0;
	TIM_OC1PolarityConfig(TIM1, TIM_OCPolarity_Low);
}

/**
  * @brief  pwm close
  * @param  None
  * @retval None
  */
void PWM_Start(void)
{
	TIM_OC1PolarityConfig(TIM1, TIM_OCPolarity_High);
	pwm.stop = 0;
}

































