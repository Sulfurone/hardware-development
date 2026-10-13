/*#ifndef CJ_H
#define CJ_H
#include "sys.h"
extern  double distance2;

void CJ_Init(void);
void TIM2_TRIG(void);
void TIM2_Init(u16 Period,u16 Prescaler);//65534,71
	void TIM2_IRQHandler(void);

#endif


	*/
	
#ifndef _CJ_H
#define _CJ_H

#include "sys.h"
#define uint unsigned int
#define TRIG_Send  PAout(14)
#define ECHO_Reci  PAin(13)

//void CH_SR05_Init(void);
//float Senor_Using(void);
void NVIC_Config(void);
#endif



