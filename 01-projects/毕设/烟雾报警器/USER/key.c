#include "stm32f10x.h"                  // 设备头文件
#include "sys.h"                        // 系统初始化
#include "delay.h"                      // 延时函数
#include "key.h"                        // 按键相关

// 全局标志变量
int startflag = 0;   // 开始标志位
int blockflag = 0;   // 障碍物标志位
int stopflag = 0;    // 停车标志位


// 按键初始化（PB12、PB14、PB15上拉输入）
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // 使能GPIOB时钟
	
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;         // 上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

// LED 初始化（PA4推挽输出）
void Led_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIOA时钟
	
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      // 推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

// 蜂鸣器初始化（PA8推挽输出）
void BUZZ_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIOA时钟
	
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      // 推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

// 障碍物检测引脚初始化（PB13下拉输入）
void Block_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // 使能GPIOB时钟

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;         // 下拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // 输入模式下速度无影响，但需填写
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

// 按键检测函数
// PB12：启动按键
// PB14：语音停止（未启用）
// PB15：语音开始（未启用）
void Key_Get(void)
{
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0) // 检测到按下
	{
		delay_ms(20); // 消抖
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0);
		delay_ms(20);
		startflag = 1; // 设置启动标志
	}
	// 语音停止按键（PB14）功能已注释
	// if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)
	// {
	//     delay_ms(20);
	//     while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0);
	//     delay_ms(20);
	//     stopflag = 1;
	// }	
}

// 障碍物检测与蜂鸣器控制
void Block(void)
{
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0)
	{
		blockflag = 1; // 设置障碍物标志
		GPIO_SetBits(GPIOA, GPIO_Pin_8); // 蜂鸣器响
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 1) { // 检测高电平	
		blockflag = 0; // 清除障碍物标志
		GPIO_ResetBits(GPIOA, GPIO_Pin_8); // 蜂鸣器bu响
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0) { // 语音停止14
		stopflag = 1;
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0) { // 语音开始15
		startflag = 1;
	}
}


