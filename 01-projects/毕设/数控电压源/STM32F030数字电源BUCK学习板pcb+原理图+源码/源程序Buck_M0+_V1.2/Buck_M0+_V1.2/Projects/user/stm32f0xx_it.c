/**
  ******************************************************************************
  * @file    ADC/ADC_BasicExample/stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    24-July-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"
#include "key.h"
#include "led.h"

#include "freeRTOS_task.h"

#if USE_USART_PRINTF
uint16_t USART_RX_STA=0;
uint8_t  USART_RX_BUF[USART_REC_LEN];
#endif

/** @addtogroup STM32F0xx_StdPeriph_Examples
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t buzzer_cnt=0;
uint8_t isOS=0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
	osSystickHandler();
}

/**
* @brief This function ...
*/
void TIM3_IRQHandler()
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update))            //判断发生update事件中断
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);     //清除update事件中断标志
		EC11Code_Scan();
		if(READ_BUZZER_STA!=Bit_RESET)
		{
			buzzer_cnt++;
			if(buzzer_cnt>BUZZER_SOUND_TIME)
			{
				buzzer_cnt = 0;
				BUZZER_OFF;
			}
		}
	}
}

/**
* @brief This function ...
*/
void TIM14_IRQHandler()
{
	if(TIM_GetITStatus(TIM14, TIM_IT_Update))            //判断发生update事件中断
	{
		TIM_ClearITPendingBit(TIM14, TIM_IT_Update);     //清除update事件中断标志
		myControlTask();
	}
}


#if 0
/**
  * @brief  This function ...
  * @param  None
  * @retval None
  */
void DMA1_Channel1_IRQHandler(void)
{
	static uint8_t ioState=0;
	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)
	{
		
		ioState = 1 - ioState;
		if(ioState==0)
		{
			GPIO_SetBits(GPIOB, GPIO_Pin_0);
		}
		else
		{
			GPIO_ResetBits(GPIOB, GPIO_Pin_0);
		}

		FilterConvData_Tab[0] = FilterConvData_Tab[0]*0.8f + RegularConvData_Tab[0]*0.2f;
		FilterConvData_Tab[1] = FilterConvData_Tab[1]*0.8f + RegularConvData_Tab[1]*0.2f;
		FilterConvData_Tab[2] = FilterConvData_Tab[2]*0.8f + RegularConvData_Tab[2]*0.2f;
		FilterConvData_Tab[3] = FilterConvData_Tab[3]*0.8f + RegularConvData_Tab[3]*0.2f;
		
		DMA_ClearITPendingBit(DMA1_IT_TC1);
	}
	DMA_ClearFlag(DMA1_FLAG_TC1);
}
#endif

#if USE_USART_PRINTF
/**
* @brief This function ...
*/
void USART1_IRQHandler(void)
{
	uint8_t res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		/* Read one byte from the receive data register */
		res = USART_ReceiveData(USART1);	//读取接收到的数据
		if((USART_RX_STA&0x80)==0)//接收未完成
		{
			if(USART_RX_STA&0x40)//接收到了0x0d
			{
				if(res!=0x0a) { USART_RX_STA = 0; }//接收错误,重新开始
				else { USART_RX_STA |= 0x80; }	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(res==0x0d) 
				{ 
					USART_RX_STA|=0x40; 
				}
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3F] = res;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))
					{
						USART_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}
		}
	}
}
#endif

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
