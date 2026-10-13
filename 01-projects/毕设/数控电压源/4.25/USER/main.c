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
extern int OUTPUT; // PWM输出相关
extern float target_voltage; // 目标电压
float actual_voltage;
float voltage;
float xianshivoltage;
extern uint16_t input_voltage; // 在key.c里加上input_voltage的extern声明
extern uint16_t input_integer;
extern uint8_t input_decimal, inputting, inputting_decimal, decimal_pos;

#define SHOW_FILTER_LEN 10
uint16_t show_voltage_buffer[SHOW_FILTER_LEN] = {0};
uint8_t show_voltage_index = 0;

int main(void)
{
    // 系统初始化
    SystemInit();            // 系统时钟初始化
    delay_init();            // 延时函数初始化
    uart_init(9600);         // 串口初始化，波特率9600
    Key_Init();              // 按键初始化
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 中断优先级分组
    TIM2_PWM_Init(7199,0);   // PWM初始化，10KHz
    TIM3_Init_Init(99,7199); // 定时器3初始化，用于控制
    OLED_Init();             // OLED初始化
    AD_Init();               // ADC初始化

    while (1)
    {
        Key_Get(); // 按键扫描

        // ADC采样，AD0不滤波，AD1~AD3进行滑动平均滤波
        AD0 = AD_GetValue(ADC_Channel_0); // 直接采样AD0
        AD1 = FilterADValue(ad1_buffer, &ad1_index, AD_GetValue(ADC_Channel_1));
        AD2 = FilterADValue(ad2_buffer, &ad2_index, AD_GetValue(ADC_Channel_2));
        AD3 = FilterADValue(ad3_buffer, &ad3_index, AD_GetValue(ADC_Channel_3));

        // 电压、电流计算
        voltage = AD1 * 3.3f / 4095;                  // 通道1电压（滤波后）
        actual_voltage = AD0 * 6 * 3.3f / 4095;       // 实际输出电压（未滤波）
        // 对actual_voltage做滑动平均滤波，得到xianshivoltage
        xianshivoltage = FilterADValue(show_voltage_buffer, &show_voltage_index, (uint16_t)(actual_voltage * 100));
        xianshivoltage = xianshivoltage / 100.0f;
        float vout = AD1 * 3.3f / 4095.0f;            // 通道1电压（浮点）
        float current = (2.5f - vout) / 0.2f;         // 电流计算（单位A）
        if(current < 0.1f)
        {
            current = 0; // 电流小于0时归零
        }
        if(current > 3.0f)
        {
            startflag = 0; // 电流过大，停止输出
            TIM_SetCompare3(TIM2, 0);
        }

        // OLED显示相关
        char str[16];
        sprintf(str, "V0:%.2fV", xianshivoltage);
        OLED_ShowString(1, 1, str);
        sprintf(str, "OUT:%d", OUTPUT);
        OLED_ShowString(2, 1, str);
        if(inputting) {
            if(inputting_decimal && decimal_pos == 1)
                sprintf(str, "Set:%d.%dV", input_integer, input_decimal);
            else if(inputting_decimal && decimal_pos == 0)
                sprintf(str, "Set:%d.V", input_integer); // 小数点已按下但未输入小数
            else
                sprintf(str, "Set:%dV", input_integer);
        } else {
            sprintf(str, "Set:%.1fV", target_voltage);
        }
        OLED_ShowString(3, 1, "                "); // 清空第三行
        OLED_ShowString(3, 1, str);
        sprintf(str, "I:%.1fA", current);
        OLED_ShowString(4, 1, str);

        // 串口输出调试信息
        printf("%.3f,%.3f\n", xianshivoltage,  current);
    }
}


