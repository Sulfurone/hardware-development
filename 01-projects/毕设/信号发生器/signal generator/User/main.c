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

#define FILTER_N 80   // 或更大，如100
#define USE_FILTER 0  // 0表示不使用滤波器，直接返回原始值；1表示使用滤波器

// 将数组和索引变量定义移到文件开头
uint16_t ad0_buf[FILTER_N] = {0};
uint16_t ad1_buf[FILTER_N] = {0};
uint8_t ad0_idx = 0, ad1_idx = 0;

// 全局AD值，用于在不同函数间共享
uint16_t ad0 = 0;
uint16_t ad1 = 0;

// 电压滤波相关参数
#define VOLTAGE_FILTER_N 10  // 电压滤波使用的样本数
float voltage0_buf[VOLTAGE_FILTER_N] = {0};
float voltage1_buf[VOLTAGE_FILTER_N] = {0};
uint8_t v0_idx = 0, v1_idx = 0;

uint16_t AD_Filter(uint16_t new_value, uint16_t *buf, uint8_t *index)
{
    if (USE_FILTER == 0) {
        // 直接返回原始值，便于调试
        buf[*index] = new_value; // 仍然存储，以便于查看
        (*index)++;
        if(*index >= FILTER_N) *index = 0;
        return new_value;
    }
    
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
        
        // 为ADC_Channel_1添加短延时，避免采样过快
        if(ADC_Channel == ADC_Channel_1)
            Delay_us(10); // 添加10微秒延时
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
    
    // 先计算第一个点
    y = 15 + 15 * sin(2 * 3.14159f * cycles * 0 / 128.0f);
    last_y = (int)y;
    
    // 分段绘制，每次绘制部分线段并加入延时
    for(int i = 1; i < 128; i += 4)
    {
        for(int j = 0; j < 4 && (i+j) < 128; j++)
        {
            int x = i + j;
            y = 15 + 15 * sin(2 * 3.14159f * cycles * x / 128.0f);
            int current_y = (int)y;
            OLED_DrawLine(last_x, last_y, x, current_y, 1);
            last_x = x;
            last_y = current_y;
        }
    }
}

// 绘制矩形波
void DrawSquareWave(void)
{
    float base_freq = 1000.0f;
    float cycles = 5.0f * freq / base_freq;
    int period = (int)(128.0f / cycles);
    if(period < 1) period = 1;
    
    // 分段绘制，每次绘制一小段
    for(int i = 0; i < 128; i += 16)
    {
        for(int j = 0; j < 16 && (i+j) < 128; j++)
        {
            int x = i + j;
            if ((x % period) < (period / 2))
                OLED_DrawLine(x, 15, x, 0, 1);
            else
                OLED_DrawLine(x, 15, x, 30, 1);
        }
    }
}

// 绘制三角波
void DrawTriangleWave(void)
{
    float y;
    int last_x = 0, last_y = 0;
    float base_freq = 1000.0f;
    float cycles = 5.0f * freq / base_freq;
    
    // 先计算第一个点
    y = 15 + 15 * (2 * fabs(2 * (cycles * 0 / 128.0f - floor(cycles * 0 / 128.0f + 0.5f))) - 1);
    last_y = (int)y;
    
    // 分段绘制，每次绘制部分线段并加入延时
    for(int i = 1; i < 128; i += 4)
    {
        for(int j = 0; j < 4 && (i+j) < 128; j++)
        {
            int x = i + j;
            y = 15 + 15 * (2 * fabs(2 * (cycles * x / 128.0f - floor(cycles * x / 128.0f + 0.5f))) - 1);
            int current_y = (int)y;
            OLED_DrawLine(last_x, last_y, x, current_y, 1);
            last_x = x;
            last_y = current_y;
        }
    }
}

// 绘制反向锯齿波
void DrawReverseSawtoothWave(void)
{
    float y;
    int last_x = 0, last_y = 0;
    float base_freq = 1000.0f;
    float cycles = 5.0f * freq / base_freq;
    
    // 先计算第一个点
    y = 15 + 15 * (1 - 2 * (cycles * 0 / 128.0f - floor(cycles * 0 / 128.0f)));
    last_y = (int)y;
    
    // 分段绘制，每次绘制部分线段并加入延时
    for(int i = 1; i < 128; i += 4)
    {
        for(int j = 0; j < 4 && (i+j) < 128; j++)
        {
            int x = i + j;
            y = 15 + 15 * (1 - 2 * (cycles * x / 128.0f - floor(cycles * x / 128.0f)));
            int current_y = (int)y;
            OLED_DrawLine(last_x, last_y, x, current_y, 1);
            last_x = x;
            last_y = current_y;
        }
    }
}

