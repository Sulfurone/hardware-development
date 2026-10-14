#include "stm32f10x.h"
#include "led.h"
#include "usart.h"
#include "delay.h"
#include "oled.h"
#include "usart2.h"
#include "gizwits_product.h"
#include "gizwits_protocol.h"
#include "timer.h"
#include "dht11.h"

/*****************辰哥单片机设计******************
											STM32
 * 项目			:	机智云平台搭建实验                     
 * 版本			: V1.0
 * 日期			: 2024.10.7
 * MCU			:	STM32F103C8T6
 * 接口			:	参看usart2.h							
 * BILIBILI	:	辰哥单片机设计
 * CSDN			:	辰哥单片机设计
 * 作者			:	辰哥 

**********************BEGIN***********************/

u8 temp;
u8 humi;

int main(void)
{ 
	
  SystemInit();//配置系统时钟为72M	
	delay_init(72);
	LED_Init();
	LED_Off();
	USART1_Config();//串口初始化
	USART2_Config();//串口初始化
	TIM3_Int_Init(1000-1,72-1);		//定时1ms中断
	
	while(DHT11_Init())
	{
		printf("DHT11 Error \r\n");
	}
	
	OLED_Init();
	printf("Start \n");
	delay_ms(1000);
	//显示“温度:”
	OLED_ShowChinese(0,0,0,16,1);
	OLED_ShowChinese(16,0,1,16,1);
	OLED_ShowChar(32,0,':',16,1);
	//显示“湿度:”
	OLED_ShowChinese(0,16,2,16,1);
	OLED_ShowChinese(16,16,1,16,1);
	OLED_ShowChar(32,16,':',16,1);

	userInit();

	gizwitsInit();
	delay_ms(500);
	gizwitsSetMode(WIFI_AIRLINK_MODE);
	
  while (1)
  {
		DHT11_Read_Data(&temp,&humi);//
		
		
		userHandle();

		gizwitsHandle((dataPoint_t *)&currentDataPoint);
				
		//printf("temp %d ,humi %d\r\n",temp,humi);
		//LED_Toggle();
		delay_ms(1000);
		//显示温度数据
		OLED_ShowNum(48,0,temp,2,16,1);
		//显示湿度数据
		OLED_ShowNum(48,16,humi,2,16,1);
		
  }
}

