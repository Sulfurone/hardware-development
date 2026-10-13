#include "stm32f10x.h"                  // STM32F10x设备头文件
#include "AD9833.h"                     // AD9833函数库头文件
#include "system.h"                     // 系统配置头文件
#include "common.h"                     // 通用函数头文件
#include "oled.h"                       // OLED显示头文件
#include "Key.h"                        // 按键和拨码开关头文件
#include <math.h>                       // 数学函数库
#include "AD.h"   // ADC头文件

// LED和按键的位带操作定义
#define LED1    BIT_ADDR(GPIOB_ODR_Addr,0)    // LED1连接到PB0
#define LED2    BIT_ADDR(GPIOB_ODR_Addr,1)    // LED2连接到PB1

float amplitude = 1.0f; // 幅度，范围可自定
float freq = 1000.0f;   // 频率，单位Hz
uint16_t phase = 0;     // 相位，0~4095

float smooth_v0 = 0;
float smooth_v1 = 0;

#define FILTER_N 70   // 或更大，如100

uint16_t AD_Filter(uint16_t new_value, uint16_t *buf, uint8_t *index)
{
    static uint32_t sum = 0;
    sum -= buf[*index];
    buf[*index] = new_value;
    sum += new_value;
    (*index)++;
    if(*index >= FILTER_N) *index = 0;
    return sum / FILTER_N;
}

// 对指定ADC通道采样N次，返回最大值
uint16_t GetADMaxValue(uint8_t ADC_Channel, uint8_t times)
{
    uint16_t max = 0;
    uint16_t temp;
    for(uint8_t i = 0; i < times; i++)
    {
        temp = AD_GetValue(ADC_Channel);
        if(temp > max) max = temp;
    }
    return max;
}

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
    float y;
    int last_x = 0, last_y = 0;
    float base_freq = 1000.0f;
    float cycles = 5.0f * freq / base_freq;
    for(int i = 0; i < 128; i++)
    {
        y = 15 + 15 * sin(2 * 3.14159f * cycles * i / 128.0f);
        OLED_DrawPoint(i, (int)y, 1);
        if(i > 0)
            OLED_DrawLine(last_x, last_y, i, (int)y, 1);
        last_x = i;
        last_y = (int)y;
    }
}

// 绘制矩形波
void DrawSquareWave(void)
{
    float base_freq = 1000.0f;
    float cycles = 5.0f * freq / base_freq;
    int period = (int)(128.0f / cycles);
    if(period < 1) period = 1;
    for(int i = 0; i < 128; i++)
    {
        if ((i % period) < (period / 2))
            OLED_DrawLine(i, 0, i, 15, 1);
        else
            OLED_DrawLine(i, 15, i, 30, 1);
    }
}

// 绘制三角波
void DrawTriangleWave(void)
{
    float y;
    int last_x = 0, last_y = 0;
    float base_freq = 1000.0f;
    float cycles = 5.0f * freq / base_freq;
    for(int i = 0; i < 128; i++)
    {
        y = 15 + 15 * (2 * fabs(2 * (cycles * i / 128.0f - floor(cycles * i / 128.0f + 0.5f))) - 1);
        OLED_DrawPoint(i, (int)y, 1);
        if(i > 0)
            OLED_DrawLine(last_x, last_y, i, (int)y, 1);
        last_x = i;
        last_y = (int)y;
    }
}



// 绘制反向锯齿波
void DrawReverseSawtoothWave(void)
{
    float y;
    int last_x = 0, last_y = 0;
    float base_freq = 1000.0f;
    float cycles = 5.0f * freq / base_freq;
    for(int i = 0; i < 128; i++)
    {
        y = 15 + 15 * (1 - 2 * (cycles * i / 128.0f - floor(cycles * i / 128.0f)));
        OLED_DrawPoint(i, (int)y, 1);
        if(i > 0)
            OLED_DrawLine(last_x, last_y, i, (int)y, 1);
        last_x = i;
        last_y = (int)y;
    }
}

