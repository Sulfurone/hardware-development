#include "SmokeSensor.h"
#include "Delay.h"

/**
  * @brief  烟雾传感器初始化
  * @param  无
  * @retval 无
  */
void SmokeSensor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    
    // 使能GPIOA时钟
    RCC_APB2PeriphClockCmd(SMOKE_SENSOR_RCC, ENABLE);
    
    // 配置PA5为模拟输入
    GPIO_InitStructure.GPIO_Pin = SMOKE_SENSOR_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  // 模拟输入模式
    GPIO_Init(SMOKE_SENSOR_PORT, &GPIO_InitStructure);
    
    // 使能ADC1时钟
    RCC_APB2PeriphClockCmd(SMOKE_SENSOR_ADC_RCC, ENABLE);
    
    // ADC初始化
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;           // 单通道模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;     // 单次转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  // 软件触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  // 数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1;                 // 1个转换通道
    ADC_Init(SMOKE_SENSOR_ADC, &ADC_InitStructure);
    
    // 配置ADC通道5，采样时间55.5周期
    ADC_RegularChannelConfig(SMOKE_SENSOR_ADC, SMOKE_SENSOR_CHANNEL, 1, ADC_SampleTime_55Cycles5);
    
    // 使能ADC
    ADC_Cmd(SMOKE_SENSOR_ADC, ENABLE);
    
    // ADC校准
    ADC_ResetCalibration(SMOKE_SENSOR_ADC);
    while(ADC_GetResetCalibrationStatus(SMOKE_SENSOR_ADC));
    
    ADC_StartCalibration(SMOKE_SENSOR_ADC);
    while(ADC_GetCalibrationStatus(SMOKE_SENSOR_ADC));
}

/**
  * @brief  读取烟雾传感器ADC值
  * @param  无
  * @retval ADC转换值 (0-4095)
  */
uint16_t SmokeSensor_ReadADC(void)
{
    // 启动ADC转换
    ADC_SoftwareStartConvCmd(SMOKE_SENSOR_ADC, ENABLE);
    
    // 等待转换完成
    while(!ADC_GetFlagStatus(SMOKE_SENSOR_ADC, ADC_FLAG_EOC));
    
    // 读取转换结果
    return ADC_GetConversionValue(SMOKE_SENSOR_ADC);
}

/**
  * @brief  获取烟雾浓度（百分比）
  * @param  无
  * @retval 烟雾浓度 (0-100%)
  */
float SmokeSensor_GetConcentration(void)
{
    uint16_t adc_value = SmokeSensor_ReadADC();
    
    // 假设ADC值越大，烟雾浓度越高
    // 将ADC值转换为百分比 (0-4095 -> 0-100%)
    float concentration = (float)adc_value / 4095.0f * 100.0f;
    
    return concentration;
}

/**
  * @brief  获取烟雾浓度等级字符串
  * @param  adc_value ADC值
  * @retval 浓度等级字符串
  */
char* SmokeSensor_GetLevelString(uint16_t adc_value)
{
    if(adc_value < SMOKE_THRESHOLD_LOW)
    {
        return "Low";
    }
    else if(adc_value < SMOKE_THRESHOLD_MEDIUM)
    {
        return "Medium";
    }
    else if(adc_value < SMOKE_THRESHOLD_HIGH)
    {
        return "High";
    }
    else
    {
        return "Danger";
    }
}