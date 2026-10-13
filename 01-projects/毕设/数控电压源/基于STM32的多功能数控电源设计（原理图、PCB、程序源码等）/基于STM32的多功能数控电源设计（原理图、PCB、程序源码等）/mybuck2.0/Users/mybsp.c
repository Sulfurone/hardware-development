#include "mybsp.h"
static void SystemClock_Config(void);
void MY_BSP_Init_All(void)
{
  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
  HAL_Init();
  /* Configure the system clock to 64 MHz */
  SystemClock_Config();
	BSP_ST_Timer_Init();
	
	LED_Init();
	BSP_ST_Timer_Set(Toggle_Leds, 100000, NULL, TRUE, TRUE, TIMERID_LED);//LED闪烁任务
	MY_BSP_Init_Uart1(115200);
}
#define LED_GPIO_CLOCK						RCC_AHBPeriph_GPIOB
#define LED_GPIO_PORT             GPIOB
#define LED1_GPIO_PIN             GPIO_PIN_3
#define LED2_GPIO_PIN             GPIO_PIN_4

void LED_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  __HAL_RCC_GPIOB_CLK_ENABLE();

	
  GPIO_InitStruct.Pin = LED1_GPIO_PIN | LED2_GPIO_PIN;
  GPIO_InitStruct.Mode =GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;  
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;;  
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	LED1_Off();
	LED2_Off();
}
void LED1_On(void)
{
	 LED_GPIO_PORT->BSRR = LED1_GPIO_PIN;
}
void LED1_Off(void)
{
  LED_GPIO_PORT->BRR = LED1_GPIO_PIN;
}
void LED1_Toggle(void)
{
  LED_GPIO_PORT->ODR ^= LED1_GPIO_PIN;
}
void LED2_On(void)
{
	 LED_GPIO_PORT->BSRR = LED2_GPIO_PIN;
}
void LED2_Off(void)
{
  LED_GPIO_PORT->BRR = LED2_GPIO_PIN;
}
void LED2_Toggle(void)
{
  LED_GPIO_PORT->ODR ^= LED2_GPIO_PIN;
}
/**
  * @brief  Toggle LEDs
  * @param  None
  * @retval None
  */
void Toggle_Leds(PVOID pvParams)
{
	LED2_Toggle(); 	
}
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* User may add here some code to deal with this error */
  while(1)
  {
		LED1_On();
  }
}
/* UART handler declaration */
UART_HandleTypeDef UartHandle;
uint8_t aRxBuffer[UARTBUFF_SIZE] = {0};//数据接收缓存
uint8_t rxchar = '\0';
uint16_t uart1RxCnt = 0;
AT_MSG_QUENE_T ATrecQue;
void MY_BSP_Init_Uart1(uint32_t BaudRate)
{
	
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART1 configured as follow:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = None
      - BaudRate = 9600 baud
      - Hardware flow control disabled (RTS and CTS signals) */
  UartHandle.Instance          = USARTx;
  
  UartHandle.Init.BaudRate     = BaudRate;
  UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits     = UART_STOPBITS_1;
  UartHandle.Init.Parity       = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode         = UART_MODE_TX_RX;
  UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;	
	
	if(HAL_UART_Init(&UartHandle) != HAL_OK)
  {
    Error_Handler();
  }
	
	  /* 使能接收，进入中断回调函数 */
  while(HAL_UART_Receive_DMA(&UartHandle,(uint8_t *)aRxBuffer,1));
	
}
void Uart1_Send(uint8_t *aTxBuffer)
{
	while(HAL_UART_Transmit_DMA(&UartHandle, aTxBuffer, strlen((char *)aTxBuffer))!= HAL_OK);
}
void Uart1RxTimeout(PVOID pvParams)
{
	if(uart1RxCnt)
	{
		EnQueueMsg(&ATrecQue,(AT_MSG_SINGLE*)aRxBuffer);
		memset(aRxBuffer,0,strlen((const char *)aRxBuffer));
		uart1RxCnt = 0;		
	}
}
/**
  * 函数功能: 串口接收完成回调函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	__HAL_UART_FLUSH_DRREGISTER(UartHandle); // Clear the buffer to prevent overrun
	BSP_ST_Timer_Set(Uart1RxTimeout, 5000, NULL, FALSE, TRUE, TIMERID_UART1_RX);
	aRxBuffer[uart1RxCnt]= rxchar;
	uart1RxCnt++;	
	while(HAL_UART_Receive_DMA(UartHandle,&rxchar,1)!=HAL_OK);//准备接受下一个										
}
/**
  * @brief  This function handles DMA RX interrupt request.
  * @param  None
  * @retval None   
  */
