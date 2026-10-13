#include "stm32f10x.h"                  // STM32F10x系列微控制器头文件
#include "adc.h"                        // ADC模块头文件

// 定义滑动平均滤波的缓冲区长度
#define AD_FILTER_LEN 10

// 声明外部变量，用于存储ADC采样值
extern uint16_t AD0, AD1, AD2, AD3;

// 定义ADC通道0的滤波缓冲区
uint16_t ad0_buffer[AD_FILTER_LEN] = {0};
uint8_t ad0_index = 0;

// 定义ADC通道1的滤波缓冲区
uint16_t ad1_buffer[AD_FILTER_LEN] = {0};
uint8_t ad1_index = 0;

// 定义ADC通道2的滤波缓冲区
uint16_t ad2_buffer[AD_FILTER_LEN] = {0};
uint8_t ad2_index = 0;

// 定义ADC通道3的滤波缓冲区
uint16_t ad3_buffer[AD_FILTER_LEN] = {0};
uint8_t ad3_index = 0;

// 声明外部变量，用于存储电压值
extern float voltage;

/**
 * @brief ADC初始化函数
 * 配置ADC1的时钟、GPIO和基本参数
 */
void AD_Init(void)
{
    // 使能ADC1和GPIOA的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 配置ADC时钟为PCLK2的6分频
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    
    // 配置GPIO为模拟输入模式
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  // 模拟输入模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;  // 配置PA0-PA3为ADC输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
        
    // 配置ADC1的基本参数
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;  // 独立模式
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  // 数据右对齐
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  // 不使用外部触发
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  // 单次转换模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;  // 单通道模式
    ADC_InitStructure.ADC_NbrOfChannel = 1;  // 转换通道数为1
    ADC_Init(ADC1, &ADC_InitStructure);
    
    // 使能ADC1
    ADC_Cmd(ADC1, ENABLE);
    
    // ADC校准
    ADC_ResetCalibration(ADC1);  // 复位校准
    while (ADC_GetResetCalibrationStatus(ADC1) == SET);  // 等待复位校准完成
    ADC_StartCalibration(ADC1);  // 开始校准
    while (ADC_GetCalibrationStatus(ADC1) == SET);  // 等待校准完成
}

/**
 * @brief 获取指定ADC通道的采样值
 * @param ADC_Channel ADC通道号
 * @return 采样值的平均值
 */
uint16_t AD_GetValue(uint8_t ADC_Channel)
{
    uint32_t sum = 0;
    uint8_t i;
    // 采样10次取平均值
    for(i = 0; i < 10; i++)
    {
        // 配置ADC通道和采样时间
        ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SampleTime_55Cycles5);
        // 启动软件触发转换
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        // 等待转换完成
        while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
        // 累加转换结果
        sum += ADC_GetConversionValue(ADC1);
    }
    return sum / 10;  // 返回平均值
}

/**滑动平均滤波
 * @brief ADC采样值滑动平均滤波函数
 * @param buffer 滤波缓冲区
 * @param index 缓冲区索引
 * @param new_value 新的采样值
 * @return 滤波后的值
使用一个固定长度的缓冲区（AD_FILTER_LEN = 10）来存储最近的采样值
每次新的采样值会替换掉缓冲区中最旧的值
计算缓冲区中所有值的算术平均值作为滤波输出
 */
uint16_t FilterADValue(uint16_t *buffer, uint8_t *index, uint16_t new_value) {
    // 将新值存入缓冲区
    buffer[*index] = new_value;
    (*index)++;
    // 循环使用缓冲区
    if (*index >= AD_FILTER_LEN) *index = 0;
    
    // 计算平均值
    uint32_t sum = 0;
    for (uint8_t i = 0; i < AD_FILTER_LEN; i++) {
        sum += buffer[i];
    }
    return sum / AD_FILTER_LEN;
}


