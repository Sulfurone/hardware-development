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

DHT11_Data_TypeDef DHT11_Data;

u16 light;
u16 value = 0;
u16 pre_value = 0;

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
	
	// 显示固定标题内容
	OLED_ShowString(1, 1, "temp:");
	OLED_ShowString(2, 1, "humidity:");
	OLED_ShowString(3, 1, "light:");
	OLED_ShowString(4, 1, "mode:");
	
	// 显示版本号，调试用
	OLED_ShowString(1, 13, "v1.1");
	
	Timer2_Init();  // 初始化定时器2用于PWM值更新
	
	// 调试信息，显示按键初始化完成
	OLED_ShowNum(2, 15, 8, 1);
	
	while (1)
	{
		// 处理按键输入
		KEY_Process();
		
		// 更新光照值显示
		light = LDR_LuxData();
		OLED_ShowNum(3, 7, light, 3);
		
		// 读取人体传感器数据
		value = BODY_HW_GetData();
		if (value != pre_value)
		{
			if (value == 1)
			{	
				OLED_ShowString(3, 11, "M");  // 有人移动
			}
			else
			{
				OLED_ShowString(3, 11, " ");  // 无人移动
			}
		}
		
		// 显示模式和亮度信息
		if (light_mode == 0)
		{
			OLED_ShowString(4, 6, "AUTO ");
		}
		else
		{
			OLED_ShowString(4, 6, "MAN  ");
		}
		
		// 显示灯的开关状态和亮度
		if (light_power)
		{
			OLED_ShowString(4, 11, "ON ");
			OLED_ShowNum(4, 14, 99 - pwm_value, 2);  // 显示亮度百分比
		}
		else
		{
			OLED_ShowString(4, 11, "OFF");
			OLED_ShowString(4, 14, "  ");  // 清除亮度显示
		}
		
		// 读取温湿度数据
		if (Read_DHT11(&DHT11_Data) == SUCCESS)
		{
			OLED_ShowNum(2, 10, DHT11_Data.humi_int, 2);
			OLED_ShowString(2, 12, ".");
			OLED_ShowNum(2, 13, DHT11_Data.humi_deci, 2);
			OLED_ShowNum(1, 6, DHT11_Data.temp_int, 2);
			OLED_ShowString(1, 8, ".");
			OLED_ShowNum(1, 9, DHT11_Data.temp_deci, 1);
		}
		
		pre_value = value;
		Delay_ms(50);  // 减少延时以提高按键响应速度
	}
}
