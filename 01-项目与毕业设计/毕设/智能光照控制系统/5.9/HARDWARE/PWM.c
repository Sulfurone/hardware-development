#include "stm32f10x.h"                  // Device header

void PWM_Init(void)
{
	// 使能GPIOB和TIM1时钟以及AFIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	// 配置GPIO - PB13用作TIM1的互补通道1 (TIM1_CH1N)
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // 复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;       // PB13
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// 配置TIM1使用内部时钟
	TIM_InternalClockConfig(TIM1);
	
	// 配置TIM1基本参数
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;    // ARR: 0-99
	TIM_TimeBaseInitStructure.TIM_Prescaler = 36 - 1;  // PSC: 72MHz/36=2MHz, 2MHz/100=20kHz PWM频率
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);
	
	// 配置TIM1通道1为PWM模式（包括互补输出）
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         // PWM模式1
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // 输出极性高
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	
	// 启用互补输出
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	
	TIM_OCInitStructure.TIM_Pulse = 0;                        // 初始CCR值为0
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	
	// 设置死区时间为0
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
	TIM_BDTRStructInit(&TIM_BDTRInitStructure);
	TIM_BDTRInitStructure.TIM_DeadTime = 0;
	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
	TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);
	
	// 对于TIM1和TIM8，需要额外使能PWM输出
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	
	// 使能TIM1
	TIM_Cmd(TIM1, ENABLE);
}

// 设置PWM占空比 (0-99)
void PWM_SetCompare(uint16_t Compare)
{
	// 确保Compare在有效范围内
	if(Compare > 99) Compare = 99;
	TIM_SetCompare1(TIM1, Compare);
}
