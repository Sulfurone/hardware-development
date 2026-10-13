#ifndef __OLED_H
#define __OLED_H

#include "myiic.h"

#define OLED_ADDRESS	0x78
#define OLED_CMD		0x00
#define OLED_DATA		0x40

#define X_VO			44
#define Y_VO			3
#define X_IO			44
#define Y_IO			6
#define X_UNIT			100

uint32_t OLED_Pow(uint8_t m, uint8_t n);
void OLED_WriteByte(uint8_t res, uint8_t cmd);
void OLED_Set_Pos(uint8_t x, uint8_t y);
void OLED_DisplayOn(void);
void OLED_DisplayOff(void);
void OLED_ClearAll(void);
void OLED_Clear(uint8_t lineX);

void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size);
void OLED_ShowNum(uint8_t x, uint8_t y, uint16_t num, const uint8_t positive);
void OLED_ShowString(uint8_t x, uint8_t y, const uint8_t *chr, uint8_t len, uint8_t size);
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t pos);
void OLED_Config(void);

void OLED_DefaultMode(void);
void OLED_ShowVI(float v, float i);


#endif


