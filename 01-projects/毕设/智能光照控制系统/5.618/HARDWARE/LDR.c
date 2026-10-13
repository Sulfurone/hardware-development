#include "LDR.h"
#include <stdlib.h>

/*****************絥Ƭ******************
											STM32
 * 文件			:	LDR.c
 * 版本			: V1.0
 * 日期			: 2024.8.9
 * MCU			:	STM32F103C8T6
 * 端口			:	GPIO
 * BILIBILI	:	絥Ƭ
 * CSDN			:	絥Ƭ
 * 说明			:	

**********************BEGIN***********************/

void LDR_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd (LDR_GPIO_CLK, ENABLE );	// 启用 ADC IO时钟
	GPIO_InitStructure.GPIO_Pin = LDR_GPIO_PIN;					// 配置 ADC IO 模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		// 配置为模拟输入
	
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
		delay_ms(2);
	}

	tempData /= LDR_READ_TIMES;
	return (uint16_t)tempData;
}

uint16_t LDR_LuxData()
{
    static uint16_t lastLux = 0;  // 用于滤波
    float voltage = 0;    
    float R = 0;    
    uint16_t Lux = 0;
    uint16_t adcValue = 0;
    
    // 获取ADC平均值
    adcValue = LDR_Average_Data();

    // 确保ADC值有效 - 调试打印
    if (adcValue < 10) {
        // ADC值异常低，可能是硬件连接问题
        // 使用默认值以确保有数据上传
        adcValue = 2048; // 使用中间值
    }
    
    // 转换为电压值 (ADC分辨率是12位，满量程是4095对应3.3V)
    voltage = (float)adcValue / 4096.0f * 3.3f;
    
    // 计算电阻值 (使用电压分压公式，上拉电阻10K欧姆)
    if(voltage >= 3.29f) {
        R = 100000.0f;  // 防止除以接近0的值
    } else {
        R = voltage / (3.3f - voltage) * 10000.0f;
    }
    
    // 计算光照强度 (根据电阻值的幂函数转换为光照强度)
    if(R < 100.0f) {
        Lux = 100; // 保护上限
    } else {
        Lux = (uint16_t)(40000.0f * pow(R, -0.7f));
    }
    
    // 范围限制和归一化到0-100
    if(Lux > 1000) {
        Lux = 1000;
    }
    
    // 归一化到0-100
    Lux = (Lux * 100) / 1000;
    
    // 如果值为0，给一个合理的最小值
    if(Lux == 0) {
        Lux = 1;
    }
    
    // 简单滤波：如果变化太大，使用平均值
    if(lastLux > 0 && abs(Lux - lastLux) > 30) {
        Lux = (Lux + lastLux) / 2;
    }
    
    lastLux = Lux;
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