void USARTx_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(UartHandle.hdmarx);
}

/**
  * @brief  This function handles DMA TX interrupt request.
  * @param  None
  * @retval None  
  */
void USARTx_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(UartHandle.hdmatx);
}

/**
  * @brief  This function handles USARTx interrupt request.
  * @param  None
  * @retval None
  */
void USARTx_IRQHandler(void)
{
  HAL_UART_IRQHandler(&UartHandle);
}

ADC_HandleTypeDef Adc1Handle;
/**
  * @brief  Configure ADC1 for being used with HRTIM
  * @param  None
  * @retval None
  */
void MY_BSP_Init_ADC1(void)
{
  ADC_MultiModeTypeDef MultiModeConfig;
  ADC_InjectionConfTypeDef InjectionConfig;

  Adc1Handle.Instance = ADC1;

  /* ADC2 is working independently */
  MultiModeConfig.DMAAccessMode = ADC_DMAACCESSMODE_DISABLED;
  MultiModeConfig.Mode = ADC_MODE_INDEPENDENT;
  MultiModeConfig.TwoSamplingDelay = ADC_TWOSAMPLINGDELAY_1CYCLE;
  HAL_ADCEx_MultiModeConfigChannel(&Adc1Handle, &MultiModeConfig);
  
  /* ADC2 global initialization */
  /* 12-bit right-aligned format, discontinuous scan mode, running from PLL */
  Adc1Handle.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  Adc1Handle.Init.Resolution = ADC_RESOLUTION_12B;
  Adc1Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  Adc1Handle.Init.ScanConvMode = ENABLE;
  Adc1Handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  Adc1Handle.Init.LowPowerAutoWait = DISABLE;
  Adc1Handle.Init.ContinuousConvMode = DISABLE;
  Adc1Handle.Init.NbrOfConversion = 1;
  Adc1Handle.Init.DiscontinuousConvMode = DISABLE;
  Adc1Handle.Init.NbrOfDiscConversion = 1;
  Adc1Handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  Adc1Handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  Adc1Handle.Init.DMAContinuousRequests = DISABLE;
  Adc1Handle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  HAL_ADC_Init(&Adc1Handle);

  /* Discontinuous injected mode: 1st injected conversion for Vin on Ch2 */
  InjectionConfig.InjectedChannel = ADC_CHANNEL_2;
  InjectionConfig.InjectedRank = ADC_INJECTED_RANK_1;
  InjectionConfig.InjectedSamplingTime = ADC_SAMPLETIME_7CYCLES_5;
  InjectionConfig.InjectedSingleDiff = ADC_SINGLE_ENDED;
  InjectionConfig.InjectedOffsetNumber = ADC_OFFSET_NONE;
  InjectionConfig.InjectedOffset = 0;
  InjectionConfig.InjectedNbrOfConversion = 3;
  InjectionConfig.InjectedDiscontinuousConvMode = DISABLE;
  InjectionConfig.AutoInjectedConv = DISABLE;
  InjectionConfig.QueueInjectedContext = DISABLE;
  InjectionConfig.ExternalTrigInjecConv = ADC_EXTERNALTRIGINJECCONV_HRTIM_TRG2;
  InjectionConfig.ExternalTrigInjecConvEdge = ADC_EXTERNALTRIGINJECCONV_EDGE_RISING;
  HAL_ADCEx_InjectedConfigChannel(&Adc1Handle, &InjectionConfig);
	
  /* Discontinuous injected mode: 1st injected conversion for Vin on Ch2 */
  InjectionConfig.InjectedChannel = ADC_CHANNEL_4;
  InjectionConfig.InjectedRank = ADC_INJECTED_RANK_2;
  InjectionConfig.InjectedSamplingTime = ADC_SAMPLETIME_7CYCLES_5;
  HAL_ADCEx_InjectedConfigChannel(&Adc1Handle, &InjectionConfig);	
	
	 /* Discontinuous injected mode: 1st injected conversion for Vin on Ch1 */
  InjectionConfig.InjectedChannel = ADC_CHANNEL_1;
  InjectionConfig.InjectedRank = ADC_INJECTED_RANK_3;
  InjectionConfig.InjectedSamplingTime = ADC_SAMPLETIME_7CYCLES_5;
  HAL_ADCEx_InjectedConfigChannel(&Adc1Handle, &InjectionConfig);	
  
  
  /* Run the ADC calibration in single-ended mode */
  HAL_ADCEx_Calibration_Start(&Adc1Handle, ADC_SINGLE_ENDED);

  /* Start ADC2 Injected Conversions */ 
  HAL_ADCEx_InjectedStart(&Adc1Handle);
  
	
//	  /* Configure and enable ADC1_2_IRQHandler interrupt channel in NVIC */
//  HAL_NVIC_SetPriority(ADC1_2_IRQn, TICK_INT_PRIORITY, 0u);
//  HAL_NVIC_EnableIRQ(ADC1_2_IRQn);

//  /* Run the ADC calibration in single-ended mode */
//  HAL_ADCEx_Calibration_Start(&Adc1Handle, ADC_SINGLE_ENDED);

//  /* Start ADC1 Injected Conversions */
//  HAL_ADCEx_InjectedStart_IT(&Adc1Handle);
}
///**
//* @brief  This function handles ADC1_2 interrupt request.
//* @param  None
//* @retval None
//*/
//void ADC1_2_IRQHandler(void)
//{
//  /* Clear ADC interrupt flag */
//  __HAL_ADC_CLEAR_FLAG(&Adc1Handle, ADC_FLAG_JEOS);
//	Outvolvalue = (double)HAL_ADCEx_InjectedGetValue(&Adc1Handle, ADC_INJECTED_RANK_1)* ADC_VREF/0xFFF/1000*30;

