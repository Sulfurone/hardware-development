#ifndef __KEY_H
#define __KEY_H
#include "stm32f0xx.h"   
#include "stdbool.h"

#define KEY_VI_FLAG			0x01
#define KEY_OUT_FLAG		0x02
#define KEY_RANGE_FLAG		0x03

#define VI_KEY				(GPIOA->IDR & GPIO_Pin_11)
#define OUT_KEY				(GPIOA->IDR & GPIO_Pin_12)

#define A_EC11				(GPIOA->IDR & GPIO_Pin_5)
#define B_EC11				(GPIOA->IDR & GPIO_Pin_6)
#define D_EC11				(GPIOB->IDR & GPIO_Pin_0)

/* ·äÃùÆ÷ */
#define BUZZER_SOUND_TIME	30	//ms
#define BUZZER_ON			GPIOA->BSRR=GPIO_Pin_4
#define BUZZER_OFF			GPIOA->BRR=GPIO_Pin_4
#define BUZZER_TOGGLE		GPIOA->ODR^=GPIO_Pin_4
#define READ_BUZZER_STA		(GPIOA->ODR & GPIO_Pin_4)

typedef struct  
{
	bool trigDis;
	bool active;
	uint8_t set_vi;
	float step;
	float set_v;
	float set_i;
}_uEncoder_Type;
extern _uEncoder_Type	ec11;

uint8_t EC11Code_Scan(void);
uint8_t Key_Scan(void);
void Key_Config(void);
void Read_CalibKey(void);

#endif
















