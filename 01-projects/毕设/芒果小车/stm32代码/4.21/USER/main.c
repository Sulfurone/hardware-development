#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
#include "string.h"
#include "usart.h"
#include "pwm.h"
#include "lunzi.h"
#include "led1.h"
#include "stdio.h"
#include "control.h"
#include "lk.h"
#include "key.h"
#include "OLED.h"
#include "Serial.h"

// 外部变量声明
extern int stopconfig;      
extern int sensor[4];       
extern int Prestopconfig;   // 用于终点线识别
extern int startflag;
extern int blockflag;
extern int stopflag;
extern int ax;
extern int by;
extern volatile uint32_t msTicks; // 这里声明
extern char Serial_RxPacket[100];
int caiflag = 0; // 采样请求标志，由主循环设置，control.c定时器中断分步处理

uint8_t key_value; // 键值

volatile uint32_t msTicks = 0; // 这里定义




int main(void)
{
	// 初始化部分
	SystemInit();
	delay_init();
	lunzi_Init();
	hongwai_Init();
	OLED_Init();
	Block_Init();
	Key_Init();
	BUZZ_Init();
	Led_Init();
	Serial_Init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	TIM2_PWM_Init(7199, 0);      // 电机PWM 10KHz
	TIM4_PWM_Init(19999, 71);    // 舵机PWM 50Hz
	TIM3_Init_Init(99, 7199);    // 10ms定时中断

	OLED_ShowString(1, 1, "TxPacket");
	OLED_ShowString(3, 1, "RxPacket");
// OLED_ShowString(1, 1, "startflag"); 
//    OLED_ShowString(2, 1, "blockflag");
//	uint32_t stop_time = 0; // 记录停车起始时间
//	uint8_t stop_timer_active = 0; // 停车计时标志

	while (1)
	{
		// 传感器与按键检测
		read_sensor();
		Key_Get();
		Block();
		jieshou();

		// OLED 实时显示
		OLED_ShowNum(1, 16, startflag, 1);
		OLED_ShowNum(2, 16, blockflag, 1);
		OLED_ShowNum(3, 16, stopflag, 1);
		OLED_ShowNum(4, 16, caiflag, 1);
		OLED_ShowNum(1, 14, sensor[3], 1);
		OLED_ShowNum(2, 14, sensor[2], 1);
		OLED_ShowNum(3, 14, sensor[1], 1);
		OLED_ShowNum(4, 14, sensor[0], 1);
   


		// 状态机切换
			if(startflag==1&&blockflag==0&&stopflag==0&&caiflag==0)//如果stopconfig未改变，也即是等于0时，小车向前行驶
			{
			forward();    //小车电机向前
			}	
			else if(startflag==1&&blockflag==1)
			{
			 stop(); 
       startflag=0;
       blockflag=0;
				}
      else if(startflag==1&&caiflag==1)
			{
			 stop(); 
      
				}
			else if (startflag==1&&stopflag==1 ) //如果识别到终点线，stopconfig数值变大，不等于0，这时使小车刹车
			{
				stop();     //小车电机向后				
			}
			
	}
}


