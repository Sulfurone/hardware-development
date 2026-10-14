#ifndef _HW_CONFIG_H
#define _HW_CONFIG_H

#include"stm32f10x.h"

/*º¯ÊýÉùÃ÷*/
void USART1_Configuration(void);
void USART1_SendString(uint8_t *ch);
void NVIC_Configuration_TIM(void);
void TIMER_Configuration(void);

#endif/*_HW_CONFIG_H*/

