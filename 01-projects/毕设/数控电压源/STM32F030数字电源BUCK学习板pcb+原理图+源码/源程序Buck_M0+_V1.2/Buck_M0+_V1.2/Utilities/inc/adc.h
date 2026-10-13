#ifndef __ADC_H
#define __ADC_H
#include "stm32f0xx.h"   


#define ADC1_DR_ADDRESS				0x40012440

#define PW_ADC_SAMPLE_LEN			16		//ADC样本空间长度

#define PW_VI_RATIO					((16.6667f*3.3f)/4096)
#define PW_VO_RATIO					((16.6667f*3.3f)/4096)
#define PW_IO_RATIO					((4.5833f*3.3f)/4096)


typedef struct  
{
	uint16_t channel[3];
	uint16_t inV[PW_ADC_SAMPLE_LEN];
	uint16_t outV[PW_ADC_SAMPLE_LEN];
	uint16_t outI[PW_ADC_SAMPLE_LEN];
	float Vi;
	float Vo;
	float Io;
}_uADC_Type;
extern _uADC_Type	power;


void ADC_Config(void);
void ADC_GetNewSample(void);

#endif
