//}
HRTIM_HandleTypeDef hhrtimA;
/* Exported constants ---------------------------------------------------------
#define PWM_PERIOD = 144000000*32/switchfrequency
#define DT_RISING = risingtime*switchfrequency*PWM_PERIOD
#define DT_FALLING = fallingtime*switchfrequency*PWM_PERIOD
-------------------------------------------------------------------------------*/
#define DEADTIME

/**
* @brief  HRTIM configuration,有死区双路,默认占空比50%
* @param  None
* @retval None
*/
void MY_BSP_Init_HRTIM_OUTA(uint32_t A_Fre,uint16_t Risingtime,uint16_t Fallingtime)
{
  HRTIM_TimeBaseCfgTypeDef timebase_config;
  HRTIM_TimerCfgTypeDef timer_config;
  HRTIM_OutputCfgTypeDef output_config_TA1_TA2;
  HRTIM_CompareCfgTypeDef compare_config;
#ifdef DEADTIME
  HRTIM_DeadTimeCfgTypeDef HRTIM_TIM_DeadTimeConfig;
#endif
  HRTIM_ADCTriggerCfgTypeDef adc_trigger_config;
#ifdef FAULTENABLE
  HRTIM_FaultCfgTypeDef fault_config;
#endif

  /* ----------------------------*/
  /* HRTIM Global initialization */
  /* ----------------------------*/
  /* Initialize the hrtim structure (minimal configuration) */
  hhrtimA.Instance = HRTIM1;
  hhrtimA.Init.HRTIMInterruptResquests = HRTIM_IT_NONE;
  hhrtimA.Init.SyncOptions = HRTIM_SYNCOPTION_NONE;

  /* Initialize HRTIM */
  HAL_HRTIM_Init(&hhrtimA);

  /* HRTIM DLL calibration: periodic calibration, set period to 14祍 */
  HAL_HRTIM_DLLCalibrationStart(&hhrtimA, HRTIM_CALIBRATIONRATE_14);
  /* Wait calibration completion*/
  if (HAL_HRTIM_PollForDLLCalibration(&hhrtimA, 100) != HAL_OK)
  {
    Error_Handler(); // if DLL or clock is not correctly set
  }
  
  /* --------------------------------------------------- */
  /* TIMERA initialization: timer mode and PWM frequency */
  /* --------------------------------------------------- */
  timebase_config.Period = 4096000000/A_Fre; /* 400kHz switching frequency */
  timebase_config.RepetitionCounter = 7; /* 1 ISR every 8 PWM periods */
  timebase_config.PrescalerRatio = HRTIM_PRESCALERRATIO_MUL32;
  timebase_config.Mode = HRTIM_MODE_CONTINUOUS;
  HAL_HRTIM_TimeBaseConfig(&hhrtimA, HRTIM_TIMERINDEX_TIMER_A, &timebase_config);

  
  /* --------------------------------------------------------------------- */
  /* TIMERA global configuration: cnt reset, sync, update, fault, burst... */
  /* timer running in continuous mode, with deadtime enabled               */
  /* --------------------------------------------------------------------- */
  timer_config.DMARequests = HRTIM_TIM_DMA_NONE;
  timer_config.DMASrcAddress = 0x0;
  timer_config.DMADstAddress = 0x0;
  timer_config.DMASize = 0x0;
  timer_config.HalfModeEnable = HRTIM_HALFMODE_DISABLED;
  timer_config.StartOnSync = HRTIM_SYNCSTART_DISABLED;
  timer_config.ResetOnSync = HRTIM_SYNCRESET_DISABLED;
  timer_config.DACSynchro = HRTIM_DACSYNC_NONE;
  timer_config.PreloadEnable = HRTIM_PRELOAD_ENABLED;
  timer_config.UpdateGating = HRTIM_UPDATEGATING_INDEPENDENT;
  timer_config.BurstMode = HRTIM_TIMERBURSTMODE_MAINTAINCLOCK;
  timer_config.RepetitionUpdate = HRTIM_UPDATEONREPETITION_ENABLED;
  timer_config.ResetUpdate = HRTIM_TIMUPDATEONRESET_DISABLED;
  timer_config.InterruptRequests = HRTIM_TIM_IT_NONE;
  timer_config.PushPull = HRTIM_TIMPUSHPULLMODE_DISABLED;
#ifdef FAULTENABLE
  timer_config.FaultEnable = HRTIM_TIMFAULTENABLE_FAULT1;
  timer_config.FaultLock = HRTIM_TIMFAULTLOCK_READWRITE;
#endif
#ifdef DEADTIME
  timer_config.DeadTimeInsertion = HRTIM_TIMDEADTIMEINSERTION_ENABLED;
#else
	timer_config.DeadTimeInsertion = HRTIM_TIMDEADTIMEINSERTION_DISABLED;
#endif
  timer_config.DelayedProtectionMode = HRTIM_TIMER_A_B_C_DELAYEDPROTECTION_DISABLED;
  timer_config.UpdateTrigger= HRTIM_TIMUPDATETRIGGER_NONE;
  timer_config.ResetTrigger = HRTIM_TIMRESETTRIGGER_NONE;
  HAL_HRTIM_WaveformTimerConfig(&hhrtimA, HRTIM_TIMERINDEX_TIMER_A, &timer_config);
    
  /* --------------------------------- */
  /* TA1 and TA2 waveforms description */
  /* --------------------------------- */
  output_config_TA1_TA2.Polarity = HRTIM_OUTPUTPOLARITY_HIGH;
  output_config_TA1_TA2.SetSource = HRTIM_OUTPUTSET_TIMPER;
  output_config_TA1_TA2.ResetSource  = HRTIM_OUTPUTRESET_TIMCMP1;
  output_config_TA1_TA2.IdleMode = HRTIM_OUTPUTIDLEMODE_NONE;
  output_config_TA1_TA2.IdleLevel = HRTIM_OUTPUTIDLELEVEL_INACTIVE;
  output_config_TA1_TA2.FaultLevel = HRTIM_OUTPUTFAULTLEVEL_INACTIVE;
  output_config_TA1_TA2.ChopperModeEnable = HRTIM_OUTPUTCHOPPERMODE_DISABLED;
  output_config_TA1_TA2.BurstModeEntryDelayed = HRTIM_OUTPUTBURSTMODEENTRY_REGULAR;
  HAL_HRTIM_WaveformOutputConfig(&hhrtimA,
                                 HRTIM_TIMERINDEX_TIMER_A,
                                 HRTIM_OUTPUT_TA1,
                                 &output_config_TA1_TA2);
  HAL_HRTIM_WaveformOutputConfig(&hhrtimA,
                                 HRTIM_TIMERINDEX_TIMER_A,
                                 HRTIM_OUTPUT_TA2,
                                 &output_config_TA1_TA2);																 

  /* Set compare registers for duty cycle on TA1 */
  compare_config.CompareValue = 4096000000/A_Fre/20;  /* 5% duty cycle */
  HAL_HRTIM_WaveformCompareConfig(&hhrtimA,
                                  HRTIM_TIMERINDEX_TIMER_A,
                                  HRTIM_COMPAREUNIT_1,
                                  &compare_config);
#ifdef DEADTIME
  HRTIM_TIM_DeadTimeConfig.FallingLock = HRTIM_TIMDEADTIME_FALLINGLOCK_WRITE;
  HRTIM_TIM_DeadTimeConfig.FallingSign = HRTIM_TIMDEADTIME_FALLINGSIGN_POSITIVE;
  HRTIM_TIM_DeadTimeConfig.FallingSignLock = HRTIM_TIMDEADTIME_FALLINGSIGNLOCK_READONLY;
  HRTIM_TIM_DeadTimeConfig.FallingValue =Risingtime*4096/1000;
  HRTIM_TIM_DeadTimeConfig.Prescaler = HRTIM_TIMDEADTIME_PRESCALERRATIO_MUL8;
  HRTIM_TIM_DeadTimeConfig.RisingLock = HRTIM_TIMDEADTIME_RISINGLOCK_WRITE;
  HRTIM_TIM_DeadTimeConfig.RisingSign = HRTIM_TIMDEADTIME_RISINGSIGN_POSITIVE;
  HRTIM_TIM_DeadTimeConfig.RisingSignLock = HRTIM_TIMDEADTIME_RISINGSIGNLOCK_READONLY;
  HRTIM_TIM_DeadTimeConfig.RisingValue = Fallingtime*4096/1000;
  HAL_HRTIM_DeadTimeConfig(&hhrtimA, HRTIM_TIMERINDEX_TIMER_A, &HRTIM_TIM_DeadTimeConfig);
#endif
  /* ------------------------------------------- */
  /* ADC trigger intialization (with CMP2 event) */
  /* ------------------------------------------- */
  compare_config.AutoDelayedMode = HRTIM_AUTODELAYEDMODE_REGULAR;
  compare_config.AutoDelayedTimeout = 0;
  compare_config.CompareValue = 4096000000/A_Fre/10; /* Samples in middle of ON time */
  HAL_HRTIM_WaveformCompareConfig(&hhrtimA,
                                  HRTIM_TIMERINDEX_TIMER_A,
                                  HRTIM_COMPAREUNIT_2,
                                  &compare_config);

  adc_trigger_config.Trigger = HRTIM_ADCTRIGGEREVENT24_TIMERA_CMP2;
  adc_trigger_config.UpdateSource = HRTIM_ADCTRIGGERUPDATE_TIMER_A;
  HAL_HRTIM_ADCTriggerConfig(&hhrtimA,
                             HRTIM_ADCTRIGGER_2,
                             &adc_trigger_config);

#ifdef FAULTENABLE
  /* ---------------------*/
  /* FAULT initialization */
  /* ---------------------*/
  fault_config.Filter = HRTIM_FAULTFILTER_NONE;
  fault_config.Lock = HRTIM_FAULTLOCK_READWRITE;
  fault_config.Polarity = HRTIM_FAULTPOLARITY_LOW;
  fault_config.Source = HRTIM_FAULTSOURCE_DIGITALINPUT;
  HAL_HRTIM_FaultConfig(&hhrtimA,
                        HRTIM_FAULT_1,
                        &fault_config);

  HAL_HRTIM_FaultModeCtl(&hhrtimA,
                        HRTIM_FAULT_1,
                        HRTIM_FAULTMODECTL_ENABLED);
#endif
  /* ---------------*/
  /* HRTIM start-up */
  /* ---------------*/
  /* Enable HRTIM's outputs TA1 and TA2 */
  /* Note: it is necessary to enable also GPIOs to have outputs functional */
  /* This must be done after HRTIM initialization */
  HAL_HRTIM_WaveformOutputStart(&hhrtimA, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2);

  /* Start HRTIM's TIMER A */
  HAL_HRTIM_WaveformCounterStart_IT(&hhrtimA, HRTIM_TIMERID_TIMER_A);
}
/**
* @brief  Configure GPIO outputs for the BUCK converter
* @param  None
* @retval None
*/
void MY_BSP_A_outputs_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Enable GPIOA clock for timer A outputs */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* Configure HRTIM output: TA1 (PA8) */
  GPIO_InitStruct.Pin = GPIO_PIN_8; 
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;;  
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;;  
  GPIO_InitStruct.Alternate = GPIO_AF13_HRTIM1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Configure HRTIM output: TA2 (PA9) */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
