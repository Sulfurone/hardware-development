#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "pwm.h"
#include "lunzi.h"
#include "led1.h"
#include "stdio.h"
#include "control.h"
#include "lk.h"
#include "key.h"
//#include "block.h"


extern int stopconfig; //引用lk.c定义的stopconfig，才能在main.c内使用stopconfig该变量
int sensor[9]={0,0,0,0,0,0,0,0,0};   //定义一个数组，用于储存红外传感器返回的值，因为有九个传感器，所以定义数组内元素个数为9
int Prestopconfig=0;//定义一个变量，放到lk.c文件read_sensor函数里识别终点线的语句中
extern int startflag;
extern int blockflag;
extern int ax;
extern int by;

uint8_t key_value; //定义键值

int main(void)
{
	SystemInit();  //系统时钟的初始化
	delay_init();  //延时时钟的初始化
	lunzi_Init();  //电机运行的状态的初始化（电机的前进或者后退）
	hongwai_Init();  //九个红外传感器连接最小系统板上的管脚IO口的初始化
//	OLED_Init();
//	 uart_init(9600);  //串口通信的初始化并设置相应的波推率9600，用于USB232口下载.hex工程文件时用
	   Block_Init();
     Key_Init();
BUZZ_Init(); 
Led_Init();

	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断分组，也就是程序先后执行的优先级
	 TIM2_PWM_Init(7199,0);  //PWM波的初始化，用于小车电机调速，传入7199和0，使得PWM的频率为10Khz    f=72000000/((7199+1)*(0+1))=10000
TIM4_PWM_Init(19999,71);   //PWM波的初始化，用于舵机调速，这里设置的频率为50hz  这样得出的周期T=1/f=20ms  因为MG90S舵机的PWM驱动信号为20ms，否侧无法驱动
	GPIO_SetBits(GPIOB,GPIO_Pin_12);//PB12管脚输出高电平	//我买的c8t6板是育松电子店买的，板上的LED灯接的IO管脚就是PB12
GPIO_SetBits(GPIOA,GPIO_Pin_4);//PB12管脚输出低电平
	TIM3_Init_Init(99,7199);	  //中断服务函数的初始化，也就是PWM波的初始化，这里的周期为10ms，也就是每10ms会执行一次control.c文件里TIM3_IRQHandler函数里的操作
	 		
//    OLED_ShowString(1, 1, "startflag");
//	  OLED_ShowNum(1, 16,startflag, 1);
	

			while(1) 
		{
//if(ax==1)
//{
////TIM_SetCompare1(TIM4,1750); //舵机控制
//GPIO_ResetBits(GPIOA,GPIO_Pin_8);//PB12管脚输出低电平

//}
			read_sensor();  //读取红外传感器返回值的函数初始化
      Key_Get();   
      Block();
			if(startflag==1&&blockflag==0)//如果stopconfig未改变，也即是等于0时，小车向前行驶
			{
			forward();    //小车电机向前
			}	
			else if(blockflag==1)
			{
			 stop();   
       startflag=0;
       blockflag=0;
				}
			else  //如果识别到终点线，stopconfig数值变大，不等于0，这时使小车刹车
			{
				stop();     //小车电机向后				
			}
//			if(Prestopconfig==0)//开始时Prestopconfig==0，起跑线就不会识别到，这样小车就不会一开始就执行刹车
//			{
//			delay_ms(400);//延时0.4s后才使Prestopconfig=1,这时小车已经过了起跑线，避过了起跑线的检测，这样再一次识别到起跑线(终点线)，也就是小车跑完了一圈，立即刹车
//			Prestopconfig=1;
//			}
//GPIO_SetBits(GPIOA,GPIO_Pin_8);
//GPIO_ResetBits(GPIOA,GPIO_Pin_8);//PB12管脚输出低电平
			
			
			
 if(by==1)
{
TIM_SetCompare1(TIM4,1750); //舵机控制
GPIO_ResetBits(GPIOA,GPIO_Pin_4);//PB12管脚输出低电平
//delay_ms(1000);					//延时1000ms=1s					//所以这里是使LED闪烁
//	by=0;
//	GPIO_SetBits(GPIOA,GPIO_Pin_8);//PB12管脚输出高电平	//我买的c8t6板是育松电子店买的，板上的LED灯接的IO管脚就是PB12
}
	

//		key_value=key_scan();
//		if(key_value == 1)
//		{
//			if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_8) == 0) //用于检测输出端口电平的函数
//			{
//				TIM_SetCompare1(TIM4,1900); //舵机控制
//				GPIO_ResetBits(GPIOA,GPIO_Pin_8); //置为高电平
//			}
//			else
//			{
//				GPIO_SetBits(GPIOA,GPIO_Pin_8);//置为低电平
//			}
//		}

			

		}
		
}
			 
			  


			
		



	
		


	
	