// 更新波形显示
void UpdateWaveformDisplay(void)
{
    static uint16_t last_ad0 = 0; // 保存上一次的AD值
    
    OLED_Clear();
    Delay_ms(2); // 减少清屏后延时
    
    uint8_t sw1 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_13);
    uint8_t sw2 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12);

    DrawCoordinateSystem();
    Delay_ms(2); // 减少绘制坐标系后延时

    // 根据开关状态绘制不同波形
    if(sw1 == 1 && sw2 == 1)
    {
        DrawTriangleWave();
        AD9833_SetWaveform(TRI_WAVE);
        
        // 三角波使用AD0通道电压
        OLED_ShowString(0, 48, (uint8_t*)"V:");
        OLED_ShowNum(12, 48, (uint16_t)smooth_v0, 1, 16);
        OLED_ShowString(20, 48, (uint8_t*)".");
        OLED_ShowNum(28, 48, (uint16_t)((smooth_v0 - (uint16_t)smooth_v0) * 100), 2, 16);
    }
    else if(sw1 == 0 && sw2 == 1)
    {
        DrawSquareWave();
        AD9833_SetWaveform(SQU_WAVE);
        
        // 方波使用AD0通道电压
        OLED_ShowString(0, 48, (uint8_t*)"V:");
        OLED_ShowNum(12, 48, (uint16_t)smooth_v0, 1, 16);
        OLED_ShowString(20, 48, (uint8_t*)".");
        OLED_ShowNum(28, 48, (uint16_t)((smooth_v0 - (uint16_t)smooth_v0) * 100), 2, 16);
    }
    else if(sw1 == 1 && sw2 == 0)
    {
        DrawReverseSawtoothWave();
        AD9833_SetWaveform(SQU_WAVE);
        
        // 锯齿波使用AD1通道电压
        OLED_ShowString(0, 48, (uint8_t*)"V:");
        OLED_ShowNum(12, 48, (uint16_t)smooth_v1, 1, 16);
        OLED_ShowString(20, 48, (uint8_t*)".");
        OLED_ShowNum(28, 48, (uint16_t)((smooth_v1 - (uint16_t)smooth_v1) * 100), 2, 16);
    }
    else
    {
        DrawSineWave();
        AD9833_SetWaveform(SIN_WAVE);
        
        // 正弦波使用AD0通道电压
        OLED_ShowString(0, 48, (uint8_t*)"V:");
        OLED_ShowNum(12, 48, (uint16_t)smooth_v0, 1, 16);
        OLED_ShowString(20, 48, (uint8_t*)".");
        OLED_ShowNum(28, 48, (uint16_t)((smooth_v0 - (uint16_t)smooth_v0) * 100), 2, 16);
    }
    
    Delay_ms(5); // 绘制波形后增加延时

    // 右下角显示频率（调整位置避免重叠）
    uint32_t display_freq = (uint32_t)freq; // 使用实际频率值
    
    // 统一位置和格式，留出足够空间
    OLED_ShowString(70, 48, (uint8_t*)"F:");
    
    if (display_freq >= 1000) {
        // 显示为整数kHz
        OLED_ShowNum(85, 48, display_freq/1000, 3, 16); // 最多3位数字
        OLED_ShowString(115, 48, (uint8_t*)"k");
    } else {
        // 频率小于1000Hz
        if (display_freq == 0) {
            OLED_ShowNum(85, 48, 0, 1, 16);
        } else {
            OLED_ShowString(85, 48, (uint8_t*)"<1");
        }
        OLED_ShowString(115, 48, (uint8_t*)"k");
    }

    // 添加延时，给OLED一些处理时间
    Delay_ms(5); // 减少到5ms
    OLED_Refresh_Gram();
    // 再添加一个延时，确保OLED有足够时间处理刷新命令
    Delay_ms(5); // 减少到5ms
}

// 电压均值滤波函数
float Voltage_Filter(float new_value, float *buf, uint8_t *index)
{
    // 检查是否所有值都为0，如果是初次运行，就填充所有缓冲区为新值
    uint8_t all_zero = 1;
    for(uint8_t i = 0; i < VOLTAGE_FILTER_N; i++) {
        if(buf[i] != 0) {
            all_zero = 0;
            break;
        }
    }
    
    if(all_zero) {
        // 初始化缓冲区所有值为当前值
        for(uint8_t i = 0; i < VOLTAGE_FILTER_N; i++) {
            buf[i] = new_value;
        }
        return new_value;
    }
    
    // 用新值替换缓冲区中的旧值
    buf[*index] = new_value;
    (*index)++;
    if(*index >= VOLTAGE_FILTER_N) *index = 0;
    
    // 计算平均值
    float sum = 0;
    for(uint8_t i = 0; i < VOLTAGE_FILTER_N; i++) {
        sum += buf[i];
    }
    
    return sum / VOLTAGE_FILTER_N;
}

