#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "DHT11.h"

DHT11_Data_TypeDef DHT11_Data;

int main(void)
{
	OLED_Init();
	DHT11_GPIO_Config();
	
	OLED_ShowString(1, 1, "temp:");
	OLED_ShowString(2, 1, "humidity:");
	
	while (1)
	{
		
		if(Read_DHT11(&DHT11_Data) == SUCCESS)
		{
			OLED_ShowNum(2, 10, DHT11_Data.humi_int, 2);
			OLED_ShowString(2, 12, ".");
			OLED_ShowNum(2, 13, DHT11_Data.humi_deci, 2);
			
			OLED_ShowNum(1, 6, DHT11_Data.temp_int, 2);
			OLED_ShowString(1, 8, ".");
			OLED_ShowNum(1, 9, DHT11_Data.temp_deci, 1);
			
		}
	}
}
