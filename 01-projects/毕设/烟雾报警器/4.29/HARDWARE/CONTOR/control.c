#include "stm32f10x.h"      // STM32F10x 头文件
#include "delay.h"          // 延时函数
#include "sys.h"            // 系统初始化
#include "string.h"         // 字符串处理
#include "pwm.h"            // PWM 驱动
#include "stdio.h"          // 标准输入输出
#include "control.h"        // 控制逻辑
#include "key.h"            // 按键处理
#include "OLED.h"           // OLED 显示
#include "ADC.h"        


extern uint16_t adc_value;
int output=0;
extern int startflag;
extern char Serial_RxPacket[100]; // 串口接收数据包

  
// PID 参数
#define KP  2.0f
#define KI  0.0f
#define KD  0.0f
#define TARGET_ADC_VALUE  2048

// 位置式 PID 计算（单参数）
int Position_PID(int measured) {
    static float error = 0, last_error = 0, integral = 0;
    error = TARGET_ADC_VALUE - measured;  // 目标值内置为宏

    integral += error;
    if (integral > 1000) integral = 1000;
    if (integral < -1000) integral = -1000;

    float derivative = error - last_error;
    last_error = error;

    return (int)(KP * error + KI * integral + KD * derivative);
}

// TIM3 中断服务函数
void TIM3_IRQHandler(void) {
    if (TIM3->SR & 0x0001) {
        TIM3->SR &= ~(1 << 0);
        output = Position_PID(adc_value);  // 直接使用全局变量 adc_value
        output = (output > 7200) ? 7200 : (output < 0) ? 0 : output;
       

TIM_SetCompare3(TIM2, output);



    }
}






