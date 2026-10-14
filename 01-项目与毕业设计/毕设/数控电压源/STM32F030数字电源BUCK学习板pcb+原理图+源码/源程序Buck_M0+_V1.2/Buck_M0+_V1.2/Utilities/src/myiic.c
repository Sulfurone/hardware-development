#include "myiic.h"
#include "delay.h"

#define IIC1_SDA_H		GPIOB->BSRR=GPIO_Pin_6
#define IIC1_SDA_L		GPIOB->BRR=GPIO_Pin_6
#define IIC1_SCL_H		GPIOB->BSRR=GPIO_Pin_7
#define IIC1_SCL_L		GPIOB->BRR=GPIO_Pin_7
#define SDA_IN			{GPIOB->MODER  &= ~(GPIO_MODER_MODER0 << 12);\
						 GPIOB->MODER |= (((uint32_t)GPIO_Mode_IN) << 12);}
#define SDA_OUT			{GPIOB->MODER  &= ~(GPIO_MODER_MODER0 << 12);\
						 GPIOB->MODER |= (((uint32_t)GPIO_Mode_OUT) << 12);}
#define READ_SDA		(GPIOB->IDR & GPIO_Pin_7)

#define MYIIC_DELAY		delay_us						 

						 
						 
/**
  * @brief  iic configuration
  * @param  None
  * @retval None
  */
void myIIC_Config(void)
{	
	GPIO_InitTypeDef	GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	/* SDA-PA6 SCL-PB7*/
	GPIO_SetBits(GPIOB, GPIO_Pin_6);
	GPIO_SetBits(GPIOB, GPIO_Pin_7);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}

/**
  * @brief  IIC start
  * @param  None
  * @retval None
  */
void iicDevStart(void)
{
	SDA_OUT;
	IIC1_SDA_H;	  	  
	IIC1_SCL_H;
	MYIIC_DELAY(1);
 	IIC1_SDA_L;
	MYIIC_DELAY(1);
	IIC1_SCL_L;
}	  

/**
  * @brief  IIC stop
  * @param  None
  * @retval None
  */
void iicDevStop(void)
{
	SDA_OUT;
	IIC1_SCL_L;
	IIC1_SDA_L;
 	MYIIC_DELAY(1);
	IIC1_SCL_H; 
	IIC1_SDA_H;
	MYIIC_DELAY(1);							   	
}

/**
  * @brief  IIC wait ack
  * @param  None
  * @retval 1->device ack fail
  * 		0->device ack success
  */
uint8_t iicDevWaitAck(void)
{
	uint8_t ucErrTime=0;
	SDA_IN;
	IIC1_SDA_H;
	MYIIC_DELAY(1);	   
	IIC1_SCL_H;
	MYIIC_DELAY(1);	 
	while(READ_SDA==Bit_SET)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			iicDevStop();
			return 1;
		}
	}
	IIC1_SCL_L;   
	return 0;  
} 

/**
  * @brief  IIC master ack
  * @param  None
  * @retval None
  */
void iicDevAck(void)
{
	IIC1_SCL_L;
	SDA_OUT;
	IIC1_SDA_L;
	MYIIC_DELAY(1);
	IIC1_SCL_H;
	MYIIC_DELAY(1);
	IIC1_SCL_L;
}

/**
  * @brief  IIC master not ack
  * @param  None
  * @retval None
  */
void iicDevNAck(void)
{
	IIC1_SCL_L;
	SDA_OUT;
	IIC1_SDA_H;
	MYIIC_DELAY(1);
	IIC1_SCL_H;
	MYIIC_DELAY(1);
	IIC1_SCL_L;
}					 				     

/**
  * @brief  IIC master send byte
  * @param  None
  * @retval None
  */
void iicDevSendByte(uint8_t txd)
{                        
	uint8_t t;   
	SDA_OUT; 	    
	IIC1_SCL_L;
	for(t=0;t<8;t++)
	{              
		if(txd&0x80) { IIC1_SDA_H; }
		else { IIC1_SDA_L; }
		txd<<=1; 	  
		MYIIC_DELAY(1);   
		IIC1_SCL_H;
		MYIIC_DELAY(1); 
		IIC1_SCL_L;	
		MYIIC_DELAY(1);
	}	 
} 	

/**
  * @brief  IIC master read byte
  * @param  1->ack, 0->not ack 
  * @retval Data
  */
uint8_t iicDevReceiveByte(uint8_t ack)
{
	uint8_t i,receive=0;
	SDA_IN;
	for(i=0;i<8;i++ )
	{
		IIC1_SCL_L; 
		MYIIC_DELAY(1);
		IIC1_SCL_H;
		receive<<=1;
		if(READ_SDA==Bit_SET) receive++;   
		MYIIC_DELAY(1); 
	}					 
	if (!ack)
		iicDevNAck();
	else
		iicDevAck();

	return receive;
}























