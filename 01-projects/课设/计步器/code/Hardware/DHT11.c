#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "DHT11.h"


void DHT11_GPIO_Config(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(DHT11_Out_RCC, ENABLE); 
  	GPIO_InitStructure.GPIO_Pin = DHT11_Out_Pin;	
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  	GPIO_Init(DHT11, &GPIO_InitStructure);		  

	GPIO_SetBits(DHT11, DHT11_Out_Pin);	 
}

static void DHT11_Mode_IPU(void)
{
 	  GPIO_InitTypeDef GPIO_InitStructure;
	  GPIO_InitStructure.GPIO_Pin = DHT11_Out_Pin;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 
	  GPIO_Init(DHT11, &GPIO_InitStructure);	 
}

static void DHT11_Mode_Out_PP(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;

  	GPIO_InitStructure.GPIO_Pin = DHT11_Out_Pin;	

  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  	GPIO_Init(DHT11, &GPIO_InitStructure);	 	 
}

static uint8_t Read_Byte(void)
{
	uint8_t i, temp=0;

	for (i=0; i<8; i++)    
	{	 
		while (DHT11_DATA_IN() == Bit_RESET);

		Delay_us(40);    	  

		if (DHT11_DATA_IN() == Bit_SET)
		{
			while(DHT11_DATA_IN() == Bit_SET);

			temp |= (uint8_t)(0x01 << (7 - i));
		}
		else
		{			   
			temp &= (uint8_t) ~ (0x01<<(7-i)); 
		}
	}
	return temp;
}

uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data)
{
	uint8_t retry = 0;
	
	// 最多尝试3次
	while(retry < 3)
	{
		DHT11_Mode_Out_PP();
		DHT11_DATA_OUT(LOW);
		Delay_ms(20);  // 增加拉低时间到20ms

		DHT11_DATA_OUT(HIGH); 

		Delay_us(35);  // 增加拉高时间到35us

		DHT11_Mode_IPU();

		// 等待DHT11响应，增加超时检测
		uint8_t timeout = 0;
		while(DHT11_DATA_IN() == Bit_SET)
		{
			timeout++;
			Delay_us(1);
			if(timeout > 100) break;
		}
		
		if(DHT11_DATA_IN() == Bit_RESET)     
		{
			timeout = 0;
			while(DHT11_DATA_IN() == Bit_RESET)
			{
				timeout++;
				Delay_us(1);
				if(timeout > 100) break;
			}

			timeout = 0;
			while(DHT11_DATA_IN() == Bit_SET)
			{
				timeout++;
				Delay_us(1);
				if(timeout > 100) break;
			}

			DHT11_Data -> humi_int = Read_Byte();
			DHT11_Data -> humi_deci = Read_Byte();
			DHT11_Data -> temp_int = Read_Byte();
			DHT11_Data -> temp_deci = Read_Byte();
			DHT11_Data -> check_sum= Read_Byte();

			DHT11_Mode_Out_PP();
			DHT11_DATA_OUT(HIGH);

			if (DHT11_Data -> check_sum == DHT11_Data -> humi_int + DHT11_Data -> humi_deci + DHT11_Data -> temp_int + DHT11_Data -> temp_deci)
				return SUCCESS;
		}
		
		retry++;
		Delay_ms(100);  // 重试前延时
	}
	
	// 所有尝试都失败
	DHT11_Mode_Out_PP();
	DHT11_DATA_OUT(HIGH);
	return ERROR;
}

	 