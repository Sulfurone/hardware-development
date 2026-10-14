#ifndef __OLED_DATA_H
#define __OLED_DATA_H

#include <stdint.h>

#define OLED_CHN_CHAR_WIDTH		2//Code type UTF-8 for 3,GB2312 for 2

typedef struct 
{
	char Index[OLED_CHN_CHAR_WIDTH + 1];
	uint8_t Data[32];
} ChineseCell_t;
//ASCII

extern const uint8_t OLED_F8x16[][16];
extern const uint8_t OLED_F6x8[][6];
//chinese
extern const ChineseCell_t OLED_CF16x16[];
//picture Data array[] if you want to add new picture ,you have to declare hear
extern const uint8_t Diode[];
extern const uint8_t block[];


#endif
