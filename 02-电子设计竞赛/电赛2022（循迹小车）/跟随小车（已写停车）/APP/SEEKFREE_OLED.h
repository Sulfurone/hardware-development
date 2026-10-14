/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2016,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：179029047
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		SEEKFREE_OLED.h
 * @brief      		0.96OLED函数库
 * @company	   		成都逐飞科技有限公司
 * @author     		Go For It(1325536866)
 * @version    		v1.0
 * @Software 		IAR 7.7 or MDK 5.17
 * @Target core		MK60DN512VLL10
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2016-02-25
 * @note	
					NRF24L01接线定义：
					------------------------------------ 
					    OLED液晶    单片机                        
    					D0          A15
    					D1          A16            
    					RES         A14    
    					DC          B16
					------------------------------------ 
 ********************************************************************************************************************/



#ifndef _SEEKFREE_OELD_H
#define _SEEKFREE_OELD_H

#include "gpio.h"
typedef unsigned short  uint16;
typedef unsigned char  uint8;
typedef  short         int16;


extern const uint8 chinese_test[10][16];

#define  OLED_SCL_PORT   GPIO_PORT_P3                          
#define  OLED_SCL_PIN    GPIO_PIN5                         

#define OLED_SDA_PORT   GPIO_PORT_P5                          
#define OLED_SDA_PIN    GPIO_PIN2                       

#define  OLED_RST_PORT   GPIO_PORT_P3                          
#define  OLED_RST_PIN    GPIO_PIN7                         

#define OLED_DC_PORT   GPIO_PORT_P3                           
#define OLED_DC_PIN    GPIO_PIN6                       



void Dly_ms(uint16 ms);
void OLED_Init(void);
void OLED_Fill(uint8 dat);
void OLED_WrDat(uint8 data);
void OLED_Wr6Dat(uint8 data);
void OLED_PutPixel(uint8 x,uint8 y);
void OLED_ClrPixel(uint8 x,uint8 y);
void OLED_Set_Pos(uint8 x, uint8 y);
void OLED_P6x8Str(uint8 x,uint8 y,uint8 ch[]);
void OLED_P8x16Str(uint8 x,uint8 y,uint8 ch[]);
void OLED_HEXACSII(uint16 hex,uint8* Print);
void OLED_Print_Num(uint8 x, uint8 y, uint16 num);
void OLED_Print_Num1(uint8 x, uint8 y, int16 num);
void dis_bmp(uint16 high, uint16 width, uint8 *p,uint8 value);
void xu_display_float(uint8 x, uint8 y, float num1,uint8 digit);
void gpio_set(uint_fast8_t selectedPort,uint_fast16_t selectedPins,uint8 x);
void oled_print_chinese(uint8 x, uint8 y, uint8 size, const uint8 *p, uint8 len);
void xu_display_signednum(uint8 x, uint8 y, long num);
/********************************************************************/

#endif
