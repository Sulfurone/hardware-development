#include "stm32f10x.h"                  // Device header
#include <stdlib.h>

uint16_t filtered_AD1 = 0; // 滤波后的AD1值

#define ALPHA 0.2f  // 滤波系数，0.1~0.3之间，越小越平滑
#define N 9 // 采样点数，建议奇数

void AD_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
        
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    
    ADC_Cmd(ADC1, ENABLE);
    
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1) == SET);
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1) == SET);
}

uint16_t AD_GetValue(uint8_t ADC_Channel)
{
    uint32_t sum = 0;
    uint8_t i;
    for(i = 0; i < 10; i++) // ??10?
    {
        ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SampleTime_55Cycles5);
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
        sum += ADC_GetConversionValue(ADC1);
    }
    return sum / 10; // ????
}

void TIM4_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
        uint16_t raw = AD_GetValue(ADC_Channel_1); // 原始采样值
        filtered_AD1 = (uint16_t)(ALPHA * raw + (1 - ALPHA) * filtered_AD1);
        // 后续用filtered_AD1代替AD1即可
    }
}

uint16_t MedianFilter(uint8_t ADC_Channel)
{
    uint16_t buf[N];
    for(int i=0; i<N; i++)
        buf[i] = AD_GetValue(ADC_Channel);

    // 冒泡排序
    for(int i=0; i<N-1; i++)
        for(int j=0; j<N-1-i; j++)
            if(buf[j] > buf[j+1])
            {
                uint16_t tmp = buf[j];
                buf[j] = buf[j+1];
                buf[j+1] = tmp;
            }
    return buf[N/2]; // 返回中位数
}