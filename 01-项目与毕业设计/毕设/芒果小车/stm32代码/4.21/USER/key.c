#include "stm32f10x.h"                  // Device header
#include "lk.h"
#include "sys.h"
#include "delay.h"
#include "key.h"
int startflag=0;
int blockflag=0;
int stopflag=0;

int ax=0;
int by=0;



void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 ;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 ;
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 ;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


}
void Led_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


}

void BUZZ_Init(void)//按键初始化
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


}

void Block_Init(void)
{
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

GPIO_InitTypeDef GPIO_InitStructure;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  // 改为下拉输入模式
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 输入模式下速度不影响，但需填写
GPIO_Init(GPIOB, &GPIO_InitStructure);


}
//12开始启动（按键），13避障，14停止（语音控制），15语音开始
void Key_Get(void)
{
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0)
	{
	
		delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0);
		delay_ms(20);
		 startflag=1;
	}
//	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)//b13做臂章用了
//	{
//	
//		delay_ms(20);
//		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0);
//		delay_ms(20);
//		stopflag=1;
//	}	

}
//uint8_t key_scan(void)
//{
//	uint8_t keynum = 0;
//	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8) == 0) //用于检测输入端口电平的函数
//	{
//		delay_ms(10);//消抖
//		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8) == 0);//松手检测
//		delay_ms(10);
//		keynum = 1;
//	}
//	
//	return keynum ;
//}
//                        

void Block(void) 
	{
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0)
 { 	
		 blockflag=1;
GPIO_SetBits(GPIOA, GPIO_Pin_8); // 蜂鸣器响
}
 if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 1) { // 检测高电平	
		 blockflag=0;
GPIO_ResetBits(GPIOA, GPIO_Pin_8); // 蜂鸣器bu响
}
   if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0) { // 语音停止14
		 stopflag=1;}
 if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0) { // 语音开始15
		 startflag=1;}
}
//else 
//{
//blockflag=0;
//}


