#ifndef _GENERIC_H
#define _GENERIC_H
#include "stm32f10x.h" 

#define PC13(x) GPIO_WriteBit(GPIOC,GPIO_Pin_13,(BitAction)x)

void Delay_100ns(uint32_t n);
void Delay_us(uint32_t us);
void Delay_ms(uint32_t ms);
void Delay_s(uint32_t s);

void Pin_init(GPIO_TypeDef* GPIOx,uint32_t PINx,GPIOMode_TypeDef GPIO_MODE);
void PIN_invert(uint8_t Pin,uint32_t Time);
void PIN_blink(uint8_t Pin,uint32_t Time,uint8_t times);

#endif 
