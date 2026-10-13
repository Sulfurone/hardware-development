#include "stm32f10x.h"                  // STM32F10x设备头文件
#include "AD9833.h"                     // AD9833函数库头文件
#include "system.h"                     // 系统配置头文件
#include "common.h"                     // 通用函数头文件
#include "oled.h"                       // OLED显示头文件
#include "Key.h"                        // 按键和拨码开关头文件
#include <math.h>                       // 数学函数库

// LED和按键的位带操作定义
#define LED1    BIT_ADDR(GPIOB_ODR_Addr,0)    // LED1连接到PB0
#define LED2    BIT_ADDR(GPIOB_ODR_Addr,1)    // LED2连接到PB1

float amplitude = 1.0f; // 幅度，范围可自定
float freq = 1000.0f;   // 频率，单位Hz
uint16_t phase = 0;     // 相位，0~4095

// 绘制坐标轴
void DrawCoordinateSystem(void)
{
    // 绘制X轴
    OLED_DrawLine(0, 32, 127, 32, 1);
    // 绘制Y轴
    OLED_DrawLine(64, 0, 64, 63, 1);
    // 绘制刻度
    for(int i = 0; i < 128; i += 16)
    {
        OLED_DrawLine(i, 30, i, 34, 1);
    }
    for(int i = 0; i < 64; i += 16)
    {
        OLED_DrawLine(62, i, 66, i, 1);
    }
}

// 绘制正弦波
void DrawSineWave(void)
{
    float x, y;
    int last_x = 0, last_y = 32;
    
    for(int i = 0; i < 128; i++)
    {
        x = i;
        // 调整周期和振幅，使波形更清晰
        y = 32 + 15 * sin(2 * 3.14159 * x / 20); // 周期为20，振幅为15
        
        // 绘制点
        OLED_DrawPoint(i, (int)y, 1);
        
        // 连接相邻点
        if(i > 0)
        {
            OLED_DrawLine(last_x, last_y, i, (int)y, 1);
        }
        
        last_x = i;
        last_y = (int)y;
    }
}

// 绘制矩形波
void DrawSquareWave(void)
{
    for(int i = 0; i < 128; i++)
    {
        if(i % 20 < 10)  // 占空比50%
        {
            OLED_DrawLine(i, 17, i, 47, 1);  // 高电平
        }
        else
        {
            OLED_DrawLine(i, 32, i, 32, 1);  // 低电平
        }
    }
}

// 绘制三角波
void DrawTriangleWave(void)
{
    float x, y;
    int last_x = 0, last_y = 32;
    
    for(int i = 0; i < 128; i++)
    {
        x = i;
        // 三角波公式
        y = 32 + 15 * (2 * fabs(2 * (x / 20 - floor(x / 20 + 0.5))) - 1);
        
        // 绘制点
        OLED_DrawPoint(i, (int)y, 1);
        
        // 连接相邻点
        if(i > 0)
        {
            OLED_DrawLine(last_x, last_y, i, (int)y, 1);
        }
        
        last_x = i;
        last_y = (int)y;
    }
}

// 更新波形显示
void UpdateWaveformDisplay(void)
{
    // 清屏
    OLED_Clear();
    
    // 绘制坐标轴
    DrawCoordinateSystem();
    
    // 根据拨码开关状态选择波形
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_13) == 1 && GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12) == 1)
    {
        // 11: 三角波
        DrawTriangleWave();
        AD9833_SetWaveform(TRI_WAVE);
    }
    else if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_13) == 0 && GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12) == 1)
    {
        // 01: 矩形波
        DrawSquareWave();
        AD9833_SetWaveform(SQU_WAVE);
    }
    else
    {
        // 00: 正弦波
        DrawSineWave();
        AD9833_SetWaveform(SIN_WAVE);
    }
    
    // 刷新显示
    OLED_Refresh_Gram();
}

int main(void)
{    
    SystemConfiguration();              // 系统初始化：配置时钟、GPIO等
    Key_Init();                         // 初始化按键和拨码开关
    OLED_Init();                        // 初始化OLED
    AD9833_Init();                      // 初始化AD9833
    
    // 设置初始频率
    AD9833_SetFrequency(freq, FREQ_REG0);  // 1kHz
    
    // 初始显示
    UpdateWaveformDisplay();
    
    while(1)
    {
        // 检测拨码开关变化
        static uint8_t last_sw1 = 1, last_sw2 = 1;
        uint8_t current_sw1 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_13);
        uint8_t current_sw2 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12);
        
        if(current_sw1 != last_sw1 || current_sw2 != last_sw2)
        {
            UpdateWaveformDisplay();
            last_sw1 = current_sw1;
            last_sw2 = current_sw2;
        }
        
        uint8_t key = Key_GetNum();
        uint8_t mode = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9); // 读取PA9

        if(mode == 1) // 幅度调节模式
        {
            if(key == 4) // 按键1：幅度增加
            {
                if(amplitude < 2.0f) amplitude += 0.1f;
                // AD9833_SetAmplitude(amplitude); // 如有外部幅度控制可调用
                OLED_Clear();
                OLED_ShowString(0,0,(uint8_t*)"AMP+");
                OLED_Refresh_Gram();
            }
            else if(key == 5) // 按键2：幅度减少
            {
                if(amplitude > 0.1f) amplitude -= 0.1f;
                // AD9833_SetAmplitude(amplitude);
                OLED_Clear();
                OLED_ShowString(0,0,(uint8_t*)"AMP-");
                OLED_Refresh_Gram();
            }
        }
        else // 频率调节模式
        {
            if(key == 4) // 按键1：频率增加
            {
                if(freq < 10000000.0f) freq += 1000.0f; // 步进100Hz，可根据需要调整
                AD9833_SetFrequency(freq, FREQ_REG0);
                OLED_Clear();
                OLED_ShowString(0,0,(uint8_t*)"FREQ+");
                OLED_ShowNum(40,0,(uint32_t)freq,7,16);
                OLED_Refresh_Gram();
            }
            else if(key == 5) // 按键2：频率减少
            {
                if(freq > 100.0f) freq -= 1000.0f;
                AD9833_SetFrequency(freq, FREQ_REG0);
                OLED_Clear();
                OLED_ShowString(0,0,(uint8_t*)"FREQ-");
                OLED_ShowNum(40,0,(uint32_t)freq,7,16);
                OLED_Refresh_Gram();
            }
        }
    }
}

