#include "LDR.h"

/*****************辰哥单片机设计******************
											STM32
 * 文件			:	光敏传感器c文件                   
 * 版本			: V1.0
 * 日期			: 2024.8.9
 * MCU			:	STM32F103C8T6
 * 接口			:	见代码							
 * BILIBILI	:	辰哥单片机设计
 * CSDN			:	辰哥单片机设计
 * 作者			:	辰哥

**********************BEGIN***********************/

void LDR_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd (LDR_GPIO_CLK, ENABLE );	// 打开 ADC IO端口时钟
	GPIO_InitStructure.GPIO_Pin = LDR_GPIO_PIN;					// 配置 ADC IO 引脚模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		// 设置为模拟输入
	
	GPIO_Init(LDR_GPIO_PORT, &GPIO_InitStructure);				// 初始化 ADC IO

	ADCx_Init();
	
}
	
uint16_t LDR_ADC_Read(void)
{
    return ADC_GetValue();
}
	
uint16_t LDR_Average_Data(void)
{
	uint32_t  tempData = 0;
	for (uint8_t i = 0; i < LDR_READ_TIMES; i++)
	{
		tempData += LDR_ADC_Read();
		delay_ms(5);
	}

	tempData /= LDR_READ_TIMES;
	return (uint16_t)tempData;
}

uint16_t LDR_LuxData()
{
	float voltage = 0;	
	float R = 0;	
	uint16_t Lux = 0;
	voltage = LDR_Average_Data();
	voltage  = voltage / 4096 * 3.3f;
	
	R = voltage / (3.3f - voltage) * 10000;
		
	Lux = 40000 * pow(R, -0.6021);
	
	if (Lux > 999)
	{
		Lux = 999;
	}
	return Lux;
}

//void LDR_LuxData(uint16_t *Lux)
//{
//	float voltage = 0;	
//	float R = 0;	
//	voltage = LDR_Average_Data();
//	voltage  = voltage / 4096 * 3.3f;
//	
//	R = voltage / (3.3f - voltage) * 10000;
//		
//	*Lux = 40000 * pow(R, -0.6021);
//	
//	if (*Lux > 999)
//	{
//		*Lux = 999;
//	}
//}

