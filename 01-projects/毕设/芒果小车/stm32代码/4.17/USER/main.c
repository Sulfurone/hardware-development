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

int caiflag = 0; // 采样请求标志，由主循环设置，control.c定时器中断分步处理

uint8_t key_value; // 键值

volatile uint32_t msTicks = 0; // 这里定义

uint32_t last_yellow_time = 0; // 上一次yellow采摘的时间

typedef enum {
    CAR_IDLE = 0,      // 空闲
    CAR_FORWARD,       // 前进
    CAR_BLOCKED,       // 障碍物
    CAR_STOPPED,       // 终点线
    CAR_SERVO_SAMPLE   // 舵机采摘
} CarState_t;

CarState_t carState = CAR_IDLE;

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
//    OLED_ShowString(1, 1, "startflag"); 
//    OLED_ShowString(2, 1, "blockflag");

	uint32_t stop_time = 0; // 记录停车起始时间
	uint8_t stop_timer_active = 0; // 停车计时标志

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

		// 状态机切换
		switch (carState)
		{
			case CAR_IDLE:
				if (startflag == 1 && stopflag == 0)
					carState = CAR_FORWARD;
				break;

			case CAR_FORWARD:
				if (caiflag == 1) // 采摘请求
				{
					stop();                // 先停车
					if (!stop_timer_active) // 第一次进入，记录时间
					{
						stop_time = msTicks;
						stop_timer_active = 1;
					}
					if (msTicks - stop_time >= 3000) // 停车满3秒
					{
						carState = CAR_SERVO_SAMPLE; // 切换到采摘状态
						stop_timer_active = 0; // 复位标志
					}
				}
				else
				{
					forward(); // 正常行走
					stop_timer_active = 0; // 复位标志
					if (blockflag == 1)
						carState = CAR_BLOCKED;
					else if (stopflag == 1)
						carState = CAR_STOPPED;
				}
				break;

			case CAR_BLOCKED:
				stop();
				startflag = 0;
				blockflag = 0;
				carState = CAR_IDLE; // 处理完回到空闲
				break;

			case CAR_STOPPED:
				stop();// 可加蜂鸣器或其他提示
				carState = CAR_IDLE; // 处理完回到空闲
				break;

			case CAR_SERVO_SAMPLE:
				if (caiflag == 0) // 采摘完成
					carState = CAR_FORWARD; // 采摘完成后继续行走
				// 否则等待采摘完成
				break;

			default:
				carState = CAR_IDLE;
				break;
		}
	}
}


