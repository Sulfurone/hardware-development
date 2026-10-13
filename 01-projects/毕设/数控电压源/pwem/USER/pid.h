#ifndef __PID_H
#define __PID_H
#include "stm32f10x.h"

typedef struct
{
	float desired;		//< set point
	float error;        //< error-0
	float prevError1;	//< previous error-1
	float prevError2;	//< previous error-2
	float kp;           //< proportional gain
	float ki;           //< integral gain
	float kd;           //< derivative gain
	float ap;           //< ap = Kp(1 + T/Ti + Td/T)
	float ai;           //< ai = Kp(1 + 2Td/T)
	float ad;           //< ad = Kp*Td/T
	float inc;         	//< proportional output
	float outLast1;     //< pid out last1
	float iLimitHigh;	//< integral limit
	float iLimitLow;    //< integral limit
	float dt;           //< delta-time dt
} PidObject;

extern PidObject vPID;
extern PidObject iPID;


void pidInit(PidObject* pid);
float pidUpdate(PidObject* pid, const float desired, const float now_value);
void pidClear(PidObject* pid);
void pidReset(PidObject* pid);


#endif /* __PID_H */
