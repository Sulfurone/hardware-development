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
#include "adc.h"
#include "oled.h"
#include <stdlib.h>

uint16_t AD0, AD1, AD2, AD3;
uint8_t key_value; //定义键值

extern int OUTPUT;// OUTPUT 是控制舵机的PWM输出值; OUTPUT1 和OUTPUT2 是控制小车左右轮速度的PWM输出值
extern	float target_voltage; // 目标电压，可根据需要修改
 float actual_voltage;
float voltage;
int main(void)
{
srand(1234); // 固定种子

// 在while(1)循环中添加：
int rand_current = rand() % 101 + 200; // 200~300之间的随机整数
char str[16];
	SystemInit();  //系统时钟的初始化
	delay_init();  //延时时钟的初始化
	 uart_init(9600);  //串口通信的初始化并设置相应的波推率9600，用于USB232口下载.hex工程文件时用
     Key_Init();

	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断分组，也就是程序先后执行的优先级
	 TIM2_PWM_Init(7199,0);  //PWM波的初始化，用于小车电机调速，传入7199和0，使得PWM的频率为10Khz    f=72000000/((7199+1)*(0+1))=10000
	TIM3_Init_Init(99,7199);	  //中断服务函数的初始化，也就是PWM波的初始化，这里的周期为10ms，也就是每10ms会执行一次control.c文件里TIM3_IRQHandler函数里的操作
	OLED_Init();
	AD_Init();
	
	
	while (1)
	{

Key_Get();

    Key_Get();
    AD0 = AD_GetValue_Filtered(ADC_Channel_0);
    AD1 = AD_GetValue_Filtered(ADC_Channel_1);

    actual_voltage = AD0 * 6 * 3.3f / 4095; // 实际电压
    voltage = AD1 * 3.3f / 4095; // 实际电压

    // 计算电流
    float vout = AD1 * 3.3f / 4095.0f;
    float current = (2.45f-vout) / 2f; // 5A型，单位A

    // OLED显示
    char str[16];
    sprintf(str, "V0:%.2fV", actual_voltage);
    OLED_ShowString(1, 1, str);
    sprintf(str, "OUT:%d", OUTPUT);
    OLED_ShowString(2, 1, str);
    sprintf(str, "Set:%.2fV", target_voltage);
    OLED_ShowString(3, 1, str);
    sprintf(str, "I:%.2fA", current);
    OLED_ShowString(4, 1, str);

    printf("%.3f,%d,%.3f,%.3f\n", actual_voltage, OUTPUT, target_voltage, current);
}

}
			 
			  


			
		



	
		


	
	

