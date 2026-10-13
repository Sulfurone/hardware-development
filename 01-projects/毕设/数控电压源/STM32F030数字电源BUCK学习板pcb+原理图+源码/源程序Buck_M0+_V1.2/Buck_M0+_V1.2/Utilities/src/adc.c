#include "adc.h"
#include "delay.h"

#include "calibration.h"


_uADC_Type	power;

/**
  * @brief  ADC1 configuration
  * @param  None
  * @retval None
  */
void ADC_Config(void)
{
	ADC_InitTypeDef     ADC_InitStructure;
	GPIO_InitTypeDef    GPIO_InitStructure;
	DMA_InitTypeDef		DMA_InitStructure;
	
	/* ADC1 DeInit */  
	ADC_DeInit(ADC1);

	/* GPIOA Periph clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	/* ADC1 Periph clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	ADC_ClockModeConfig(ADC1, ADC_ClockMode_SynClkDiv4);
	
	 /* DMA1 clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);
	
	/* TIM1 Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);

	/* Configure ADC channel0、channel1、channel2 and channel3 as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* DMA1 Channel1 Config */
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr	= (uint32_t)ADC1_DR_ADDRESS;
	DMA_InitStructure.DMA_MemoryBaseAddr		= (uint32_t)&power.channel[0];
	DMA_InitStructure.DMA_DIR					= DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize			= 3;
	DMA_InitStructure.DMA_PeripheralInc			= DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc				= DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize	= DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize		= DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode					= DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority				= DMA_Priority_High;
	DMA_InitStructure.DMA_M2M					= DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	/* Initialize ADC structure */
	ADC_StructInit(&ADC_InitStructure);
	/* Configure the ADC1 in continuous mode withe a resolution equal to 12 bits  */
	ADC_InitStructure.ADC_Resolution			= ADC_Resolution_12b;
	ADC_InitStructure.ADC_ContinuousConvMode	= ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge	= ADC_ExternalTrigConvEdge_Falling;
	ADC_InitStructure.ADC_ExternalTrigConv		= ADC_ExternalTrigConv_T1_CC4;
	ADC_InitStructure.ADC_DataAlign				= ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ScanDirection			= ADC_ScanDirection_Upward;
	ADC_Init(ADC1, &ADC_InitStructure); 
	
	ADC_OverrunModeCmd(ADC1, ENABLE); 

	ADC_ChannelConfig(ADC1, ADC_Channel_1, ADC_SampleTime_7_5Cycles);	//输入电压
	ADC_ChannelConfig(ADC1, ADC_Channel_2, ADC_SampleTime_7_5Cycles);	//输出电流
	ADC_ChannelConfig(ADC1, ADC_Channel_3, ADC_SampleTime_7_5Cycles);	//输出电压
	
	/* ADC Calibration */
	ADC_GetCalibrationFactor(ADC1);

	/* ADC DMA request in circular mode */
	ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);

	/* Enable ADC_DMA */
	ADC_DMACmd(ADC1, ENABLE);  

	/* Enable the ADC peripheral */
	ADC_Cmd(ADC1, ENABLE);     

	/* Wait the ADRDY flag */
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); 

	/* ADC1 regular Software Start Conv */ 
	ADC_StartOfConversion(ADC1);
}

/**
  * @brief  This function is...
  * @param  None
  * @retval None
  */
void ADC_GetNewSample(void)
{
	#if 1
	uint16_t i;
	float sum[3]={0.0f,0.0f,0.0f};
	
	for(i=PW_ADC_SAMPLE_LEN-1; i>0; i--)
	{
		power.inV[i]  = power.inV[i-1];
		power.outI[i] = power.outI[i-1];
		power.outV[i] = power.outV[i-1];
		sum[0] += power.inV[i];
		sum[1] += power.outI[i];
		sum[2] += power.outV[i];
	}
	
	power.inV[0]  = power.channel[0]; //PA1->输入电压
	power.outI[0] = power.channel[1]; //PA2->输出电流
	power.outV[0] = power.channel[2]; //PA3->输出电压
	sum[0] += power.inV[0];
	sum[1] += power.outI[0];
	sum[2] += power.outV[0];
	
	power.Vi = (sum[0] / PW_ADC_SAMPLE_LEN) * calib.viRatio;
	power.Io = (sum[1] / PW_ADC_SAMPLE_LEN - calib.ioZero) * calib.ioRatio;
	power.Vo = (sum[2] / PW_ADC_SAMPLE_LEN) * calib.voRatio;

	#endif
	
	#if 0
	float toV[3]={0.0f,0.0f,0.0f};
	
	toV[0] = power.channel[0] * calib.viRatio;
	toV[1] = (power.channel[1] - calib.ioZero) * calib.voRatio;
	toV[2] = power.channel[2] * calib.voRatio;
	
	
	power.Vi = 0.8f*power.Vi + 0.2f*toV[0];
	power.Io = 0.8f*power.Io + 0.2f*toV[1];
	power.Vo = 0.8f*power.Vo + 0.2f*toV[2];
	#endif
	
	if(power.Io<0)
	{
		power.Io = 0;
	}
	
	/* 大电流补偿压差 */
	power.Vo -= power.Io * 0.208f;
	if(power.Vo<0)
	{
		power.Vo = 0;
	}
	/* */
}











