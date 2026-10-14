#include "stm32f10x.h"
#include "stdio.h"
#include "common.h"

static u8  fac_us=0; //usʱ
static u16 fac_ms=0; //msʱ


/*******************************************************************************
*        :  
*        : Printfִ֧   printf("aֵ: %d\r\n\r\n",a);
* ڲ       :
* ڲ       :
*******************************************************************************/
#if 1
#pragma import(__use_no_semihosting)             
                
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
 
void _sys_exit(int x) //_sys_exit()Աʹðģʽ   
{ 
	x = x; 
} 

int fputc(int ch, FILE *f)//ضfputc 
{      
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); 
    USART_SendData(USART1,(uint8_t)ch);   
	return ch;
}
#endif 

/*******************************************************************************
*        : Delay_Init
*        : ʱʼ
* ڲ       :
* ڲ       :
*******************************************************************************/
void Delay_Init(void)
{
	SysTick->CTRL&=0xfffffffb;//bit2,ѡⲿʱ  HCLK/8
	fac_us=9;		    
	fac_ms=(u16)fac_us*1000;
}								    

/*******************************************************************************
*        : DelayMs
*        : ʱ72Mms<=1864
* ڲ       : ms ʱС
* ڲ       :
*******************************************************************************/
void Delay_ms(u16 ms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)ms*fac_ms; //ʱ(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;            //ռ
	SysTick->CTRL=0x01 ;           //ʼ  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16))); //ȴʱ䵽   
	SysTick->CTRL=0x00;                //رռ
	SysTick->VAL =0X00;                //ռ	  	    
}   

/*******************************************************************************
*        : DelayUs
*        : ʱ72Mms<=1864000
* ڲ       : us ʱС
* ڲ       :
*******************************************************************************/
void Delay_us(u32 us)
{		
	u32 temp;	    	 
	SysTick->LOAD=us*fac_us; //ʱ	  		 
	SysTick->VAL=0x00;        //ռ
	SysTick->CTRL=0x01 ;      //ʼ 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16))); //ȴʱ䵽   
	SysTick->CTRL=0x00;                //رռ
	SysTick->VAL =0X00;                //ռ	 
}

/*******************************************************************************
*        : USART_Config
*        : 
* ڲ       : USARTx ں 
                   Baud   
* ڲ       :
*******************************************************************************/
void USART_Config(USART_TypeDef* USARTx,u32 Baud)
{
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate =Baud;	//
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //λ8λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //ֹͣλ1λ
	USART_InitStructure.USART_Parity = USART_Parity_No; //Уλ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //Ӳ
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //շģʽ

	USART_Init(USARTx, &USART_InitStructure);	//ôڲ
	
#ifdef EN_USART_RX
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);  //ʹܽж
#endif
	//USART_ITConfig(USARTx, USART_IT_TXE, ENABLE); //ʹܷͻж
	//USART_ITConfig(USART1, USART_IT_TC, ENABLE);  //ʹܷж

	/* Enable the USART1 */
	USART_Cmd(USARTx, ENABLE); //ʹܴ	
	/*CPUСȱݣúãֱSend1ֽڷͲȥ
	1ֽ޷ȷͳȥ  */
	USART_ClearFlag(USARTx, USART_FLAG_TXE);/* 巢ɱ־Transmission Complete flag */ 
}


/*******************************************************************************
*        : UartSendData
*        : һֽ
* ڲ       : USARTx ںţ
* ڲ       :
*******************************************************************************/
u8 UartSendData(USART_TypeDef* USARTx,u8 ch)
{
	/* Write a character to the USART */
	USART_SendData(USARTx, ch);
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET){}
	return ch;
}

//һַ
void UartSendString(USART_TypeDef* USARTx,char *str)
{

	 while(*str!='\0')
	{
		USART_SendData(USARTx, *str++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET){}	
	}
}	