// AD1电压专用的加权滤波函数
float Voltage1_Filter(float new_value, float *buf, uint8_t *index)
{
    // 检查是否所有值都为0，如果是初次运行，就填充所有缓冲区为新值
    uint8_t all_zero = 1;
    for(uint8_t i = 0; i < VOLTAGE_FILTER_N; i++) {
        if(buf[i] != 0) {
            all_zero = 0;
            break;
        }
    }
    
    if(all_zero) {
        // 初始化缓冲区所有值为当前值
        for(uint8_t i = 0; i < VOLTAGE_FILTER_N; i++) {
            buf[i] = new_value;
        }
        return new_value;
    }
    
    // 用新值替换缓冲区中的旧值
    buf[*index] = new_value;
    (*index)++;
    if(*index >= VOLTAGE_FILTER_N) *index = 0;
    
    // 计算加权平均值（新数据权重更高）
    float sum = 0;
    float total_weight = 0;
    
    for(uint8_t i = 0; i < VOLTAGE_FILTER_N; i++) {
        // 计算样本到当前位置的距离，越近权重越高
        uint8_t distance = (*index >= i) ? (*index - i) : (VOLTAGE_FILTER_N + *index - i);
        float weight = VOLTAGE_FILTER_N - distance; // 距离越近，权重越大
        
        sum += buf[i] * weight;
        total_weight += weight;
    }
    
    return sum / total_weight;
}

// 对AD1通道进行多次采样并应用中值滤波
uint16_t GetAD1FilteredValue(uint8_t times)
{
    uint16_t values[30];  // 存储采样值的数组，最多30个
    
    // 确保times不超过数组大小
    if(times > 30) times = 30;
    
    // 采集多个样本
    for(uint8_t i = 0; i < times; i++)
    {
        values[i] = AD_GetValue(ADC_Channel_1);
        Delay_us(50); // 较长的延时确保ADC稳定
    }
    
    // 冒泡排序
    for(uint8_t i = 0; i < times-1; i++) {
        for(uint8_t j = 0; j < times-i-1; j++) {
            if(values[j] > values[j+1]) {
                uint16_t temp = values[j];
                values[j] = values[j+1];
                values[j+1] = temp;
            }
        }
    }
    
    // 返回中值（去除最大最小值后的平均）
    uint32_t sum = 0;
    uint8_t count = 0;
    
    // 去掉10%的最大值和最小值
    uint8_t skip = times / 10;
    if(skip < 1) skip = 1;
    
    for(uint8_t i = skip; i < times-skip; i++) {
        sum += values[i];
        count++;
    }
    
    return (count > 0) ? (sum / count) : values[times/2];
}

