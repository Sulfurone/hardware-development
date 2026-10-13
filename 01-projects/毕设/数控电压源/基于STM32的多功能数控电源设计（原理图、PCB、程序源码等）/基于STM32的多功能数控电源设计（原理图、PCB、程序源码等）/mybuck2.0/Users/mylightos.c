#include "mylightos.h"
/*————————————————————————————————
*软定时器有关
————————————————————————————————*/
volatile uint32_t globalTime = 0;//系统计时全局变量
TIMER timer[TIMER_COUNT];
/**************************************************************************************
* FunctionName   : MyOS_SysTick_Init
* Description    : 初始化系统计时全局变量
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void BSP_ST_SysTick_Init(void)
{
	globalTime = 0;
}
/**************************************************************************************
* FunctionName   : MyOS_TickCount_Get
* Description    : 得到系统计时全局变量
* EntryParameter : None
* ReturnValue    : 当前计时值
**************************************************************************************/
uint32_t BSP_ST_TickCount_Get(void)
{
	return globalTime;
}
/**************************************************************************************
* FunctionName   : MyOS_SysTick_ISR
* Description    : 系统中断函数
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void BSP_ST_SysTick_ISR(void)
{
	uint8_t i;
	globalTime++;
  for(i = 0; i < TIMER_COUNT; i++)
	{
		if((timer[i].u8ID != 0) && (timer[i].bActive == TRUE) && (int32_t)(globalTime - timer[i].u32CurrentTick) >= (int32_t)(timer[i].u32Interval))
		{
			timer[i].bShouldInvoke = TRUE;
		}
	}
	
}
/**************************************************************************************
* FunctionName   : MyOS_Timer_Poll
* Description    : 软定时器轮询，主循环中调用
* EntryParameter : 
* ReturnValue    : 
**************************************************************************************/
void BSP_ST_Timer_Poll(void)
{
	uint8_t i;
	
	for(i = 0; i < TIMER_COUNT; i++)
	{
		if(timer[i].bActive && timer[i].bShouldInvoke && (timer[i].u8ID != 0))
		{
			timer[i].pfTimer(timer[i].pvParams);
			if(!timer[i].bInfinite)
			{
				timer[i].bActive = FALSE;
				timer[i].bShouldInvoke = FALSE;
			}
			else
			{
				timer[i].bActive = TRUE;
				timer[i].bShouldInvoke = FALSE;
				timer[i].u32CurrentTick = BSP_ST_TickCount_Get();
			}
		}
	}	
}
/**************************************************************************************
* FunctionName   : MyOS_Timer_Init
* Description    : 软定时器初始化函数，默认全空
* EntryParameter : 
* ReturnValue    : 
**************************************************************************************/
void BSP_ST_Timer_Init(void)
{
	uint8_t i = 0;
	for(i = 0; i < TIMER_COUNT; i++)
	{
		timer[i].pfTimer = NULL;
		timer[i].u32Interval = 0;
		timer[i].pvParams = NULL;
		timer[i].u8ID = 0;
		timer[i].bActive = FALSE;
		timer[i].bShouldInvoke = FALSE;
		timer[i].bInfinite = FALSE;
	}
}
/**************************************************************************************
* FunctionName   : MyOS_Timer_Init
* Description    : 软定时器配置函数，按照输入参数进行配置
* EntryParameter : PTIMERFUNC pfTimer, uint32_t u32Interval, PVOID pvParams, bool_t bInfinite, uint8_t u8TimerID
* ReturnValue    : 
**************************************************************************************/
void BSP_ST_Timer_Set(PTIMERFUNC pfTimer, uint32_t u32Interval, PVOID pvParams, bool_t bInfinite, bool_t bActive , uint8_t u8TimerID)
{
	uint8_t i = 0;
	for(i = 0; i < TIMER_COUNT; i++)
	{
		if(timer[i].u8ID == u8TimerID)
		{
			timer[i].pfTimer = pfTimer;
			timer[i].u32Interval = u32Interval;
			timer[i].pvParams = pvParams;
			timer[i].u8ID = u8TimerID;
			timer[i].bActive = bActive;
			timer[i].bShouldInvoke = FALSE;
			timer[i].bInfinite = bInfinite;
			timer[i].u32CurrentTick = BSP_ST_TickCount_Get();
			return;
		}
	}
	for(i = 0; i < TIMER_COUNT; i++)
	{
		if(timer[i].u8ID == 0)
		{
			timer[i].pfTimer = pfTimer;
			timer[i].u32Interval = u32Interval;
			timer[i].pvParams = pvParams;
			timer[i].u8ID = u8TimerID;
			timer[i].bActive = bActive;
			timer[i].bShouldInvoke = FALSE;
			timer[i].bInfinite = bInfinite;
			timer[i].u32CurrentTick = BSP_ST_TickCount_Get();
			return;
		}
	}
}
/**************************************************************************************
* FunctionName   : MyOS_Timer_Stop
* Description    : 关闭定时器
* EntryParameter : u8TimerID
* ReturnValue    : 
**************************************************************************************/
void BSP_ST_Timer_Stop(uint8_t u8TimerID)
{
	uint8_t i = 0;
	for(i = 0; i < TIMER_COUNT; i++)
	{
		if(timer[i].u8ID == u8TimerID)
		{
			timer[i].bActive = FALSE;
			return;
		}
	}
}
/**************************************************************************************
* FunctionName   : MyOS_Timer_Start
* Description    : 打开定时器
* EntryParameter : u8TimerID
* ReturnValue    : 
**************************************************************************************/
void BSP_ST_Timer_Start(uint16_t u8TimerID)
{
	uint8_t i = 0;
	for(i = 0; i < TIMER_COUNT; i++)
	{
		if(timer[i].u8ID == u8TimerID)
		{
			timer[i].bActive = TRUE;
			return;
		}
	}
}
/**************************************************************************************
* FunctionName   : MyOS_Timer_Del
* Description    : 删除定时器
* EntryParameter : u8TimerID
* ReturnValue    : 
**************************************************************************************/
void BSP_ST_Timer_Del(uint16_t u8TimerID)
{
	uint8_t i = 0;
	for(i = 0; i < TIMER_COUNT; i++)
	{
		if(timer[i].u8ID == u8TimerID)
		{
			timer[i].u8ID = 0;
			timer[i].bActive = FALSE;
			return;
		}
	}
}
bool_t IsTimeExpired(uint32_t tick)
{
	uint32_t time_now;
    
	time_now = globalTime;
    
	if (time_now > tick)
	{
		if ((time_now - tick) < 0x80000000)
			return TRUE;
		else
		  	return FALSE;
	}
	else
	{
		if ((tick - time_now) > 0x80000000)
		  	return TRUE;
		else
		 	return FALSE;
	}	
}
/**************************************************************************************
* FunctionName   : EnQueueMsg
* Description    : 由单个数组向结构体队列的数据搬移
* EntryParameter : 
* ReturnValue    : 
**************************************************************************************/
PACKAGE_STATUS_T EnQueueMsg(AT_MSG_QUENE_T *dest,AT_MSG_SINGLE *src)
{
    uint8_t i = 0;

    i = ((dest->u8ATMQrear)+1)%AT_MSG_QUENE_LENGTH;
    if(dest->u8ATMQfront  == i)
    {
        return EN_PACKAGE_FULL;
    }
    i = dest->u8ATMQrear;
    (dest->ATMQFrameBuf)[i] = *src;
    i = (i+1)%AT_MSG_QUENE_LENGTH;
    dest->u8ATMQrear = i;
    return PACKAGE_OK;
}
/**************************************************************************************
* FunctionName   : DeQueueCanMsg
* Description    : 由结构体队列向单个数组的数据搬移
* EntryParameter : 
* ReturnValue    : 
**************************************************************************************/
PACKAGE_STATUS_T DeQueueCanMsg(AT_MSG_QUENE_T *src,AT_MSG_SINGLE *dest)
{
    uint8_t i = 0;
    if(src->u8ATMQrear == src->u8ATMQfront)
    {
        return DE_PACKAGE_EMPTY;
    }
    i = src->u8ATMQfront;
    *dest = (src->ATMQFrameBuf)[i];
    //clear the data
    memset((void*)&(src->ATMQFrameBuf)[i], 0, sizeof((src->ATMQFrameBuf)[i]));
    i = (i+1)%AT_MSG_QUENE_LENGTH;
    src->u8ATMQfront = i;
  
    return PACKAGE_OK;
}
