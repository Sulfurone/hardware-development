#include "usart.h"
#include "string.h"
#include  "main.h"
#include  <stdio.h>
/********************************************/
//波特率计算公式：
// N=时钟主频/波特率
// 如果 N<16  UCBRx=INT(N),UCBRF=0;
// 如果 N>16  UCBRx=INT(N/16),UCBRF=INT{[(N/16)-INT(N/16)]*16}
// UCBRS 取值见MSP432 USER'S GUIDE P731
// 或者 使用计算器 
// software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
/********************************************/

 u8   BuffFlag =0;
 u8   BUF[BUFSIZE]={0,0,0,0,0};
 
 int fputc(int ch, FILE *f)
{
	UART_transmitData(EUSCI_A1_BASE, ch & 0xFF);
	return ch;
}
//int fgetc(FILE *f)
//{
//	while(EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG !=
//	 UART_getInterruptStatus(
//		EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG));
//	return UART_receiveData(EUSCI_A0_BASE);
//}

 
//////////////////////////////////////////////////////
                /* 串口0 */
   //frequency  12M
const eUSCI_UART_Config uartConfig0 =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        1,                                      // BRDIV = 78
        10,                                       // UCxBRF = 2
        0,                                       // UCxBRS = 0
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
};
void Usart0tInit(void){
    /* Selecting P1.2 and P1.3 in UART mode */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
            GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Setting DCO to 12MHz */
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_3);

    /* Configuring UART Module */
    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig0);

    /* Enable UART module */
    MAP_UART_enableModule(EUSCI_A0_BASE);

    /* Enabling interrupts */
    MAP_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA0);
    MAP_Interrupt_enableSleepOnIsrExit();
    MAP_Interrupt_enableMaster();   

}

 void USART0PutChar(unsigned char ch)
	{ 
		UART_transmitData(EUSCI_A0_BASE, ch); //发送数据
  }

void USART0PutString(unsigned char *str){ 
 while((*str)!= '\0'){
  USART0PutChar(*str++);
   }
  }
/* EUSCI A0 UART ISR - Echoes data back to PC host */
void EUSCIA0_IRQHandler(void)
{
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A0_BASE);//中断接收标志

    MAP_UART_clearInterruptFlag(EUSCI_A0_BASE, status);//清除中断标志

    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)  //说明接收到数据
    {
			MAP_UART_transmitData(EUSCI_A0_BASE, MAP_UART_receiveData(EUSCI_A0_BASE));//将接收到的数据发送出去
    }

}

//////////////////////////////////////////////////////
             /*  串口1  */
   //frequency  12M
eUSCI_UART_Config   uartConfig1 =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        78,                                      // BRDIV = 78
        2,                                       // UCxBRF = 2
        0,                                       // UCxBRS = 0
        EUSCI_A_UART_NO_PARITY,                  // 无奇偶校验
        EUSCI_A_UART_LSB_FIRST,                  // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
};

void Usart1tInit(void){
    /* Selecting P2.2 and P2.3 in UART mode */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2,
            GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
	
    /* Configuring UART Module */
    MAP_UART_initModule(EUSCI_A1_BASE, &uartConfig1);

    /* Enable UART module */
    MAP_UART_enableModule(EUSCI_A1_BASE);

    /* Enabling interrupts */
    MAP_UART_enableInterrupt(EUSCI_A1_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA1);
    //MAP_Interrupt_enableSleepOnIsrExit();
    MAP_Interrupt_enableMaster();   
}
 
 void USART1PutChar(unsigned char ch)
	{ 
		UART_transmitData(EUSCI_A1_BASE, ch); //发送数据
  }

void USART1PutString(unsigned char *str){ 
 while((*str)!= '\0'){
  USART1PutChar(*str++);
   }
  }

/* EUSCI A0 UART ISR - Echoes data back to PC host */
void EUSCIA1_IRQHandler(void)
{ 
	  u8   rev ;
    uint32_t status = UART_getEnabledInterruptStatus(EUSCI_A1_BASE);//中断接收标志
    
    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)  //说明接收到数据
    {
			rev=UART_receiveData(EUSCI_A1_BASE);
			//USART1PutChar( UART_receiveData(EUSCI_A1_BASE));//将接收到的数据发送出去
			switch(rev)
            {
//                case 0x00:keyvalue=0;break;
//                case '1':keyvalue=1;break;
//                case '2':keyvalue=2;break;
//                case '3':keyvalue=3;break;
//                case '4':keyvalue=4;break;
//                case '5':keyvalue=5;break;
//                case 0x06:keyvalue=6;break;
//                case 0x07:keyvalue=7;break;
                
            }
            UART_clearInterruptFlag(EUSCI_A1_BASE, status);//清除中断标志
//		  if((BuffFlag&0x8000)==0){     //接收未完成	
//			if(BuffFlag&0x4000){          //接收到了0x0d				
//				if(rev!=0x0a)
//					BuffFlag=0;               //接收错误,重新开始
//				else 
//					BuffFlag|=0x8000;	        //接收完成了 
//			}
//			else{                        //还没收到0X0D					
//				 if(rev==0x0d)
//					 BuffFlag|=0x4000;
//				 else{
//					BUF[BuffFlag&0X3FFF]=rev ; //将收到的数据放入数组
//					 BuffFlag++;	               //数据长度计数加1
//					if(BuffFlag>BUFSIZE)
//					   BuffFlag=0;              //接收数据错误,重新开始接收	  
//				}		 
//			}
//		} 
			
//			
//			 	if((rev!=0X0D)||(rev!=0X0A)){
//			   BUF[BuffFlag]=rev;
//			   BuffFlag++;
//				 if(BuffFlag>=BUFSIZE){ 
//				 BuffFlag=0;
//				 }
//			     }
//				else if((rev==0X0D)||(rev==0X0A)){
//					BuffFlag=0; 
//				}

    }
}






















//////////////////////////////////////////////////////
























//////////////////////////////////////////////////////
