/*
 * MOTOR.H
 *
 *  Created on: 2020Äê11ÔÂ29ÈÕ
 *      Author: Indira
 */

#ifndef MOTOR_H_
#define MOTOR_H_

/* DriverLib Includes */
#include <driverlib.h>

extern Timer_A_PWMConfig pwmConfigLeft;
extern Timer_A_PWMConfig pwmConfigRight;

void Motor_init(void);

void setPWM(int left,int right);
void Car_Forward_Function(void);
void Car_LeftRun_Function(void);
void Car_RightRun_Function(void);
void Car_RightRun(int left,int right);
void Car_backRun_Function(void);
void Car_StopRun_Function(void);

#endif /* MOTOR_H_ */
