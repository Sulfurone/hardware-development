#include "oled.h"
#include "delay.h"
#include "oled_font.h"
#include "stdio.h"

/**
  * @brief  OLED configuration
  * @param  None
  * @retval None
  */
void OLED_WriteByte(uint8_t res, uint8_t cmd)
{
	iicDevStart();
	iicDevSendByte(OLED_ADDRESS);
	iicDevWaitAck();
	iicDevSendByte(cmd);
	iicDevWaitAck();
	iicDevSendByte(res); 
	iicDevWaitAck();
	iicDevStop();
}

/**
  * @brief  OLED configuration
  * @param  None
  * @retval None
  */
void OLED_Config(void)
{	
	myIIC_Config();
	delay_us(100000);
	
	OLED_WriteByte(0xAE, OLED_CMD);//--display off
	OLED_WriteByte(0x00, OLED_CMD);//---set low column address
	OLED_WriteByte(0x10, OLED_CMD);//---set high column address
	OLED_WriteByte(0x40, OLED_CMD);//--set start line address  
	OLED_WriteByte(0xB0, OLED_CMD);//--set page address
	OLED_WriteByte(0x81, OLED_CMD); // contract control
	OLED_WriteByte(0xFF, OLED_CMD);//--128   
	OLED_WriteByte(0xA1, OLED_CMD);//set segment remap 
	OLED_WriteByte(0xA6, OLED_CMD);//--normal / reverse
	OLED_WriteByte(0xA8, OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WriteByte(0x3F, OLED_CMD);//--1/32 duty
	OLED_WriteByte(0xC8, OLED_CMD);//Com scan direction
	OLED_WriteByte(0xD3, OLED_CMD);//-set display offset
	OLED_WriteByte(0x00, OLED_CMD);//
	OLED_WriteByte(0xD5, OLED_CMD);//set osc division
	OLED_WriteByte(0x80, OLED_CMD);//
	OLED_WriteByte(0xD8, OLED_CMD);//set area color mode off
	OLED_WriteByte(0x05, OLED_CMD);//
	OLED_WriteByte(0xD9, OLED_CMD);//Set Pre-Charge Period
	OLED_WriteByte(0xF1, OLED_CMD);//
	OLED_WriteByte(0xDA, OLED_CMD);//set com pin configuartion
	OLED_WriteByte(0x12, OLED_CMD);//
	OLED_WriteByte(0xDB, OLED_CMD);//set Vcomh
	OLED_WriteByte(0x20, OLED_CMD);//
	delay_us(100000);
	OLED_ClearAll();
	OLED_DisplayOn();
	
	OLED_DefaultMode();
}

/**
  * @brief  OLED set pos
  * @param  x:0~127
			y:0~63
  * @retval None
  */   
void OLED_Set_Pos(uint8_t x, uint8_t y)
{ 	
	uint8_t sy=0xB0+y;
	uint8_t sx0=((x&0xF0)>>4)|0x10;
	uint8_t sx1=x&0x0F;
	
	OLED_WriteByte(sy, OLED_CMD);
	OLED_WriteByte(sx0, OLED_CMD);
	OLED_WriteByte(sx1, OLED_CMD); 
}

/**
  * @brief  OLED display on
  * @param  None
  * @retval None
  */   
void OLED_DisplayOn(void)
{
	OLED_WriteByte(0X8D, OLED_CMD);  //SET DCDC命令
	OLED_WriteByte(0X14, OLED_CMD);  //DCDC ON
	OLED_WriteByte(0XAF, OLED_CMD);  //DISPLAY ON
}

/**
  * @brief  OLED display off
  * @param  None
  * @retval None
  */
void OLED_DisplayOff(void)
{
	OLED_WriteByte(0X8D, OLED_CMD);  //SET DCDC命令
	OLED_WriteByte(0X10, OLED_CMD);  //DCDC OFF
	OLED_WriteByte(0XAE, OLED_CMD);  //DISPLAY OFF
}

/**
  * @brief  OLED clear all display
  * @param  None
  * @retval None
  */
void OLED_ClearAll(void)  
{  
	uint8_t i=0,n=0;		    
	for(i=0; i<8; i++)  
	{  
		OLED_WriteByte(0xB0+i, OLED_CMD);    //设置页地址（0~7）
		OLED_WriteByte(0x00, OLED_CMD);      //设置显示位置—列低地址
		OLED_WriteByte(0x10, OLED_CMD);      //设置显示位置—列高地址   
		for(n=0; n<128; n++)
		{
			OLED_WriteByte(0x00, OLED_DATA); 
		}
	}
}

/**
  * @brief  OLED clear one line display
  * @param  0~7
  * @retval None
  */
void OLED_Clear(uint8_t lineX) 
{  
	uint8_t i=0;		    
	OLED_WriteByte(0xB0+lineX, OLED_CMD);    //设置页地址（0~7）
	OLED_WriteByte(0x00, OLED_CMD);      //设置显示位置—列低地址
	OLED_WriteByte(0x10, OLED_CMD);      //设置显示位置—列高地址   
	for(i=0; i<128; i++)
	{
		OLED_WriteByte(0x00, OLED_DATA); 
	}
}

/**
  * @brief  OLED show one char
  * @param  x:0~127
			y:0~63
			chr:char
			size:font 16
  * @retval None
  */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size)
{      	
	uint8_t c=0,i=0;	
	c = chr - ' ';			

	OLED_Set_Pos(x, y);	
	for(i=0; i<8; i++)
	{
		OLED_WriteByte(F8X16[c*16+i], OLED_DATA);
	}
	OLED_Set_Pos(x, y+1);
	for(i=0; i<8; i++)
	{
		OLED_WriteByte(F8X16[c*16+i+8], OLED_DATA);
	}		
} 

