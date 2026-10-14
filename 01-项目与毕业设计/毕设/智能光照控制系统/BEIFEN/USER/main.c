#include "stm32f10x.h"
#include "LED.h"
#include "usart.h"
#include "delay.h"
#include "oled.h"
#include "usart2.h"
#include "gizwits_product.h"
#include "gizwits_protocol.h"
#include "timer.h"
#include "BODY_HW.h"
#include "adcx.h"
#include "LDR.h"
#include "LED.h"
#include "Light.h"
#include "KEY.h"
#include "DHT11.h"

u8 temp;
u8 humi;
u16 light;

extern volatile uint16_t pwm_value;
extern volatile uint8_t pwm_dir;
extern volatile uint8_t light_mode;
extern volatile uint8_t light_power;

void Timer2_Init(void);
void KEY_Process(void);
int main(void)
{ 
	
  SystemInit();
	delay_init(72);
	LED_Init();
	LED_OFF();
	USART1_Config();
	USART2_Config();
	TIM3_Int_Init(1000-1,72-1);
	OLED_Init();
	DHT11_Init();
	LDR_Init();
	BODY_HW_Init();
	KEY_Init();
	Timer2_Init();
  PWM_Init();
  delay_ms(500);
	userInit();
  delay_ms(500);
	gizwitsInit();
  delay_ms(500);
	gizwitsSetMode(WIFI_AIRLINK_MODE);
	
	OLED_ShowChineseCompact(1, 1, 0);
	OLED_ShowChineseCompact(1, 3, 1);
	OLED_ShowString(1, 5, ":");

	OLED_ShowChineseCompact(2, 1, 2);
	OLED_ShowChineseCompact(2, 3, 1);
	OLED_ShowString(2, 5, ":");

	OLED_ShowChineseCompact(3, 1, 3);
	OLED_ShowChineseCompact(3, 3, 4);
	OLED_ShowString(3, 5, ":");


	OLED_ShowChineseCompact(4, 1, 5);
	OLED_ShowChineseCompact(4, 3, 6);
	OLED_ShowString(4, 5, ":");

  while (1)
  {
		KEY_Process();  

		light = LDR_LuxData();
		OLED_ShowNum(3, 7, light, 3);
		
		if (BODY_HW_GetData() == 1)
		{
			OLED_ShowString(2, 13, "YES");
		}
		else
		{
			OLED_ShowString(2, 13, "NO ");
		}

		if (light_mode == 1)
		{
			OLED_ShowChineseCompact(4, 6, 9);
			OLED_ShowChineseCompact(4, 8, 10);
		}
		else
		{
			OLED_ShowChineseCompact(4, 6, 11);
			OLED_ShowChineseCompact(4, 8, 10);
		}

		if (light_power)
		{
			OLED_ShowString(1, 13, "ON ");
			OLED_ShowNum(3, 13, 99 - pwm_value, 2);
		}
		else
		{
			OLED_ShowString(1, 13, "OFF");
			OLED_ShowString(3, 13, "  ");
		}
 
		DHT11_Read_Data(&temp,&humi);
		OLED_ShowNum(1, 7, temp, 2);
		OLED_ShowNum(2, 7, humi, 2);
		
		userHandle();
		gizwitsHandle((dataPoint_t *)&currentDataPoint);
		delay_ms(200);  
	}
}

