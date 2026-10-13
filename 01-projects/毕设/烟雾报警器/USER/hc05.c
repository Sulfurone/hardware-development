#include "stm32f10x.h"                  // Device header
#include "uart.h"

 uint8_t RxSTA;

void HC05_Init()
{
	USART2_init();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
		
}

void HC05_EnterAT()
{
	GPIO_SetBits(GPIOA, GPIO_Pin_0);
}	

void HC05_ExitAT()
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_0);
}

void HC05_SendString(char *Buf)
{
	printf_plus(Buf);
}

void HC05_GetData(char *Buf)
{
	uint32_t count = 0, a = 0;
	while (count < 10000)
	{
		if (Get1Flag() == 1)
		{
			Buf[a] =GetRxData();
			a ++;
			count = 0;
			RxSTA = 0;
		}
		count ++;
	}

}

