#ifndef OLED_SSD1306_H
#define OLED_SSD1306_H

#include "stm32f10x.h"

void OLED_Init(void);
void OLED_Clear(void);
void OLED_Text(uint8_t x, uint8_t page, const char *text);

#endif
