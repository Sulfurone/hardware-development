#include "stm32f10x.h"      // STM32F10x 头文件
#include "delay.h"          // 延时函数
#include "sys.h"            // 系统初始化
#include "string.h"         // 字符串处理
#include "usart.h"          // 串口通信
#include "pwm.h"            // PWM 驱动
#include "Motor.h"          // 轮子控制
#include "led1.h"           // LED 控制
#include "stdio.h"          // 标准输入输出
#include "control.h"        // 控制逻辑
#include "key.h"            // 按键处理
#include "OLED.h"           // OLED 显示
#include "ADC.h"        


uint16_t adc_value,AD0;
extern int output;
extern volatile uint32_t msTicks; // 系统节拍计数器（声明）
extern char Serial_RxPacket[100]; // 串口接收数据包

int caiflag = 0;                  // 采样请求标志，由主循环设置，control.c定时器中断分步处理
uint8_t key_value;                // 按键值
volatile uint32_t msTicks = 0;    // 系统节拍计数器（定义）

int main(void)
{
	// ===================== 初始化部分 =====================
	SystemInit();                         // 系统初始化
	delay_init();                         // 延时初始化
	OLED_Init();                          // OLED 显示初始化
	Key_Init();                           // 按键初始化
Motor_Init();
                        // 串口初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 中断优先级分组
	TIM2_PWM_Init(7199, 0);               // 电机PWM 10KHz
	
	TIM3_Init_Init(99, 7199);             // 10ms定时中断
	AD_Init();
	
	OLED_ShowString(1, 1, "AD0:");
	OLED_ShowString(2, 1, "OUT:");

	while (1)
	{
		start();
		AD0 = AD_GetValue_Filtered(ADC_Channel_0);
//adc_value=4096-AD0;
adc_value=AD0;
		delay_ms(10);  // 控制采样频率
		OLED_ShowNum(1, 5, adc_value, 4);
		OLED_ShowNum(2, 5, output, 4);
	}
}




