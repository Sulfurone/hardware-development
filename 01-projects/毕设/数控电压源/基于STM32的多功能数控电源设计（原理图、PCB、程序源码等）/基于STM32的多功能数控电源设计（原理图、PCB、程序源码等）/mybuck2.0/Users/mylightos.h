#ifndef _MYLIGHTOS_H_
#define _MYLIGHTOS_H_
#include "type.h"
#include "string.h"
#include "stdio.h"
/*————————————————————————————————
*软定时器有关
————————————————————————————————*/
enum
{
	TIMERID_LED = 1,
		TIMERID_AD ,
		TIMERID_PID ,
		TIMERID_DIP , 
		TIMERID_CUR_PRO ,
		TIMERID_CUR_RECHECK,
		TIMERID_UART1_RX ,
	TIMER_COUNT_END
};
#define TIMER_COUNT	(TIMER_COUNT_END)
typedef struct
{
	PTIMERFUNC 	pfTimer;//软定时器目标函数
	uint32_t	  u32Interval;//计数值
	PVOID		    pvParams;
	uint8_t	    u8ID;
	bool_t		  bActive;//是否激活
	bool_t		  bShouldInvoke;//是否触发
	bool_t		  bInfinite;//是否能循环执行
	uint32_t	  u32CurrentTick;//当前值
} TIMER, *PTIMER;
extern TIMER timer[TIMER_COUNT];
extern void BSP_ST_SysTick_Init(void);
extern uint32_t BSP_ST_TickCount_Get(void);
extern void BSP_ST_SysTick_ISR(void);
extern bool_t BSP_ST_SysTick_Delay(uint32_t tick);
extern void BSP_ST_Timer_Poll(void);
extern void BSP_ST_Timer_Init(void);
extern void BSP_ST_Timer_Set(PTIMERFUNC pfTimer, uint32_t u32Interval, PVOID pvParams, bool_t bInfinite, bool_t bActive , uint8_t u8TimerID);
extern void BSP_ST_Timer_Stop(uint8_t u8TimerID);
extern void BSP_ST_Timer_Start(uint16_t u8TimerID);
extern void BSP_ST_Timer_Del(uint16_t u8TimerID);
extern bool_t IsTimeExpired(uint32_t tick);
#define AT_BUFFER_LEN 300
#define AT_MSG_QUENE_LENGTH 4
typedef enum
{  
    PACKAGE_OK = 0,
    EN_PACKAGE_FULL,
    DE_PACKAGE_EMPTY,
    RV_CH_Err, 
} PACKAGE_STATUS_T;
typedef struct
{
    uint8_t u8ATMsgData[300];
}
AT_MSG_SINGLE;

typedef struct __AT_MSG_QUENE_T
{
    uint8_t u8ATMQfront;//前计数           
    uint8_t u8ATMQrear; //后计数
    AT_MSG_SINGLE ATMQFrameBuf[AT_MSG_QUENE_LENGTH];
}AT_MSG_QUENE_T;
extern PACKAGE_STATUS_T EnQueueMsg(AT_MSG_QUENE_T *dest,AT_MSG_SINGLE *src);
extern PACKAGE_STATUS_T DeQueueCanMsg(AT_MSG_QUENE_T *src,AT_MSG_SINGLE *dest);
#endif
