#ifndef	__FREERTOS_TASK_H
#define	__FREERTOS_TASK_H
#include "stm32f0xx.h" 
#include "cmsis_os.h"



void myDisplayTask(void * pvParameters);
void myInputTask(void * pvParameters);
void myControlTask(void);

void FreeRTOS_Task_Config(void);


#endif

