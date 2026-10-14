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
#include "Light.h"
#include "KEY.h"
#include "DHT11.h"


volatile u8 temp;
volatile u8 humi;
extern u8 a;
volatile u8 human;
volatile u16 light;

extern volatile uint16_t pwm_value;
extern volatile uint8_t pwm_dir;
extern volatile uint8_t light_mode;
extern volatile uint8_t light_power;
extern volatile uint8_t online_control;
extern volatile uint16_t online_control_timer;  // 添加对online_control_timer的引用

void Timer2_Init(void);
void KEY_Process(void);  // 按键处理函数声明

int main(void)
{ 
	
  SystemInit();//设置系统时钟为72M	
	delay_init(72);
	LED_Init();
	LED_OFF();
	USART1_Config();//串口初始化
	USART2_Config();//串口初始化
	TIM3_Int_Init(1000-1,72-1);		//定时1ms中断
	OLED_Init();
	DHT11_Init();
	LDR_Init();
	BODY_HW_Init();
	Motor_Init();   // 初始化PWM输出
	KEY_Init();     // 初始化按键
	Timer2_Init();  // 初始化定时器2用于PWM控制


	
	// 显示静态字符
	OLED_ShowChineseCompact(1, 1, 0);  // ??"?" @ Col 1 (X=0)
	OLED_ShowChineseCompact(1, 3, 1);  // ??"?" @ Col 3 (X=16)
	OLED_ShowString(1, 5, ":"); // ??":" @ Col 5 (X=32)

	OLED_ShowChineseCompact(2, 1, 2);  // ??"?" @ Col 1 (X=0)
	OLED_ShowChineseCompact(2, 3, 1);  // ??"?" @ Col 3 (X=16)
	OLED_ShowString(2, 5, ":"); // ??":" @ Col 5 (X=32)

	OLED_ShowChineseCompact(3, 1, 3);  // ??"?" @ Col 1 (X=0)
	OLED_ShowChineseCompact(3, 3, 4);  // ??"?" @ Col 3 (X=16)
	OLED_ShowString(3, 5, ":"); // ??":" @ Col 5 (X=32)

	// 显示静态字符
	OLED_ShowChineseCompact(4, 1, 5);  // ??"?" @ Col 1 (X=0)
	OLED_ShowChineseCompact(4, 3, 6);  // ??"?" @ Col 3 (X=16)
	OLED_ShowString(4, 5, ":"); // ??":" @ Col 5 (X=32)


userInit();//机智云初始化与设置配网模式
delay_ms(500);
	gizwitsInit();
	delay_ms(500);
	gizwitsSetMode(WIFI_AIRLINK_MODE);
	
  // 添加DHT11读取计时器
  volatile uint32_t dht11_read_timer = 0;
  #define DHT11_READ_INTERVAL 20  // 每20次循环读取一次DHT11，约1秒

  while (1)
  {
		KEY_Process();  // 处理按键输入

		light = LDR_LuxData();
		OLED_ShowNum(3, 7, light, 3); // 显示光照强度值

		// 显示人体存在状态 - 每次循环都更新
		if (BODY_HW_GetData() == 1)
		{
			OLED_ShowString(2, 13, "YES");  // 有人在场时显示YES
			human=0;
		}
		else
		{
			OLED_ShowString(2, 13, "NO ");  // 无人在场时显示NO
			human=1;
		}

		// 显示当前模式
		if (light_mode == 1)  // 手动模式
		{
			OLED_ShowChineseCompact(4, 6, 9);   // 显示"手" @ Col 6
			OLED_ShowChineseCompact(4, 8, 10);  // 显示"动" @ Col 8
		}
		else if (light_mode == 0) // 自动模式
		{
			OLED_ShowChineseCompact(4, 6, 11);  // 显示"自" @ Col 6
			OLED_ShowChineseCompact(4, 8, 10);  // 显示"动" @ Col 8
		}
		else if (light_mode == 2) // 在线模式
		{
			OLED_ShowChineseCompact(4, 6, 12);  // 显示"在" @ Col 6
			OLED_ShowChineseCompact(4, 8, 13);  // 显示"线" @ Col 8
		}

		// 显示灯光电源状态和亮度
		if (light_power)
		{
			OLED_ShowString(1, 13, "ON ");  // 显示灯光开启状态
			// 显示灯光亮度百分比（PWM值反向转换为亮度值）
			OLED_ShowNum(3, 13, 99 - pwm_value, 2);  // 显示亮度值
		}
		else
		{
			OLED_ShowString(1, 13, "OFF");  // 显示灯光关闭状态
			OLED_ShowString(3, 13, "  ");   // 清除亮度值显示
		}

		// 读取温湿度 - 定时读取，避免频繁访问DHT11
		dht11_read_timer++;
		if (dht11_read_timer >= DHT11_READ_INTERVAL) 
		{
			u8 temp_value, humi_value;  // 临时变量用于接收DHT11数据
			DHT11_Read_Data(&temp_value, &humi_value);  // 使用非volatile类型的临时变量
			temp = temp_value;  // 将值赋给volatile变量
			humi = humi_value;
			dht11_read_timer = 0;  // 重置计时器
		}
		
		// 显示温湿度 - 每次循环都显示最新值
		OLED_ShowNum(1, 7, a, 2);  // 显示温度值
		OLED_ShowNum(2, 7, humi, 2);  // 显示湿度值
		
		userHandle();  // 处理用户数据		
		gizwitsHandle((dataPoint_t *)&currentDataPoint);
		
		// 添加在线模式自动切换逻辑
		if (currentDataPoint.valuemode == mode_VALUE2 && light_mode != 2)
		{
			// 强制设置为在线模式
			light_mode = 2;
			online_control = 1;
			online_control_timer = 0;
		}
		
		delay_ms(50);  // 减少延时，提高更新频率
	}
}

