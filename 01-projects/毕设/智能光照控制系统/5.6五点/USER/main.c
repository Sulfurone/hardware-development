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
/*****************���絥Ƭ�����******************
											STM32
 * ��Ŀ			:	������ƽ̨�ʵ��                     
 * �汾			: V1.0
 * ����			: 2024.10.7
 * MCU			:	STM32F103C8T6
 * �ӿ�			:	�ο�usart2.h							
 * BILIBILI	:	���絥Ƭ�����
 * CSDN			:	���絥Ƭ�����
 * ����			:	���� 

**********************BEGIN***********************/

u8 temp;
u8 humi;


u16 light;

// ??TIM.c????
extern volatile uint16_t pwm_value;
extern volatile uint8_t pwm_dir;
extern volatile uint8_t light_mode;
extern volatile uint8_t light_power;

void Timer2_Init(void);
void KEY_Process(void);  // ????????
int main(void)
{ 
	
  SystemInit();//����ϵͳʱ��Ϊ72M	
	delay_init(72);
	LED_Init();
	LED_OFF();
	USART1_Config();//���ڳ�ʼ��
	USART2_Config();//���ڳ�ʼ��
	TIM3_Int_Init(1000-1,72-1);		//��ʱ1ms�ж�
	OLED_Init();
	DHT11_Init();
	LDR_Init();
	BODY_HW_Init();
	Motor_Init();   // ?????PWM??
	KEY_Init();     // ?????
		Timer2_Init();  // ??????2??PWM???


	
	// ????????,??????
	OLED_ShowChineseCompact(1, 1, 0);  // ??"?" @ Col 1 (X=0)
	OLED_ShowChineseCompact(1, 3, 1);  // ??"?" @ Col 3 (X=16)
	OLED_ShowString(1, 5, ":"); // ??":" @ Col 5 (X=32)

	OLED_ShowChineseCompact(2, 1, 2);  // ??"?" @ Col 1 (X=0)
	OLED_ShowChineseCompact(2, 3, 1);  // ??"?" @ Col 3 (X=16)
	OLED_ShowString(2, 5, ":"); // ??":" @ Col 5 (X=32)

	OLED_ShowChineseCompact(3, 1, 3);  // ??"?" @ Col 1 (X=0)
	OLED_ShowChineseCompact(3, 3, 4);  // ??"?" @ Col 3 (X=16)
	OLED_ShowString(3, 5, ":"); // ??":" @ Col 5 (X=32)

	// ??????
	OLED_ShowChineseCompact(4, 1, 5);  // ??"?" @ Col 1 (X=0)
	OLED_ShowChineseCompact(4, 3, 6);  // ??"?" @ Col 3 (X=16)
	OLED_ShowString(4, 5, ":"); // ??":" @ Col 5 (X=32)



	userInit();

	gizwitsInit();
	delay_ms(500);
	gizwitsSetMode(WIFI_AIRLINK_MODE);
	
  while (1)
  {

		
		
	
				
	// ??????
		KEY_Process();  // ??????,????????????????

		// ???????
		light = LDR_LuxData();
		OLED_ShowNum(3, 7, light, 3); // ???":" ??,Col 7 (X=48)

		// ?????????
		if (BODY_HW_GetData() == 1)
		{
			OLED_ShowString(2, 13, "YES");  // ??????,?ON/OFF??
		}
		else
		{
			OLED_ShowString(2, 13, "NO ");  // ??????,?ON/OFF??
		}

		// ????
		if (light_mode == 1)  // ?0?????
		{
			OLED_ShowChineseCompact(4, 6, 9);   // ??"?" @ Col 6
			OLED_ShowChineseCompact(4, 8, 10);  // ??"?" @ Col 8
		}
		else                  // ?1?????
		{
			OLED_ShowChineseCompact(4, 6, 11);  // ??"?" @ Col 6
			OLED_ShowChineseCompact(4, 8, 10);  // ??"?" @ Col 8
		}

		// ????????????
		if (light_power)
		{
			OLED_ShowString(1, 13, "ON ");  // ??????
			// ???????
			OLED_ShowNum(3, 13, 99 - pwm_value, 2);  // ?????,?PWM?????????
		}
		else
		{
			OLED_ShowString(1, 13, "OFF");  // ??????
			OLED_ShowString(3, 13, "  ");   // ???????
		}

		// ???????
		DHT11_Read_Data(&temp,&humi);//
		OLED_ShowNum(1, 7, temp, 2);  // 显示温度值
		OLED_ShowNum(2, 7, humi, 2);  // 显示湿度值
		
		userHandle();

		gizwitsHandle((dataPoint_t *)&currentDataPoint);
		delay_ms(50);  // ?????????????
	}
}

