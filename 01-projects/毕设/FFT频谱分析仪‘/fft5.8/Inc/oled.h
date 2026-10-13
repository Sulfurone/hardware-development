#ifndef __OLED_H
#define __OLED_H			  	 
#include "stm32f1xx_hal.h"  

//////////////////////////////////////////////////////////////////////////////////	 
//imodule	
//////////////////////////////////////////////////////////////////////////////////	

//OLED模式设置
//0:4线串行模式
//1:并行8080模式
#define OLED_MODE 0

#define DATE_GPIO 	GPIOB
//#define OLED_GPIO_CLK	RCC_APB2Periph_GPIOB
#define OLED_GPIO_CLK	B

#define OLED_SCLK	GPIO_PIN_7
#define OLED_SDIN	GPIO_PIN_6
#define OLED_RST	GPIO_PIN_5
#define OLED_RS		GPIO_PIN_4

//-----------------OLED端口定义----------------  					   
#define OLED_SCLK_Clr() HAL_GPIO_WritePin(DATE_GPIO,OLED_SCLK,GPIO_PIN_RESET)
#define OLED_SCLK_Set() HAL_GPIO_WritePin(DATE_GPIO,OLED_SCLK,GPIO_PIN_SET)

#define OLED_SDIN_Clr() HAL_GPIO_WritePin(DATE_GPIO,OLED_SDIN,GPIO_PIN_RESET)
#define OLED_SDIN_Set() HAL_GPIO_WritePin(DATE_GPIO,OLED_SDIN,GPIO_PIN_SET)

#define OLED_RST_Clr() HAL_GPIO_WritePin(DATE_GPIO,OLED_RST,GPIO_PIN_RESET)
#define OLED_RST_Set() HAL_GPIO_WritePin(DATE_GPIO,OLED_RST,GPIO_PIN_SET)

#define OLED_RS_Clr() HAL_GPIO_WritePin(DATE_GPIO,OLED_RS,GPIO_PIN_RESET)
#define OLED_RS_Set() HAL_GPIO_WritePin(DATE_GPIO,OLED_RS,GPIO_PIN_SET)

//PC0~7,作为数据线
//#define DATAOUT(x) GPIO_Write(GPIOC,x);//输出  
//#define DATAOUT(x) GPIO_Write(GPIOC,x);//输出 
//使用4线串行接口时使用 

 		     
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据
//OLED控制用函数
void OLED_WR_Byte(uint8_t dat,uint8_t cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);		   
							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
void OLED_ShowString(uint8_t x,uint8_t y,const uint8_t *p);	 
#endif  
	 