#ifdef FAULTENABLE
	/* Configure PA11 as output to control T6 FET on discovery board */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;;  
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;;  

  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif
  
}
uint8_t MY_BSP_Get_HRTIM_OUTA_Duty(void)
{
	return (HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR*100/HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].PERxR);
}
void MY_BSP_Set_HRTIM_OUTA_Duty(uint8_t A_Duty)
{
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR = HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].PERxR*A_Duty/100; /* Duty cycle update */
  if(A_Duty>=50)
	{
		HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP2xR = HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].PERxR*A_Duty/200; /* ADC trigger position update */
	}
	else
	{
		HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP2xR =  HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].PERxR*(A_Duty+100)/200; 
	}
}
void MY_BSP_Set_HRTIM_OUTA_DutyHEX(uint32_t A_DutyHEX)
{
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR = A_DutyHEX; /* Duty cycle update */
  if(A_DutyHEX>=HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].PERxR/2)
	{
		HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP2xR = A_DutyHEX/2; /* ADC trigger position update */
	}
	else
	{
		HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP2xR =  (HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].PERxR-A_DutyHEX)/2+A_DutyHEX; 
	}
}
void MY_BSP_Set_HRTIM_OUTA_Fre(uint32_t A_Fre)
{
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].PERxR = 4096000000/A_Fre; /* Duty cycle update */
} 

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 64000000
  *            HCLK(Hz)                       = 64000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            PLLMUL                         = RCC_PLL_MUL16 (16)
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  
  /* HSI Oscillator already ON after system reset, activate PLL with HSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct)!= HAL_OK)
  {
    Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    Error_Handler();
  }
}
/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef        Tim6Handle;
uint32_t                 uwIncrementState = 0;
/**
  * @brief  This function configures the TIM6 as a time base source. 
  *         The time source is configured  to have 1ms time base with a dedicated 
  *         Tick interrupt priority. 
  * @note   This function is called  automatically at the beginning of program after
  *         reset by HAL_Init() or at any time when clock is configured, by HAL_RCC_ClockConfig(). 
  * @param  TickPriority: Tick interrupt priorty.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_InitTick (uint32_t TickPriority)
{
  RCC_ClkInitTypeDef    sClokConfig;
  uint32_t              uwTimclock, uwAPB1Prescaler = 0;
  uint32_t              uwPrescalerValue = 0;
  uint32_t              pFLatency;
  
    /*Configure the TIM6 IRQ priority */
  HAL_NVIC_SetPriority(TIM6_DAC1_IRQn, TickPriority ,0);
  
  /* Enable the TIM6 global Interrupt */
  HAL_NVIC_EnableIRQ(TIM6_DAC1_IRQn);
  
  /* Enable TIM6 clock */
  __HAL_RCC_TIM6_CLK_ENABLE();
  
  /* Get clock configuration */
  HAL_RCC_GetClockConfig(&sClokConfig, &pFLatency);
  
  /* Get APB1 prescaler */
  uwAPB1Prescaler = sClokConfig.APB1CLKDivider;
  
  /* Compute TIM6 clock */
  if (uwAPB1Prescaler == 0) 
  {
    uwTimclock = HAL_RCC_GetPCLK1Freq();
  }
  else
  {
    uwTimclock = 2*HAL_RCC_GetPCLK1Freq();
  }

  /* Compute the prescaler value to have TIM6 counter clock equal to 10MHz */
  uwPrescalerValue = (uint32_t) ((uwTimclock / 10000000) - 1);
  
  /* Initialize TIM6 */
  Tim6Handle.Instance = TIM6;
    
  /* Initialize TIMx peripheral as follow:
       + Period = [(TIM6CLK/1000) - 1]. to have a 10 ns time base.
       + Prescaler = (uwTimclock/10000000 - 1) to have a 10MHz counter clock.
       + ClockDivision = 0
       + Counter direction = Up
  */
  Tim6Handle.Init.Period = (10000000 / 100000) - 1;
  Tim6Handle.Init.Prescaler = uwPrescalerValue;
  Tim6Handle.Init.ClockDivision = 0;
  Tim6Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
  if(HAL_TIM_Base_Init(&Tim6Handle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  /* Start the TIM time Base generation in interrupt mode */
  if(HAL_TIM_Base_Start_IT(&Tim6Handle) != HAL_OK)
  {
    /* Starting Error */
    Error_Handler();
  }
 

   /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Suspend Tick increment.
  * @note   Disable the tick increment by disabling TIM6 update interrupt.
  * @param  None
  * @retval None
  */
void HAL_SuspendTick(void)
{
  /* Disable TIM6 update Interrupt */
  __HAL_TIM_DISABLE_IT(&Tim6Handle, TIM_IT_UPDATE);                                                  
}

/**
  * @brief  Resume Tick increment.
  * @note   Enable the tick increment by Enabling TIM6 update interrupt.
  * @param  None
  * @retval None
  */
void HAL_ResumeTick(void)
{
  /* Enable TIM6 Update interrupt */
  __HAL_TIM_ENABLE_IT(&Tim6Handle, TIM_IT_UPDATE);
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static uint16_t ostick;
	if(ostick++>100)
	{
		HAL_IncTick();
		ostick=0;
	}
	BSP_ST_SysTick_ISR();//100k的系统时钟
}
/**
  * @brief  This function handles TIM interrupt request.
  * @param  None
  * @retval None
  */
void TIM6_DAC1_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&Tim6Handle);
}
