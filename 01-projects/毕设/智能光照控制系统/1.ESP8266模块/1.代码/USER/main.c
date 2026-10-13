#include "stm32f10x.h"
#include "led.h"
#include "usart.h"
#include "delay.h"
#include "esp8266.h"

/*****************辰哥单片机设计******************
											STM32
 * 项目			:	ESP8266模块通信实验                     
 * 版本			: V1.0
 * 日期			: 2024.9.30
 * MCU			:	STM32F103C8T6
 * 接口			:	参看usart2.h							
 * BILIBILI	:	辰哥单片机设计
 * CSDN			:	辰哥单片机设计
 * 作者			:	辰哥 

**********************BEGIN***********************/

uint8_t flag;

int main(void)
{ 
	
  SystemInit();//配置系统时钟为72M	
	delay_init(72);
	LED_Init();
	LED_On();
	USART_Config();
	
	ESP8266_Init ();   //初始化WiFi模块使用的接口和外设（使用串口2）
  ESP8266_StaTcpClient ();//WiFi模块设置
	
	printf("Start \n");
	delay_ms(1000);
	

  while (1)
  {
		switch(flag)
		{
			case 'a': LED_On();break;//开灯
			
			case 'c': LED_Off();break;//关灯
		}	
  }
}




