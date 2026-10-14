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

volatile u8 temp;//定义温度
volatile u8 humi;//定义湿度
volatile u8 human;//定义是否有人
volatile u16 light;//定义光照强度

extern volatile uint16_t pwm_value;//pwm占空比的值
extern volatile uint8_t light_mode;//孔子方式
extern volatile uint8_t light_power;//是否开灯
extern volatile uint8_t online_control;//在线控制
extern volatile uint16_t online_control_timer;  // 在线控制时间，就是定义一个时间，多长时间没反应就回到自动模式
extern volatile uint16_t auto_pwm;  // 自动模式下PWM值oled

void Timer2_Init(void);
void KEY_Process(void);  // 按键处理函数声明

int main(void)
{ 
  SystemInit();//设置系统时钟为72M	
	delay_init(72);    //延时函数初始化
	LED_Init();        //LED初始化
	LED_OFF();         //关闭LED
	USART1_Config();   //串口1初始化
	USART2_Config();   //串口2初始化
	TIM3_Int_Init(1000-1,72-1);		//定时1ms中断
	OLED_Init();       //OLED显示屏初始化
	DHT11_Init();      //温湿度传感器初始化
	LDR_Init();        //光敏电阻初始化
	BODY_HW_Init();    //人体感应模块初始化
	Motor_Init();      //PWM输出初始化（用于灯光控制）
	KEY_Init();        //按键初始化
	Timer2_Init();     //定时器2初始化（用于PWM控制）	
	// 显示静态字符 - OLED屏幕布局设置
	OLED_ShowChineseCompact(1, 1, 0);  // 显示"灯" @ 第1行第1列
	OLED_ShowChineseCompact(1, 3, 1);  // 显示"光" @ 第1行第3列
	OLED_ShowString(1, 5, ":"); // 显示":" @ 第1行第5列

	OLED_ShowChineseCompact(2, 1, 2);  // 显示"人" @ 第2行第1列
	OLED_ShowChineseCompact(2, 3, 1);  // 显示"体" @ 第2行第3列
	OLED_ShowString(2, 5, ":"); // 显示":" @ 第2行第5列

	OLED_ShowChineseCompact(3, 1, 3);  // 显示"光" @ 第3行第1列
	OLED_ShowChineseCompact(3, 3, 4);  // 显示"照" @ 第3行第3列
	OLED_ShowString(3, 5, ":"); // 显示":" @ 第3行第5列

	// 显示静态字符
	OLED_ShowChineseCompact(4, 1, 5);  // 显示"模" @ 第4行第1列
	OLED_ShowChineseCompact(4, 3, 6);  // 显示"式" @ 第4行第3列
	OLED_ShowString(4, 5, ":"); // 显示":" @ 第4行第5列


userInit();//机智云初始化与设置配网模式
delay_ms(500);
	gizwitsInit();      //初始化机智云协议
	delay_ms(500);
	gizwitsSetMode(WIFI_AIRLINK_MODE);  //设置为AirLink配网模式
  // 添加DHT11读取计时器
  volatile uint32_t dht11_read_timer = 0;
  #define DHT11_READ_INTERVAL 20  // 每20次循环读取一次DHT11，约1秒

  while (1)
  {
		KEY_Process();  // 处理按键输入

		light = LDR_LuxData();  // 读取光照强度值
		OLED_ShowNum(3, 7, light, 3); // 在OLED上显示光照强度值

		// 显示人体存在状态 - 每次循环都更新
		if (BODY_HW_GetData() == 1)
		{
			OLED_ShowString(2, 13, "YES");  // 有人在场时显示YES
			human=0;  // 人体存在标志位设为0
		}
		else
		{
			OLED_ShowString(2, 13, "NO ");  // 无人在场时显示NO
			human=1;  // 人体不存在标志位设为1
		}

		// 显示当前工作模式
		if (light_mode == 1)  // 手动模式
		{
			OLED_ShowChineseCompact(4, 6, 9);   // 显示"手" @ 第4行第6列
			OLED_ShowChineseCompact(4, 8, 10);  // 显示"动" @ 第4行第8列
		}
		else if (light_mode == 0) // 自动模式
		{
			OLED_ShowChineseCompact(4, 6, 11);  // 显示"自" @ 第4行第6列
			OLED_ShowChineseCompact(4, 8, 10);  // 显示"动" @ 第4行第8列
		}
		else if (light_mode == 2) // 在线模式
		{
			OLED_ShowChineseCompact(4, 6, 12);  // 显示"在" @ 第4行第6列
			OLED_ShowChineseCompact(4, 8, 13);  // 显示"线" @ 第4行第8列
		}

		// 显示灯光电源状态和亮度
		if (light_power)
		{
			OLED_ShowString(1, 13, "ON ");  // 显示灯光开启状态
			
			// 根据当前模式显示对应的PWM值
			if (light_mode == 0)  // 自动模式
			{
				// 显示自动计算的PWM值
				OLED_ShowNum(3, 13, 99 - auto_pwm, 2);  // 显示亮度值（99-PWM值）
			}
			else
			{		// 显示手动或在线模式的PWM值
				OLED_ShowNum(3, 13, 99 - pwm_value, 2);  // 显示亮度值（99-PWM值）
			}
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
			DHT11_Read_Data(&temp_value, &humi_value);  // 读取温湿度数据
			temp = temp_value;  // 将温度值赋给全局变量
			humi = humi_value;  // 将湿度值赋给全局变量
			dht11_read_timer = 0;  // 重置计时器
		}	
		// 显示温湿度 - 每次循环都显示最新值
		OLED_ShowNum(1, 7, temp, 2);  // 在OLED上显示温度值
		OLED_ShowNum(2, 7, humi, 2);  // 在OLED上显示湿度值
		
		userHandle();  // 处理用户数据		
		gizwitsHandle((dataPoint_t *)&currentDataPoint);  // 处理机智云数据点
		
		// 添加在线模式自动切换逻辑
		if (currentDataPoint.valuemode == mode_VALUE2 && light_mode != 2)
		{
			// 强制设置为在线模式
			light_mode = 2;  // 设置为在线模式
			online_control = 1;  // 启用在线控制
			online_control_timer = 0;  // 重置在线控制计时器
		}
		delay_ms(50);  // 延时50ms，减少延时提高更新频率，这个地方会提高数据刷新速度，但是机智云上不会体现出来，我不解
	}
}

