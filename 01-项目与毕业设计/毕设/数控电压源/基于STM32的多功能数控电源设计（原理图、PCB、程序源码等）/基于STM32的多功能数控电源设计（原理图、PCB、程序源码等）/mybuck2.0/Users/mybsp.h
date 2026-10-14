#ifndef _MYBSP_H_
#define _MYBSP_H_


#include <stdio.h>
#include <string.h>
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_msp.h"
#include "mylightos.h"
#include "mypid.h"
#include "myuartLCD.h"

extern void MY_BSP_Init_All(void);
extern void Toggle_Leds(PVOID pvParams);
extern void Error_Handler(void);
extern void MY_BSP_Init_Uart1(uint32_t BaudRate);
extern void MY_BSP_Init_ADC1(void);
extern void MY_BSP_Init_HRTIM_OUTA(uint32_t A_Fre,uint16_t Risingtime,uint16_t Fallingtime);
extern void MY_BSP_A_outputs_Config(void);
extern uint16_t Set_vol;
/* 私有宏定义 ----------------------------------------------------------------*/
#define UARTBUFF_SIZE              200  // 串口DMA发送缓冲区大小
extern AT_MSG_QUENE_T ATrecQue;
extern UART_HandleTypeDef UartHandle;
extern void MY_BSP_Init_Uart1(uint32_t BaudRate);
extern uint16_t uart1RxCnt;
extern uint8_t rxchar;
extern uint8_t aRxBuffer[UARTBUFF_SIZE];
extern void Uart1_Send(uint8_t *aTxBuffer);	
/* Private typedef -----------------------------------------------------------*/
#define ADC_VREF ((uint16_t)3300) /* Vref+ input voltage in mV */
extern ADC_HandleTypeDef Adc1Handle;
extern HRTIM_HandleTypeDef hhrtimA;
extern uint8_t MY_BSP_Get_HRTIM_OUTA_Duty(void);
extern void MY_BSP_Set_HRTIM_OUTA_Duty(uint8_t A_Duty);
extern void MY_BSP_Set_HRTIM_OUTA_Fre(uint32_t A_Fre);
extern void MY_BSP_Set_HRTIM_OUTA_DutyHEX(uint32_t A_DutyHEX);
extern void LED_Init(void);
extern void LED1_On(void);
extern void LED1_Off(void);
extern void LED1_Toggle(void);

extern void LED2_On(void);
extern void LED2_Off(void);
extern void LED2_Toggle(void);
#endif
