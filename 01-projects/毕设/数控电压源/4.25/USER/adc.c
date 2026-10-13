#include "stm32f10x.h"                  // Device header
#include "adc.h" // 推荐加上，保证宏和声明一致


#define AD_FILTER_LEN 10
extern uint16_t AD0, AD1, AD2, AD3;
uint16_t ad0_buffer[AD_FILTER_LEN] = {0};
uint8_t ad0_index = 0;

uint16_t ad1_buffer[AD_FILTER_LEN] = {0};
uint8_t ad1_index = 0;

uint16_t ad2_buffer[AD_FILTER_LEN] = {0};
uint8_t ad2_index = 0;

uint16_t ad3_buffer[AD_FILTER_LEN] = {0};
uint8_t ad3_index = 0;

extern float voltage;


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

uint16_t FilterADValue(uint16_t *buffer, uint8_t *index, uint16_t new_value) {
    buffer[*index] = new_value;
    (*index)++;
    if (*index >= AD_FILTER_LEN) *index = 0;
    uint32_t sum = 0;
    for (uint8_t i = 0; i < AD_FILTER_LEN; i++) {
        sum += buffer[i];
    }
    return sum / AD_FILTER_LEN;
}


