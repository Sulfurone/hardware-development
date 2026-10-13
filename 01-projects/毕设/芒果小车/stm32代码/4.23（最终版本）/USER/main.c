#include "stm32f10x.h"      // STM32F10x 头文件
#include "delay.h"          // 延时函数
#include "sys.h"            // 系统初始化
#include "string.h"         // 字符串处理
#include "usart.h"          // 串口通信
#include "pwm.h"            // PWM 驱动
#include "lunzi.h"          // 轮子控制
#include "led1.h"           // LED 控制
#include "stdio.h"          // 标准输入输出
#include "control.h"        // 控制逻辑
#include "lk.h"             // 其他功能模块
#include "key.h"            // 按键处理
#include "OLED.h"           // OLED 显示
#include "Serial.h"         // 串口通信
#include "control.h"        // 控制逻辑（重复包含，建议去除）

// 外部变量声明
extern int stopconfig;            // 停车配置参数
extern int sensor[4];             // 传感器数组
extern int Prestopconfig;         // 用于终点线识别
extern int startflag;             // 启动标志
extern int blockflag;             // 障碍物标志
extern int stopflag;              // 停车标志
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
	lunzi_Init();                         // 轮子初始化
	hongwai_Init();                       // 红外传感器初始化
	OLED_Init();                          // OLED 显示初始化
	Block_Init();                         // 障碍物检测初始化
	Key_Init();                           // 按键初始化
	BUZZ_Init();                          // 蜂鸣器初始化
	Led_Init();                           // LED 初始化
	Serial_Init();                        // 串口初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 中断优先级分组
	TIM2_PWM_Init(7199, 0);               // 电机PWM 10KHz
	TIM4_PWM_Init(19999, 71);             // 舵机PWM 50Hz
	TIM3_Init_Init(99, 7199);             // 10ms定时中断

	// OLED 显示初始化信息
	OLED_ShowString(1, 1, "TxPacket");
	OLED_ShowString(3, 1, "RxPacket");
	// OLED_ShowString(1, 1, "startflag"); 
	// OLED_ShowString(2, 1, "blockflag");
	// uint32_t stop_time = 0; // 记录停车起始时间
	// uint8_t stop_timer_active = 0; // 停车计时标志

	// ===================== 主循环 =====================
	while (1)
	{
		// ----------- 传感器与按键检测 -----------
		read_sensor();    // 读取传感器数据
		Key_Get();        // 获取按键值
		Block();          // 检测障碍物
		jieshou();        // 串口数据接收

		// ----------- OLED 实时数据显示 -----------
		OLED_ShowNum(1, 16, startflag, 1);   // 显示启动标志
		OLED_ShowNum(2, 16, blockflag, 1);   // 显示障碍物标志
		OLED_ShowNum(3, 16, stopflag, 1);    // 显示停车标志
		OLED_ShowNum(4, 16, caiflag, 1);     // 显示采样标志
		OLED_ShowNum(1, 14, sensor[3], 1);   // 显示传感器3
		OLED_ShowNum(2, 14, sensor[2], 1);   // 显示传感器2
		OLED_ShowNum(3, 14, sensor[1], 1);   // 显示传感器1
		OLED_ShowNum(4, 14, sensor[0], 1);   // 显示传感器0

		// ----------- 状态机切换 -----------
		if(startflag==1 && blockflag==0 && stopflag==0 && caiflag==0)
		{
			// 正常行驶：启动标志有效，无障碍物、未停车、未采样
			forward();    // 小车电机向前
		}
		else if(blockflag==1)
		{
			// 检测到障碍物，停车并清除相关标志
			stop(); 
			startflag = 0;
			blockflag = 0;
		}
		else if(startflag==1 && caiflag==1)
		{
			// 采样请求，停车
			stop(); 
			// OUTPUT1=0;
			// OUTPUT2=0;
		}
		else if (startflag==1 && stopflag==1)
		{
			// 识别到终点线，停车
			stop();     // 停车
		}
	}
}


