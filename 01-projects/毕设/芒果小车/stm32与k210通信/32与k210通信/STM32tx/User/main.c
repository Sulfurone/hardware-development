#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "LED.h"
#include "string.h"
#include "uart2.h"
#include "hc05.h"


uint8_t RxSTA = 1;

int main(void)
{
	/*模块初始化*/
	OLED_Init();		//OLED初始化
	LED_Init();			//LED初始化
	Serial_Init();		//串口初始化
	
	USART2_init();
	HC05_Init();
	/*显示静态字符串*/
	OLED_ShowString(1, 1, "TxPacket");
	OLED_ShowString(3, 1, "RxPacket");
	LED1_OFF();
	 
	while (1)
	{		
		//SendString("health\r\n");
		//SendByte(0x88);
		//Delay_ms(1000);
		if (Serial_RxFlag == 1)		//如果接收到数据包
		{
			OLED_ShowString(4, 1, "                ");
			OLED_ShowString(4, 1, Serial_RxPacket);				//OLED清除指定位置，并显示接收到的数据包
			
			/*将收到的数据包与预设的指令对比，以此决定将要执行的操作*/
			if (strcmp(Serial_RxPacket, "Red") == 0)			//如果收到LED_ON指令
			{
				OLED_ShowString(2, 1, "Red");				//OLED清除指定位置，并显示LED_ON_OK
			}
			else if (strcmp(Serial_RxPacket, "Green") == 0)	//如果收到LED_OFF指令
			{
				//OLED_ShowString(2, 1, "                ");
				OLED_ShowString(2, 1, "Green");			//OLED清除指定位置，并显示LED_OFF_OK
			}
			
			else if (strcmp(Serial_RxPacket, "Yellow") == 0)	//如果收到LED_OFF指令
			{
//				OLED_ShowString(2, 1, "                ");
				OLED_ShowString(2, 1, "Yellow");			//OLED清除指定位置，并显示LED_OFF_OK
			}
			
			else if (strcmp(Serial_RxPacket, "Blue") == 0)	//如果收到LED_OFF指令
			{
//				OLED_ShowString(2, 1, "                ");
				OLED_ShowString(2, 1, "Blue");			//OLED清除指定位置，并显示LED_OFF_OK
			}
			
			else						//上述所有条件均不满足，即收到了未知指令
			{
				Serial_SendString("ERROR_COMMAND\r\n");			//串口回传一个字符串ERROR_COMMAND
				OLED_ShowString(2, 1, "                ");
				OLED_ShowString(2, 1, "ERROR_COMMAND");			//OLED清除指定位置，并显示ERROR_COMMAND
			}
			
			Serial_RxFlag = 0;			//处理完成后，需要将接收数据包标志位清零，否则将无法接收后续数据包
		}
	}
}
