/*
 * track.h
 *
 *  Created on: 2020Äê12ÔÂ5ÈÕ
 *      Author: Indira
 */

#ifndef HEADERS_TRACK_H_
#define HEADERS_TRACK_H_

/* DriverLib Includes */
#include <driverlib.h>


//#define   BASICPWM      100// 128

extern uint16_t       Pulse_Count;


void Infrared_Init(void);
void bsp_GetLineWalking(int *p_iL1, int *p_iL2, int *p_iR1, int *p_iR2);
void app_LineWalking(void);
void Tracking_Work(void);

extern uint8_t   line_flag;    

#endif /* HEADERS_TRACK_H_ */
