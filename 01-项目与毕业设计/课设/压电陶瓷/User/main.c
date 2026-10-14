#include "stm32f10x.h"

/*
 * Passive two-wire piezo driver.
 *
 * PA4             -> TB6612FNG STBY
 * PA5             -> TB6612FNG PWMA
 * PA6 / TIM3_CH1  -> TB6612FNG AIN1
 * PA7 / TIM3_CH2  -> TB6612FNG AIN2
 *
 * TIM3_CH1 and TIM3_CH2 are complementary square waves. They must feed the
 * logic inputs of a TB6612FNG H-bridge. The piezo element connects only to
 * AO1/AO2, never directly to a GPIO pin.
 */

#define PIEZO_FREQUENCY_HZ  2000U

static void TB6612_Init(void)
{
	GPIO_InitTypeDef gpio;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* Keep the bridge disabled until timer signals are configured. */
	gpio.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &gpio);
	GPIO_ResetBits(GPIOA, GPIO_Pin_4 | GPIO_Pin_5);
}

static void TB6612_SetEnabled(FunctionalState state)
{
	if (state != DISABLE)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_4 | GPIO_Pin_5);
	}
	else
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_4 | GPIO_Pin_5);
	}
}

static uint32_t TIM3_GetClockHz(void)
{
	RCC_ClocksTypeDef clocks;
	uint32_t timerClock;

	RCC_GetClocksFreq(&clocks);
	timerClock = clocks.PCLK1_Frequency;

	/* When APB1 is prescaled, STM32F1 timer clock is 2 x PCLK1. */
	if ((RCC->CFGR & RCC_CFGR_PPRE1) != RCC_CFGR_PPRE1_DIV1)
	{
		timerClock *= 2U;
	}

	return timerClock;
}

static void Piezo_HBridgePWM_Init(uint32_t frequencyHz)
{
	GPIO_InitTypeDef gpio;
	TIM_TimeBaseInitTypeDef timeBase;
	TIM_OCInitTypeDef outputCompare;
	uint32_t timerClock;
	uint16_t prescaler;
	uint16_t period;
	uint16_t halfPeriod;

	/* A 1 MHz timer clock limits the 16-bit period to 16 Hz minimum. */
	if ((frequencyHz < 16U) || (frequencyHz > 1000000U))
	{
		frequencyHz = PIEZO_FREQUENCY_HZ;
	}

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* PA6 = TIM3_CH1 and PA7 = TIM3_CH2. */
	gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &gpio);

	/* Use a 1 MHz TIM3 counter clock for direct frequency calculation. */
	timerClock = TIM3_GetClockHz();
	prescaler = (uint16_t)((timerClock / 1000000U) - 1U);
	period = (uint16_t)((1000000U / frequencyHz) - 1U);
	halfPeriod = (uint16_t)((period + 1U) / 2U);

	TIM_TimeBaseStructInit(&timeBase);
	timeBase.TIM_Prescaler = prescaler;
	timeBase.TIM_CounterMode = TIM_CounterMode_Up;
	timeBase.TIM_Period = period;
	timeBase.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM3, &timeBase);

	/* CH1 is high during the first half-period. */
	TIM_OCStructInit(&outputCompare);
	outputCompare.TIM_OCMode = TIM_OCMode_PWM1;
	outputCompare.TIM_OutputState = TIM_OutputState_Enable;
	outputCompare.TIM_Pulse = halfPeriod;
	outputCompare.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM3, &outputCompare);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

	/* PWM2 with the same compare value is the complement of CH1. */
	outputCompare.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OC2Init(TIM3, &outputCompare);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
}

int main(void)
{
	TB6612_Init();
	Piezo_HBridgePWM_Init(PIEZO_FREQUENCY_HZ);
	TB6612_SetEnabled(ENABLE);

	while (1)
	{
		/* The timer continuously drives the H-bridge inputs. */
	}
}
