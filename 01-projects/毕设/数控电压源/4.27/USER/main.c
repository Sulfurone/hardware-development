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

extern uint16_t ad0_buffer[AD_FILTER_LEN];
extern uint8_t ad0_index;
extern uint16_t ad1_buffer[AD_FILTER_LEN];
extern uint8_t ad1_index;

uint16_t AD0, AD1, AD2, AD3;
uint8_t key_value; // 按键值
extern int startflag;
extern int OUTPUT; // PWM输出值
extern float target_voltage; // 目标电压
float actual_voltage;
float voltage;
float xianshivoltage;
extern uint16_t input_voltage; // 从key.c中extern的input_voltage
extern uint16_t input_integer;
extern uint8_t input_decimal, inputting, inputting_decimal, decimal_pos;

#define SHOW_FILTER_LEN 10
uint16_t show_voltage_buffer[SHOW_FILTER_LEN] = {0};
uint8_t show_voltage_index = 0;

int main(void)
{
    // 系统初始化部分
    SystemInit();            // 初始化系统时钟，配置时钟树
    delay_init();            // 初始化延时函数，配置系统滴答定时器
    uart_init(9600);         // 初始化串口通信，波特率设置为9600
    Key_Init();              // 初始化按键，配置相关GPIO引脚
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 配置中断优先级分组为2
    TIM2_PWM_Init(7199,0);   // 初始化PWM输出，频率为10KHz
    TIM3_Init_Init(99,7199); // 初始化定时器3，用于系统控制
    OLED_Init();             // 初始化OLED显示屏
    AD_Init();               // 初始化ADC，用于电压采样

    while (1)
    {
        Key_Get(); // 扫描按键输入，处理按键事件

        // ADC采样处理部分
        // AD0用于采样输出电压，直接读取不做滤波
        // AD1使用滑动平均滤波处理
			  AD0 = AD_GetValue(ADC_Channel_0);    // 采样输出电压值，这个地方我们计算pid的值是未滤波的，滤波的值输入pid反应会慢，oled显示的值是滤波的
        AD1 = FilterADValue(ad1_buffer, &ad1_index, AD_GetValue(ADC_Channel_1)); // 采样电流并滤波处理，采样电流是滤波的，

        // 电压计算和处理部分
        voltage = AD1 * 3.3f / 4095;                  // 计算AD1通道电压值（已滤波）
        actual_voltage = AD0 * 6 * 3.3f / 4095;       // 计算实际输出电压值（未滤波）
        // 对实际输出电压进行滑动平均滤波，转换为显示值
        // 将电压值放大100倍进行整数运算，滤波后再除以100得到实际值
        xianshivoltage = FilterADValue(show_voltage_buffer, &show_voltage_index, (uint16_t)(actual_voltage * 100));
        xianshivoltage = xianshivoltage / 100.0f;

        // 电流计算部分
        float vout = AD1 * 3.3f / 4095.0f;            // 计算采样电压值
        float current = (vout - 2.53f) / 0.07f;       // 根据采样电压计算实际电流值，单位为安培，这个计算方法看acs712的技术手册
        
        // 电流保护处理
        if(current < 0.1f)//略有误差，防止在空载时显示电流
        {
					current = 0; // 电流值小于0.1时就为0
        }
        if(current > 3.0f)//电流大于3A时
        {
            startflag = 0; // 过流保护，停止输出
					TIM_SetCompare3(TIM2, 0);//pwm输出为0
        }

        // OLED显示部分
        char str[16];
        sprintf(str, "V0:%.2fV", xianshivoltage);     // 显示当前输出电压
        OLED_ShowString(1, 1, str);
        sprintf(str, "OUT:%d", OUTPUT);                // 显示PWM输出值
        OLED_ShowString(2, 1, str);
        
        // 显示设定电压值
        // 根据不同的输入状态显示不同格式
        if(inputting) {
            if(inputting_decimal && decimal_pos == 1)
                sprintf(str, "Set:%d.%dV", input_integer, input_decimal);    // 显示完整的设定值（整数和小数部分）
            else if(inputting_decimal && decimal_pos == 0)
                sprintf(str, "Set:%d.V", input_integer);                     // 显示正在输入小数点的状态
            else
                sprintf(str, "Set:%dV", input_integer);                      // 仅显示整数部分
        } else {
            sprintf(str, "Set:%.1fV", target_voltage);                      // 显示目标电压值
        }
        OLED_ShowString(3, 1, "                ");     // 清空该行原有显示
        OLED_ShowString(3, 1, str);
        sprintf(str, "I:%.1fA", current);              // 显示当前电流值
        OLED_ShowString(4, 1, str);

        // 通过串口发送数据
        // 发送格式：输出电压,电流\n
        printf("%.3f,%.3f\n", xianshivoltage,  current);
    }
}


