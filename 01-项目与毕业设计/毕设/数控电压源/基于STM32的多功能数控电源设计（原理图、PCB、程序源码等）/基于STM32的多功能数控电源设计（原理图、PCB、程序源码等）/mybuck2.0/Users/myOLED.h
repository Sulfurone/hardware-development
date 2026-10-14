/************************************************************************************
*  Copyright (c), 2014, HelTec Automatic Technology co.,LTD.
*            All rights reserved.
*
* Http:    www.heltec.cn
* Email:   cn.heltec@gmail.com
* WebShop: heltec.taobao.com
*
* File name: OLED.h
* Project  : HelTec.uvprij
* Processor: STM32F103C8T6
* Compiler : MDK fo ARM
* 
* Author : 小林
* Version: 1.00
* Date   : 2014.2.20
* Email  : hello14blog@gmail.com
* Modification: none
* 
* Description:128*64点阵的OLED显示屏驱动文件，仅适用于惠特自动化(heltec.taobao.com)的SD1306驱动SPI通信方式显示屏
*
* Others: none;
*
* Function List:
*
* 2. void OLED_WrDat(unsigned char dat) -- 向OLED写数据
* 3. void OLED_WrCmd(unsigned char cmd) -- 向OLED写命令
* 4. void OLED_SetPos(unsigned char x, unsigned char y) -- 设置起始点坐标
* 5. void OLED_Fill(unsigned char bmp_dat) -- 全屏填充(0x00可以用于清屏，0xff可以用于全屏点亮)
* 6. void OLED_CLS(void) -- 清屏
* 7. void OLED_Init(void) -- OLED显示屏初始化
* 8. void OLED_6x8Str(unsigned char x, y,unsigned char ch[]) -- 显示6x8的ASCII字符
* 9. void OLED_8x16Str(unsigned char x, y,unsigned char ch[]) -- 显示8x16的ASCII字符
* 10.void OLED_16x16CN(unsigned char x, y, N) -- 显示16x16中文汉字,汉字要先在取模软件中取模
* 11.void OLED_BMP(unsigned char x0, y0,x1, y1,unsigned char BMP[]) -- 全屏显示128*64的BMP图片
*
* History: none;
*
*************************************************************************************/

#ifndef __MYOLED_H
#define __MYOLED_H
//////////////////////////////////////////////////////////////////////////////////	 
//imodule	
//////////////////////////////////////////////////////////////////////////////////	

#define XLevelL			0x00
#define XLevelH			0x10
#define XLevel	    ((XLevelH&0x0F)*16+XLevelL)
#define Max_Column	128
#define Max_Row			64
#define	Brightness	0xCF 
#define X_WIDTH 		128
#define Y_WIDTH 		64

//-----------------OLED端口定义----------------
#define OLED_SCL_Clr() 			(HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET))	//SCLK D0
#define OLED_SCL_Set() 			(HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET))

#define OLED_SDA_Clr() 			(HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET))	//SDIN D1
#define OLED_SDA_Set() 			(HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET))

#define OLED_RST_Clr()				(HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET))	//RST(RES)
#define OLED_RST_Set()				(HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET))

#define OLED_DC_Clr()				(HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET))	//DC
#define OLED_DC_Set()				(HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET))

#define OLED_CS_Clr()				(HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET))	//CS
#define OLED_CS_Set()				(HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET))

//#define OLED_CMD  0	//写命令
//#define OLED_DATA 1	//写数据
#include "mybsp.h"


//OLED控制用函数
void OLED_WrDat(unsigned char dat);//写数据
void OLED_WrCmd(unsigned char cmd);//写命令
void OLED_SetPos(unsigned char x, unsigned char y);//设置起始点坐标
void OLED_Fill(unsigned char bmp_dat);//全屏填充
void OLED_CLS(void);//清屏
void OLED_Init(void);//初始化
void OLED_6x8Str(unsigned char x, unsigned char y, unsigned char ch[]);
void OLED_8x16Str(unsigned char x, unsigned char y, unsigned char ch[]);
void OLED_16x16CN(unsigned char x, unsigned char y, unsigned char N);
void OLED_BMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[]);

#endif
