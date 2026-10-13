#include "stm32f10x.h"
#include "PWM.h"
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
#include "light_mode.h"

u8 temp;
u8 humi;
u16 light2;
int human;
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
	TIM3_Int_Init(500-1,72-1);
	OLED_Init();
	DHT11_Init();
	LDR_Init();
	BODY_HW_Init();
	KEY_Init();
	Timer2_Init();
  PWM_Init();
 
	userInit();

	gizwitsInit();
  delay_ms(500);
	gizwitsSetMode(WIFI_AIRLINK_MODE);
	
	// 初始化OLED显示
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

		// 读取光照值 - 尝试多次读取以确保可靠性
		uint16_t temp_light = 0;
		for(uint8_t i = 0; i < 3; i++) {
			temp_light = LDR_LuxData();
			if(temp_light > 0) {
				break; // 如果读取到非零值，则退出循环
			}
			delay_ms(5);
		}
		
		// 确保光照值非零，避免上传零值
		if(temp_light == 0) {
			temp_light = 1; // 至少使用最小值1
		}
		
		light2 = temp_light;
		OLED_ShowNum(3, 7, light2, 3);
		
		if (BODY_HW_GetData() == 0)
		{
			OLED_ShowString(2, 13, "YES");
			human = 1;
		}
		else
		{
			OLED_ShowString(2, 13, "NO ");
			human = 0;
		}

		// 显示当前控制模式
		switch(light_mode)
		{
			case 0: // 自动模式
				OLED_ShowChineseCompact(4, 6, 11); // 自
				OLED_ShowChineseCompact(4, 8, 10); // 动
				break;
			case 1: // 手动模式
				OLED_ShowChineseCompact(4, 6, 9);  // 手
				OLED_ShowChineseCompact(4, 8, 10); // 动
				break;
			case 2: // 在线模式
				OLED_ShowChineseCompact(4, 6, 12); // 在
				OLED_ShowChineseCompact(4, 8, 13); // 线
				break;
			default:
				break;
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
 
		// 每次读取前增加一个延时，确保DHT11有足够时间准备数据
		delay_ms(20);
		
		// 检查DHT11读取结果，只有在成功时才更新显示
		if(DHT11_Read_Data(&temp, &humi) == 0)
		{
			// 读取成功，确保数据在合理范围内
			if(temp > 50) temp = 25;  // 温度超出范围，使用默认值
			if(humi < 20 || humi > 95) humi = 50;  // 湿度超出范围，使用默认值
			
			// 更新OLED显示
			OLED_ShowNum(1, 7, temp, 2);
			OLED_ShowNum(2, 7, humi, 2);
		}
		
		// 无论DHT11读取成功与否，都更新数据到云平台
    // 更新所有传感器数据到机智云平台
    userHandle();
		
		// 使用更短的上报间隔，提高数据上传效率
		static uint8_t report_count = 0;
		if(report_count >= 1) // 每次循环都执行数据上报处理
		{
			report_count = 0;
			gizwitsHandle((dataPoint_t *)&currentDataPoint);
		}
		report_count++;
		
		delay_ms(50);
	}
}

