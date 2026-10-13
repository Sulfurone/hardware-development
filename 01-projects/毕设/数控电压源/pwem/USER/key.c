#include "stm32f10x.h"                  // Device header
#include "lk.h"
#include "sys.h"
#include "delay.h"
#include "key.h"

extern	float target_voltage ; // 目标电压，可根据需要修改
extern int OUTPUT,OUTPUT1,OUTPUT2;


void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 ;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


}


void Key_Get(void)
{ if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0) // 按键1加
    {
        delay_ms(20);
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0)
        {
            target_voltage += 1.0f;
            if (target_voltage > 20.0f) target_voltage = 0.0f; // 上限
           while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0); // 等待松手
        }
    }
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0) // 按键2减
    {
        delay_ms(20);
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0)
        {
            target_voltage -= 1.0f;
            if (target_voltage < 0.0f) target_voltage = 0.0f; // 下限
            while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0); // 等待松手
        }
    }

}
             



