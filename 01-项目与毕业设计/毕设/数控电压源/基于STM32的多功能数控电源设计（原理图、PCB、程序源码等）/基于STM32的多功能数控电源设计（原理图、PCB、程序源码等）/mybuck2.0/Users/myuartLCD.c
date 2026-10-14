#include "myuartLCD.h"
BOOLEAN Uartflag = TRUE;
BOOLEAN Loadcheckflag = TRUE;
uint8_t menuflag = 0;
uint8_t keyvalue = 0;
uint16_t Set_vol = 3000;
uint16_t Set_cur = 500;
double Outvolvalue = 0.0;
double Involvalue = 0.0;
double Outcurvalue = 0.0;
double Loadvalue = 0.0;
void OverCurRecheck(PVOID pvParams)
{
	if(-(Outcurvalue/1000-1.585)/0.265<2.0)
	{
		LED1_Off();
		BSP_ST_Timer_Stop(TIMERID_CUR_PRO);
		switch(menuflag)
		{
			case 2:
			{
				Set_vol = 2000;
				BSP_ST_Timer_Start(TIMERID_PID);
				HAL_Delay(200);
				BSP_ST_Timer_Start(TIMERID_CUR_PRO);
			}break;
			case 3:
			{
				BSP_ST_Timer_Start(TIMERID_PID);
			}break;
		}
	}
	else
	{			
		BSP_ST_Timer_Set(OverCurRecheck, 1000000-20000, NULL, FALSE, TRUE, TIMERID_CUR_RECHECK);//为下一次检测设置任务
	}
	
}
void OverCurProtect(PVOID pvParams)
{
	if(-(Outcurvalue/1000-1.585)/0.265>=2.0)
	{
		MY_BSP_Set_HRTIM_OUTA_Duty(1);
		LED1_On();
		BSP_ST_Timer_Stop(TIMERID_PID);
		BSP_ST_Timer_Set(OverCurRecheck, 1000000-20000, NULL, FALSE, TRUE, TIMERID_CUR_RECHECK);//为下一次检测设置任务
	}
}
void GetADValue(PVOID pvParams)
{				
	Outvolvalue = (double)HAL_ADCEx_InjectedGetValue(&Adc1Handle, ADC_INJECTED_RANK_1)* ADC_VREF*29/0xFFF;	
	Involvalue = (double)HAL_ADCEx_InjectedGetValue(&Adc1Handle, ADC_INJECTED_RANK_2)* ADC_VREF*30.4/0xFFF;
	Outcurvalue = (double)HAL_ADCEx_InjectedGetValue(&Adc1Handle, ADC_INJECTED_RANK_3)* ADC_VREF/0xFFF;
	if(Loadcheckflag == TRUE)
	{
		Loadvalue = (Outvolvalue*265) /(1585-Outcurvalue);//放大一千倍的负载
		Loadcheckflag = FALSE;
	}
}
static int32_t hexvol(int32_t temp)
{
	return temp*0xFFF/ADC_VREF/29;
}
static int32_t hexcur(int32_t temp)
{
	return temp*0xFFF/ADC_VREF;
}
void pidpoll(PVOID pvParams)
{	
	switch(menuflag)
	{
		case 2:
		{
			/* Compute PI buck Mode */
			MY_BSP_Set_HRTIM_OUTA_Duty(PI_Buck(Outvolvalue,Set_vol,hexvol)*100/HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].PERxR);
		}break;
		case 3:
		{
			MY_BSP_Set_HRTIM_OUTA_DutyHEX(PI_Buck(Outcurvalue,1585-Set_cur*265/1000,hexcur));
		}break;
	}	
}
void dipvol(PVOID pvParams)//输入电压,输出电流,输出电压的动态显示函数
{
	char buf1[100];
	sprintf(buf1,"DS16(360,60,'%.3f  ',16);DS16(360,120,'%.3f  ',16);DS16(360,180,'%.3f  ',16);S%d;DS16(360,210,'%d ',16);\r\n",Involvalue/1000,(1585-Outcurvalue)/265,Outvolvalue/1000,(uint32_t)Outvolvalue*256/24000,MY_BSP_Get_HRTIM_OUTA_Duty());
	Uart1_Send((uint8_t*)buf1);
	HAL_Delay(20);
}
void UartLCDPoll(void)
{
	char8_t *strPtr = 0;
	AT_MSG_SINGLE msg; 
	if(DeQueueCanMsg(&ATrecQue,&msg))
    return;	
	switch(menuflag)
	{
		case 0:
		{
			if(strstr((const char*)msg.u8ATMsgData,"OK") != NULL)//进入了主菜单
			{

			}		
		}break;
		case 1:
		{
			if(strstr((const char*)msg.u8ATMsgData,"BN:1") != NULL)//电压模式按键按下
			{
				keyvalue = 11;
			}
			else if(strstr((const char*)msg.u8ATMsgData,"BN:2") != NULL)//电流模式按键按下
			{
				keyvalue = 12;
			}
			else if(strstr((const char*)msg.u8ATMsgData,"BN:3") != NULL)//充电模式按键按下
			{
				keyvalue = 13;
			}
			else if(strstr((const char*)msg.u8ATMsgData,"OK") != NULL)//从主菜单切换过来
			{
			}			
		}break;
		case 2://电压菜单下选择
		{
			if(strstr((const char*)msg.u8ATMsgData,"BN:2") != NULL)//电流模式按键按下
			{
				keyvalue = 12;
			}
			else if(strstr((const char*)msg.u8ATMsgData,"BN:3") != NULL)//充电模式按键按下
			{
				keyvalue = 13;
			}			
			else if(strstr((const char*)msg.u8ATMsgData,"BN:4") != NULL)//UP按键按下
			{
				keyvalue = 21;
			}
			else if(strstr((const char*)msg.u8ATMsgData,"BN:5") != NULL)//DOWN模式按键按下
			{
				keyvalue = 22;
			}
		}break;
		case 3://电流菜单下操作
		{
			if(strstr((const char*)msg.u8ATMsgData,"BN:1") != NULL)//电压模式按键按下
			{
				keyvalue = 11;
			}
			else if(strstr((const char*)msg.u8ATMsgData,"BN:3") != NULL)//充电模式按键按下
			{
				keyvalue = 13;
			}			
			else if(strstr((const char*)msg.u8ATMsgData,"BN:4") != NULL)//UP按键按下
			{
				keyvalue = 31;
			}
			else if(strstr((const char*)msg.u8ATMsgData,"BN:5") != NULL)//DOWN模式按键按下
			{
				keyvalue = 32;
			}
		}break;		
		case 4://充电菜单下操作
		{
			if(strstr((const char*)msg.u8ATMsgData,"BN:1") != NULL)//电压模式按键按下
			{
				keyvalue = 11;
			}
			else if(strstr((const char*)msg.u8ATMsgData,"BN:2") != NULL)//电流模式按键按下
			{
				keyvalue = 12;
			}
			else if(strstr((const char*)msg.u8ATMsgData,"BN:4") != NULL)//UP按键按下
			{
				keyvalue = 41;
			}
			else if(strstr((const char*)msg.u8ATMsgData,"BN:5") != NULL)//DOWN模式按键按下
			{
				keyvalue = 42;
			}
		}break;		
	}
}
void RefreshSetValue(void)
{
	char buf1[40];
	switch(menuflag)
	{
		case 2:
		{
			sprintf(buf1,"DS16(360,150,'%d    ',16);\r\n",Set_vol/1000);
		}break;
		case 3:
		{
			
			sprintf(buf1,"DS16(360,150,'%.3f  ',16);\r\n",(double)Set_cur/1000);
		}break;
	}		
	Uart1_Send((uint8_t*)buf1);
	HAL_Delay(20);
}
void MainMenu(void)//在此菜单下,PWM设为最小值,只有AD和过流检测在工作,理论输出为0
{
	Uart1_Send((uint8_t *)"SPG(1);\r\n");
	HAL_Delay(20);
	menuflag = 1;
	/* Initialize ADC to be triggered by the HRTIMER,此AD和PWM输出为一对 */
  MY_BSP_Init_ADC1();
	MY_BSP_Init_HRTIM_OUTA(200000,50,50);//200K,50ns,50ns
	MY_BSP_Set_HRTIM_OUTA_Duty(1);
	MY_BSP_A_outputs_Config();	
	BSP_ST_Timer_Set(dipvol , 10000-2000  , NULL, TRUE , FALSE, TIMERID_DIP);
	BSP_ST_Timer_Set(GetADValue, 10, NULL, TRUE, TRUE , TIMERID_AD);//AD采样任务
	BSP_ST_Timer_Set(OverCurProtect, 10, NULL, TRUE, TRUE , TIMERID_CUR_PRO);//过流检测任务		
	BSP_ST_Timer_Set(pidpoll, 50, NULL, TRUE, FALSE, TIMERID_PID);//为下一次检测设置任务
}
void VolMenu(void)
{
	Uart1_Send((uint8_t *)"SPG(2);\r\n");
	HAL_Delay(50);
	menuflag = 2;
	
  Reset_PI();
	BSP_ST_Timer_Start(TIMERID_PID);	
	BSP_ST_Timer_Start(TIMERID_DIP);	
}
void CurMenu(void)
{
	Uart1_Send((uint8_t *)"SPG(3);\r\n");
	HAL_Delay(50);
	menuflag = 3;	
	Reset_PI();
	BSP_ST_Timer_Start(TIMERID_PID);	
	BSP_ST_Timer_Start(TIMERID_DIP);	
}
void ChaMenu(void)
{
	//BSP_ST_Timer_Stop(TIMERID_DIP);
}
void KeyPoll(void)
{
	switch(keyvalue)
	{
		case 11:
		{
			VolMenu();
		}break;
		case 12:
		{
			CurMenu();
		}break;		
		case 13:
		{
			ChaMenu();
		}break;		
		case 21:
		{
			Set_vol = Set_vol + 1000;
			if(Set_vol >= 23000)
			{
				Set_vol = 23000;
			}
			RefreshSetValue();

		}break;
		case 22:
		{
			Set_vol = Set_vol - 1000;
			if(Set_vol < 1000)
			{
				Set_vol = 1000;
			}			
			RefreshSetValue();
		}break;
		case 31:
		{
			Set_cur = Set_cur + 100;
			RefreshSetValue();			
		}break;
		case 32:
		{
			Set_cur = Set_cur - 100;
			if(Set_cur <= 100)
			{
				Set_cur = 100;
			}
			RefreshSetValue();			
		}break;
		case 41:
		{
			
		}break;
		case 42:
		{
			
		}break;
	}
	if(keyvalue != 0)
	{
		keyvalue = 0;
	}
}
