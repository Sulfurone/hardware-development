#include "stm32f10x.h"      // STM32F10x 头文件
#include "delay.h"          // 延时函数
#include "sys.h"            // 系统初始化
#include "string.h"         // 字符串处理
#include "uart2.h"          // 串口通信
#include "pwm.h"            // PWM 驱动
#include "Motor.h"          // 电机控制
#include "stdio.h"          // 标准输入输出
#include "control.h"        // 控制逻辑
#include "key.h"            // 按键处理
#include "OLED.h"           // OLED 显示
#include "ADC.h"        
#include "hc05.h"  
#include "uart.h"
uint16_t adc_value,AD0;
extern int output;
uint8_t RxSTA = 1;
uint8_t key_value;                // 按键值
uint16_t pollution_index =0;

int startflag=0;
int fanflag=0;
int main(void)
{
	// ===================== 初始化部分 =====================
	SystemInit();                         // 系统初始化
	delay_init();                         // 延时初始化
	OLED_Init();                          // OLED 显示初始化                         
  Motor_Init();
  HC05_Init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 中断优先级分组
	TIM2_PWM_Init(7199, 0);               // 电机PWM 10KHz
	TIM3_Init_Init(99, 7199);             // 10ms定时中断
	AD_Init();
char RxData[100] = " ";

	OLED_ShowString(1, 1, "AD0:");
	OLED_ShowString(2, 1, "OUT:");
	OLED_ShowString(3, 1, "PI:");

	
	while (1)
	{
		start();

		HC05_GetData(RxData);
		// 判断是否收到A、C、B、E指令
		if (RxSTA == 0)
		{
			OLED_ShowString(4, 1, RxData);
			RxSTA = 1;
		}
		if (strcmp(RxData, "A") == 0)
		{
			startflag = 1;
		}
		
		else if (strcmp(RxData, "B") == 0)
		{
			fanflag = 1;
		}
		else if (strcmp(RxData, "E") == 0)
		{
			fanflag = 0;
		}

		// 风扇控制
		if (fanflag == 1)
		{
      startflag = 1;
			TIM_SetCompare3(TIM2,6000);//电机控制
		}
		else
		{
      startflag = 1;
			TIM_SetCompare3(TIM2,output);//电机控制
		}



		if (startflag == 1)
		{
			AD0 = AD_GetValue_Filtered(ADC_Channel_0);
			adc_value = 4096 - AD0;
			pollution_index = AD0 / 10;
		}
		else
		{
			adc_value = 4095;
			pollution_index = 0; 
		}



		delay_ms(10);  // 控制采样频率
		OLED_ShowNum(1, 5, adc_value, 4);
		OLED_ShowNum(2, 5, output, 4);
		OLED_ShowNum(3, 5, pollution_index, 4);
	}
}