int main(void)
{
    // 系统初始化配置，设置时钟、中断等基础功能
    SystemConfiguration();
    // 初始化按键和拨码开关，用于用户交互
    Key_Init();
    // 初始化OLED显示屏，用于显示波形和参数
    OLED_Init();
    // 初始化AD9833信号发生器芯片
    AD9833_Init();
    // 初始化ADC模数转换器，用于采集电压值
    AD_Init();
    // 设置AD9833的初始频率为全局变量freq (1000Hz)
    AD9833_SetFrequency(freq, FREQ_REG0);
    
    // 初始化ADC并获取初始AD值，每个通道采样10次取最大值
    ad0 = GetADMaxValue(ADC_Channel_0, 10);  // 采集通道0的电压值
    ad1 = GetADMaxValue(ADC_Channel_1, 10);  // 采集通道1的电压值
    
    // 计算初始电压值，避免滤波器启动时的不稳定状态
    // 将AD值转换为实际电压值，4095为ADC满量程，3.3V为参考电压，乘以系数进行校准
    float raw_v0 = (float)ad0 / 4095.0f * 3.3f * 2.0f;  // 通道0电压计算，乘2为分压比
    float raw_v1 = (float)ad1 / 4095.0f * 3.3f * 2.2f;  // 通道1电压计算，乘2.2为分压比
    // 初始化平滑电压值
    smooth_v0 = raw_v0;
    smooth_v1 = raw_v1;
    
    // 预填充电压滤波缓冲区，确保滤波器从稳定状态开始工作
    for(uint8_t i = 0; i < VOLTAGE_FILTER_N; i++) {
        voltage0_buf[i] = raw_v0;  // 填充通道0电压缓冲区
    }
    for(uint8_t i = 0; i < VOLTAGE_FILTER_N; i++) {
        voltage1_buf[i] = raw_v1;  // 填充通道1电压缓冲区
    }
    
    // 初始化OLED显示，清屏后显示初始波形
    OLED_Clear();  // 清除OLED屏幕内容
    Delay_ms(20);  // 等待OLED清屏完成
    UpdateWaveformDisplay();  // 更新波形显示
    
    // 定义刷新显示的时间控制变量
    uint32_t lastUpdateTime = 0;  // 上次更新显示的时间计数
    uint32_t displayUpdateInterval = 200; // 显示更新间隔，200ms刷新一次显示

    // 主循环，程序将一直在此循环中运行
    while(1)
    {
        // 检测拨码开关状态变化，用于切换波形类型
        static uint8_t last_sw1 = 1, last_sw2 = 1;  // 保存上一次的开关状态
        uint8_t current_sw1 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_13);  // 读取SW1开关状态
        uint8_t current_sw2 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12);  // 读取SW2开关状态
        // 如果开关状态发生变化，则更新波形显示
        if(current_sw1 != last_sw1 || current_sw2 != last_sw2)
        {
            // 更新保存的开关状态
            last_sw1 = current_sw1;
            last_sw2 = current_sw2;
            Delay_ms(5);  // 短暂延时防抖
            UpdateWaveformDisplay();  // 根据新的开关状态更新波形显示
            lastUpdateTime = 0;  // 重置显示更新计时器
            Delay_ms(20);  // 切换波形后短暂延时
        }

        // 检测按键状态，用于调节频率
        uint8_t key = Key_GetNum();  // 获取当前按下的按键编号
        if(key == 4)  // 如果按键4被按下（频率增加按键）
        {
            if(freq < 10000000.0f) freq += 1000.0f;  // 频率增加1000Hz，设置上限
            AD9833_SetFrequency(freq, FREQ_REG0);  // 更新AD9833的输出频率
            // 显示频率增加信息
            OLED_Clear();
            Delay_ms(2);
            OLED_ShowString(0,0,(uint8_t*)"FREQ+");  // 显示频率增加标识
            OLED_ShowNum(40,0,(uint32_t)freq,7,16);  // 显示当前频率值
            OLED_Refresh_Gram();  // 刷新OLED显示
            Delay_ms(20);
            lastUpdateTime = 0;  // 重置显示更新计时器
        }
        else if(key == 5)  // 如果按键5被按下（频率减少按键）
        {
            if(freq > 100.0f) freq -= 1000.0f;  // 频率减少1000Hz，设置下限
            AD9833_SetFrequency(freq, FREQ_REG0);  // 更新AD9833的输出频率
            // 显示频率减少信息
            OLED_Clear();
            Delay_ms(2);
            OLED_ShowString(0,0,(uint8_t*)"FREQ-");  // 显示频率减少标识
            OLED_ShowNum(40,0,(uint32_t)freq,7,16);  // 显示当前频率值
            OLED_Refresh_Gram();  // 刷新OLED显示
            Delay_ms(20);
            lastUpdateTime = 0;  // 重置显示更新计时器
        }

        // 采集ADC值并进行滤波处理
        ad0 = GetADMaxValue(ADC_Channel_0, 10);  // 采集通道0的电压，取10次采样的最大值
        ad1 = GetADMaxValue(ADC_Channel_1, 10);  // 采集通道1的电压，取10次采样的最大值
        
        // 将ADC值存入滤波缓冲区，但根据USE_FILTER设置决定是否使用滤波后的值
        AD_Filter(ad0, ad0_buf, &ad0_idx);  // 对通道0的ADC值进行滤波
        AD_Filter(ad1, ad1_buf, &ad1_idx);  // 对通道1的ADC值进行滤波
        
        // 将ADC原始值转换为电压值
        float raw_v0 = (float)ad0 / 4095.0f * 3.3f * 2.0f;  // 通道0电压计算
        float raw_v1 = (float)ad1 / 4095.0f * 3.3f * 6.0f;  // 通道1电压计算，注意这里系数变为6.0
        
        // 对电压值进行均值滤波，使显示更稳定
        smooth_v0 = Voltage_Filter(raw_v0, voltage0_buf, &v0_idx);  // 平滑通道0电压
        smooth_v1 = Voltage_Filter(raw_v1, voltage1_buf, &v1_idx);  // 平滑通道1电压

        // 频率超限保护：当频率超过10kHz时自动归零
        if(freq > 10000.0f) {
            freq = 0;  // 频率归零
            AD9833_SetFrequency(freq, FREQ_REG0);  // 更新AD9833输出频率
            UpdateWaveformDisplay();  // 更新显示以反映频率归零
            Delay_ms(20);
            lastUpdateTime = 0;  // 重置显示更新计时器
        }

        // 控制OLED显示更新频率，避免频繁刷新导致闪烁或资源浪费
        if(lastUpdateTime >= displayUpdateInterval) {  // 如果达到更新间隔
            UpdateWaveformDisplay();  // 更新波形显示
            lastUpdateTime = 0;  // 重置计时器
            Delay_ms(10);  // 短暂延时
        } else {
            lastUpdateTime += 50;  // 每次循环增加50ms计时
        }

        Delay_ms(50);  // 主循环延时，控制整体运行速度
    }
}