/**
  * @brief  OLED m^n
  * @param  None
  * @retval None
  */
uint32_t OLED_Pow(uint8_t m, uint8_t n)
{
	uint32_t result=1;	 
	while(n--)
	{
		result *= m;    
	}
	return result;
}
 	
/**
  * @brief  OLED show number
  * @param  x:0~127
			y:0~63
			num:0~4294967295
			len:1~10
			size:font 16
  * @retval None
  */
void OLED_ShowNum(uint8_t x, uint8_t y, uint16_t num, const uint8_t positive)
{ 
/*	
	uint8_t t,temp;
	uint8_t enshow=0;
	uint16_t xpos=x-8;
	uint8_t dot=0;
	
	for(t=0; t<5; t++)
	{
		dot++;
		xpos += 8;
		if(dot==3)
		{
			t--;
			OLED_ShowChar(xpos, y, '.', 16);
			continue;
		}
		temp = (num / OLED_Pow(10, 4-t)) % 10;
		if(enshow==0 && t<4)
		{
			if(temp==0)
			{
				OLED_ShowChar(xpos, y, ' ', 16);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(xpos, y, temp+'0', 16); 
	}
*/
	uint8_t temp=0;
	uint16_t number=num;
	
	temp = (number / OLED_Pow(10, 4)) % 10;
	if(temp==0)
	{
		if(positive!=0)
		{
			OLED_ShowChar(x, y, ' ', 16);
		}
		else
		{
			OLED_ShowChar(x, y, '-', 16);
		}
	}
	else
	{
		OLED_ShowChar(x, y, temp+'0', 16); 
	}
	
	temp = (number / OLED_Pow(10, 3)) % 10;
	OLED_ShowChar(x+8, y, temp+'0', 16);
	OLED_ShowChar(x+16, y, '.', 16);
	
	temp = (number / OLED_Pow(10, 2)) % 10;
	OLED_ShowChar(x+24, y, temp+'0', 16);
	
	temp = (number / OLED_Pow(10, 1)) % 10;
	OLED_ShowChar(x+32, y, temp+'0', 16);
	
	temp = (number / OLED_Pow(10, 0)) % 10;
	OLED_ShowChar(x+40, y, temp+'0', 16);
}

/**
  * @brief  OLED show string
  * @param  x:0~127
			y:0~63
			*chr:string pointer
			size:font 16
  * @retval None
  */
void OLED_ShowString(uint8_t x, uint8_t y, const uint8_t *chr, uint8_t len, uint8_t size)
{
	uint8_t lenght=len;
	while(lenght--)
	{
		OLED_ShowChar(x, y, *chr, size);
		x += 8;
		if(x>120)
		{
			x = 0;
			y += 2;
		}
		chr++;
	}
}

/**
  * @brief  OLED show chinese
  * @param  x:0~127
			y:0~63
			pos:chinese position
  * @retval None
  */
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t pos)
{      			    
	uint8_t t,adder=0;
	OLED_Set_Pos(x, y);
	
    for(t=0; t<16; t++)
	{
		OLED_WriteByte(Hzk[2*pos][t],OLED_DATA);
		adder += 1;
    }	
	
	OLED_Set_Pos(x, y+1);	
    for(t=0; t<16; t++)
	{	
		OLED_WriteByte(Hzk[2*pos+1][t],OLED_DATA);
		adder += 1;
     }					
}

/**
  * @brief  OLED default mode
  * @param  None
  * @retval None
  */
void OLED_DefaultMode(void)
{
	OLED_ShowString(12,0, "Buck Convert", 12, 16);
	OLED_ShowChinese(0, 3, 1); 	//电
	OLED_ShowChinese(16, 3, 2);	//压
	OLED_ShowChar(32, 3, ':', 16);
	OLED_ShowNum(X_VO, Y_VO, 0, 1);
	OLED_ShowChar(X_UNIT, Y_VO, 'V', 16);
	OLED_ShowChar(X_UNIT+8, Y_VO, '>', 16);
	
	OLED_ShowChinese(0, 6, 1); 	//电
	OLED_ShowChinese(16, 6, 3);	//流
	OLED_ShowChar(32, 6, ':', 16);
	OLED_ShowNum(X_IO, Y_IO, 0, 1);
	OLED_ShowChar(X_UNIT, Y_IO, 'A', 16);
}

/**
  * @brief  OLED display
  * @param  None
  * @retval None
  */
void OLED_ShowVI(float v, float i)
{
	float t_v=v;
	float t_i=i;
	uint8_t str[7];

	sprintf((char*)&str[0], "%2.4f", t_v);
	OLED_ShowString(X_VO, Y_VO, (uint8_t*)&str[0], 6, 16);
	sprintf((char*)&str[0], "%2.4f", t_i);
	OLED_ShowString(X_IO, Y_IO, (uint8_t*)&str[0], 6, 16);
}













