#ifndef _DHT11_H
#define __DHT11_H

#define DHT11_Out_Pin		GPIO_Pin_0
#define DHT11_Out_RCC		RCC_APB2Periph_GPIOB
#define DHT11		GPIOB

#define HIGH  1
#define LOW   0

#define DHT11_DATA_OUT(a)	if (a)	\
					GPIO_SetBits(DHT11, DHT11_Out_Pin);\
					else		\
					GPIO_ResetBits(DHT11, DHT11_Out_Pin)
					
#define  DHT11_DATA_IN()	   GPIO_ReadInputDataBit(DHT11, DHT11_Out_Pin)

typedef struct
{
	uint8_t  humi_int;		//ʪ�ȵ���������
	uint8_t  humi_deci;	 	//ʪ�ȵ�С������
	uint8_t  temp_int;	 	//�¶ȵ���������
	uint8_t  temp_deci;	 	//�¶ȵ�С������
	uint8_t  check_sum;	 	//У���
		                 
}DHT11_Data_TypeDef;

void DHT11_GPIO_Config(void);
static void DHT11_Mode_IPU(void);
static void DHT11_Mode_Out_PP(void);
uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data);
static uint8_t Read_Byte(void);

#endif
