#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "DHT11.h"
#include "BODY_HW.h"
#include "adcx.h"
#include "LDR.h"
#include "LED.h"
#include "Light.h"
#include "KEY.h"
#include "gizwits_product.h"
#include "gizwits_protocol.h"
DHT11_Data_TypeDef DHT11_Data;

u16 light;

// 引用TIM.c中的变量
extern volatile uint16_t pwm_value;
extern volatile uint8_t pwm_dir;
extern volatile uint8_t light_mode;
extern volatile uint8_t light_power;

void Timer2_Init(void);
void KEY_Process(void);  // 按键处理函数声明

int main(void)
{
	OLED_Init();
	DHT11_GPIO_Config();
	LDR_Init();
	BODY_HW_Init();
	Motor_Init();   // 初始化电机PWM控制
	KEY_Init();     // 初始化按键
	userInit();
	gizwitsInit();
	
	OLED_Clear();  // 清屏
	
	// 显示固定标题内容，使用中文显示
	OLED_ShowChineseCompact(1, 1, 0);  // 显示"温" @ Col 1 (X=0)
	OLED_ShowChineseCompact(1, 3, 1);  // 显示"度" @ Col 3 (X=16)
	OLED_ShowString(1, 5, ":"); // 显示":" @ Col 5 (X=32)

	OLED_ShowChineseCompact(2, 1, 2);  // 显示"湿" @ Col 1 (X=0)
	OLED_ShowChineseCompact(2, 3, 1);  // 显示"度" @ Col 3 (X=16)
	OLED_ShowString(2, 5, ":"); // 显示":" @ Col 5 (X=32)

	OLED_ShowChineseCompact(3, 1, 3);  // 显示"光" @ Col 1 (X=0)
	OLED_ShowChineseCompact(3, 3, 4);  // 显示"照" @ Col 3 (X=16)
	OLED_ShowString(3, 5, ":"); // 显示":" @ Col 5 (X=32)

	// 显示模式标题
	OLED_ShowChineseCompact(4, 1, 5);  // 显示"模" @ Col 1 (X=0)
	OLED_ShowChineseCompact(4, 3, 6);  // 显示"式" @ Col 3 (X=16)
	OLED_ShowString(4, 5, ":"); // 显示":" @ Col 5 (X=32)

	Timer2_Init();  // 初始化定时器2用于PWM值更新
gizwitsSetMode(WIFI_AIRLINK_MODE);
	while (1)
	{
		// 处理按键输入
		KEY_Process();  // 保留按键处理，处理模式切换、亮度调节和开关控制

		// 更新光照值显示
		light = LDR_LuxData();
		OLED_ShowNum(3, 7, light, 3); // 显示在":" 后面，Col 7 (X=48)

		// 显示人体传感器状态
		if (BODY_HW_GetData() == 1)
		{
			OLED_ShowString(2, 13, "YES");  // 显示在右上角，与ON/OFF对齐
		}
		else
		{
			OLED_ShowString(2, 13, "NO ");  // 显示在右上角，与ON/OFF对齐
		}

		// 显示模式
		if (light_mode == 1)  // 为0时显示自动
		{
			OLED_ShowChineseCompact(4, 6, 9);   // 显示"自" @ Col 6
			OLED_ShowChineseCompact(4, 8, 10);  // 显示"动" @ Col 8
		}
		else                  // 为1时显示手动
		{
			OLED_ShowChineseCompact(4, 6, 11);  // 显示"手" @ Col 6
			OLED_ShowChineseCompact(4, 8, 10);  // 显示"动" @ Col 8
		}

		// 显示灯的开关状态在右上角
		if (light_power)
		{
			OLED_ShowString(1, 13, "ON ");  // 显示在右上角
			// 显示当前亮度值
			OLED_ShowNum(3, 13, 99 - pwm_value, 2);  // 显示亮度值，从PWM值转换为亮度百分比
		}
		else
		{
			OLED_ShowString(1, 13, "OFF");  // 显示在右上角
			OLED_ShowString(3, 13, "  ");   // 清除亮度值显示
		}

		// 读取温湿度数据
		if (Read_DHT11(&DHT11_Data) == SUCCESS)
		{
			OLED_ShowNum(1, 7, DHT11_Data.temp_int, 2); // 显示在":" 后面，Col 7 (X=48)
			OLED_ShowString(1, 9, ".");                 // Col 9 (X=64)
			OLED_ShowNum(1, 10, DHT11_Data.temp_deci, 1); // Col 10 (X=72)
			OLED_ShowNum(2, 7, DHT11_Data.humi_int, 2); // 显示在":" 后面，Col 7 (X=48)
			OLED_ShowString(2, 9, ".");                 // Col 9 (X=64)
			OLED_ShowNum(2, 10, DHT11_Data.humi_deci, 2); // Col 10 (X=72)
		}

		Delay_ms(50);  // 减少延时以提高按键响应速度
		
	}
}
