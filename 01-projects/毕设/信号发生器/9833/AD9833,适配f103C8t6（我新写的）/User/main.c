#include "stm32f10x.h"                  // STM32F10x设备头文件
#include "AD9833.h"                     // AD9833函数库头文件
#include "system.h"                     // 系统配置头文件
#include "common.h"                     // 通用函数头文件

// LED和按键的位带操作定义
#define LED1    BIT_ADDR(GPIOB_ODR_Addr,0)    // LED1连接到PB0
#define LED2    BIT_ADDR(GPIOB_ODR_Addr,1)    // LED2连接到PB1
#define KEY     BIT_ADDR(GPIOB_IDR_Addr,2)    // 按键连接到PB2

// AD9833时钟引脚定义
#define CLK     BIT_ADDR(GPIOB_ODR_Addr,14)   // AD9833时钟引脚连接到PB14

int main(void)
{    
    SystemConfiguration();              // 系统初始化：配置时钟、GPIO等
// 系统初始化
//    SystemInit();
    
    // 初始化AD9833
    AD9833_Init();
    
    // 设置1kHz正弦波
//    AD9833_SetFrequency(1000.0, FREQ_REG0);
//    AD9833_SetWaveform(SIN_WAVE);
    
    // 设置2kHz三角波
//     AD9833_SetFrequency(2000.0, FREQ_REG0);
//     AD9833_SetWaveform(TRI_WAVE);
    
    // 设置5kHz方波
     AD9833_SetFrequency(5000.0, FREQ_REG0);
     AD9833_SetWaveform(SQU_WAVE);
    

    while(1)
    {
        // 按键检测
        if(KEY == 0)                    // 按键按下
        {
            Delay_ms(10);               // 消抖
            if(KEY == 0)                // 再次确认按键按下
            {
                LED1 = !LED1;           // LED1状态翻转
                while(KEY == 0);        // 等待按键释放
            }
        }
        
        // 可以添加其他功能，如：
        // - LED闪烁
        // - 波形切换
        // - 频率调节等
    }
}