// 更新波形显示
void UpdateWaveformDisplay(void)
{
    OLED_Clear();
    uint8_t sw1 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_13);
    uint8_t sw2 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12);
    uint32_t freq_khz = (uint32_t)(freq / 1000.0f + 0.5f);

    DrawCoordinateSystem();

    if(sw1 == 1 && sw2 == 1)
    {
        DrawTriangleWave();
        AD9833_SetWaveform(TRI_WAVE);
        OLED_ShowString(0, 48, (uint8_t*)"AMP:");
        OLED_ShowNum(32, 48, (uint16_t)smooth_v1, 2, 16);
        OLED_ShowString(48, 48, (uint8_t*)".");
        OLED_ShowNum(56, 48, (uint16_t)((smooth_v1 - (uint16_t)smooth_v1) * 100), 2, 16);
    }
    else if(sw1 == 0 && sw2 == 1)
    {
        DrawSquareWave();
        AD9833_SetWaveform(SQU_WAVE);
        OLED_ShowString(0, 48, (uint8_t*)"AMP:");
        OLED_ShowNum(32, 48, (uint16_t)smooth_v1, 2, 16);
        OLED_ShowString(48, 48, (uint8_t*)".");
        OLED_ShowNum(56, 48, (uint16_t)((smooth_v1 - (uint16_t)smooth_v1) * 100), 2, 16);
    }
    else if(sw1 == 1 && sw2 == 0)
    {
        DrawReverseSawtoothWave();
        AD9833_SetWaveform(SIN_WAVE);
        OLED_ShowString(0, 48, (uint8_t*)"AMP:");
        OLED_ShowNum(32, 48, (uint16_t)smooth_v0, 2, 16);
        OLED_ShowString(48, 48, (uint8_t*)".");
        OLED_ShowNum(56, 48, (uint16_t)((smooth_v0 - (uint16_t)smooth_v0) * 100), 2, 16);
    }
    else
    {
        DrawSineWave();
        AD9833_SetWaveform(SQU_WAVE);
        OLED_ShowString(0, 48, (uint8_t*)"AMP:");
        OLED_ShowNum(32, 48, (uint16_t)smooth_v0, 2, 16);
        OLED_ShowString(48, 48, (uint8_t*)".");
        OLED_ShowNum(56, 48, (uint16_t)((smooth_v0 - (uint16_t)smooth_v0) * 100), 2, 16);
    }

    // 右下角统一显示频率（加单位k，更靠左）
    OLED_ShowString(80, 48, (uint8_t*)"F:");
    OLED_ShowNum(88, 48, freq_khz, 3, 16);
    OLED_ShowString(110, 48, (uint8_t*)"k");

    OLED_Refresh_Gram();
}

float AD_To_RealVoltage(uint16_t ad_value)
{
    float voltage = (float)ad_value / 4095.0f * 3.3f; // 先转为电压
    return voltage * 3.0f; // 放大三倍
}

uint16_t ad0_buf[FILTER_N] = {0};
uint16_t ad1_buf[FILTER_N] = {0};
uint8_t ad0_idx = 0, ad1_idx = 0;

int main(void)
{
    SystemConfiguration();
    Key_Init();
    OLED_Init();
    AD9833_Init();
    AD_Init();
    AD9833_SetFrequency(freq, FREQ_REG0);
    UpdateWaveformDisplay();

    while(1)
    {
        // 拨码开关切换波形时刷新
        static uint8_t last_sw1 = 1, last_sw2 = 1;
        uint8_t current_sw1 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_13);
        uint8_t current_sw2 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12);
        if(current_sw1 != last_sw1 || current_sw2 != last_sw2)
        {
            UpdateWaveformDisplay();
            last_sw1 = current_sw1;
            last_sw2 = current_sw2;
        }

        // 按键调节频率
        uint8_t key = Key_GetNum();
        if(key == 4)
        {
            if(freq < 10000000.0f) freq += 1000.0f;
            AD9833_SetFrequency(freq, FREQ_REG0);
            OLED_Clear();
            OLED_ShowString(0,0,(uint8_t*)"FREQ+");
            OLED_ShowNum(40,0,(uint32_t)freq,7,16);
            OLED_Refresh_Gram();
        }
        else if(key == 5)
        {
            if(freq > 100.0f) freq -= 1000.0f;
            AD9833_SetFrequency(freq, FREQ_REG0);
            OLED_Clear();
            OLED_ShowString(0,0,(uint8_t*)"FREQ-");
            OLED_ShowNum(40,0,(uint32_t)freq,7,16);
            OLED_Refresh_Gram();
        }

        // AD采集与滤波
        uint16_t ad0 = GetADMaxValue(ADC_Channel_0, 10);
        uint16_t ad1 = GetADMaxValue(ADC_Channel_1, 10);
        uint16_t smooth_ad0 = AD_Filter(ad0, ad0_buf, &ad0_idx);
        uint16_t smooth_ad1 = AD_Filter(ad1, ad1_buf, &ad1_idx);
        float base_v0 = (float)smooth_ad0 / 4095.0f * 3.3f;
        float base_v1 = (float)smooth_ad1 / 4095.0f * 3.3f;
        smooth_v0 = base_v0 * 3.0f;
        smooth_v1 = base_v1 * 2.5f;

        // 频率大于10kHz时自动归零
        if(freq > 10000.0f) {
            freq = 0;
            AD9833_SetFrequency(freq, FREQ_REG0);
            UpdateWaveformDisplay(); // 保证显示也归零
        }

        // 实时刷新显示
        UpdateWaveformDisplay();
        Delay_ms(100);
    }
}

