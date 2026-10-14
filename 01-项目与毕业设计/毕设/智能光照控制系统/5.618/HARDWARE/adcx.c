#include "adcx.h"

/**
  * @brief  ADC初始化
  * @param  
  * @retval 
  */
void ADCx_Init(void)
{
	// 使能GPIOB和ADC1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(ADC_CLK, ENABLE);
	
	// 配置PB1为模拟输入
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// 配置ADC时钟为PCLK2的6分频
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	// ADC初始化
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	// 独立模式
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	// 数据右对齐
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	// 外部触发选择：禁止
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	// 禁止连续转换
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	// 禁止扫描模式
	ADC_InitStructure.ADC_NbrOfChannel = 1;	// 转换的通道数：1
	ADC_Init(ADCx, &ADC_InitStructure);	// 初始化ADC1
	
	// 使能ADC
	ADC_Cmd(ADCx, ENABLE);
	
	// 校准ADC
	ADC_ResetCalibration(ADCx);
	while(ADC_GetResetCalibrationStatus(ADCx) == SET);
	ADC_StartCalibration(ADCx);
	while(ADC_GetCalibrationStatus(ADCx) == SET);	
}

/**
  * @brief  获取ADC转换
  * @param  ADC_Channel 	选择要转换的ADC通道
  * @param  ADC_SampleTime 	选择要转换的时间
  * @retval 转换后的数据
  */
u16 ADC_GetValue(void)
{
	// 配置ADC1的规则通道9（PB1对应的通道是ADC_Channel_9）
	// STM32F103中PB1对应ADC通道9
	ADC_RegularChannelConfig(ADCx, ADC_Channel_9, 1, ADC_SampleTime_239Cycles5);
	
	// 开始ADC转换
	ADC_SoftwareStartConvCmd(ADCx, ENABLE);
	
	// 等待转换完成
	while(ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC) == RESET);
	
	// 清除EOC标志
	ADC_ClearFlag(ADCx, ADC_FLAG_EOC);
	
	// 返回转换结果
	return ADC_GetConversionValue(ADCx);
}
