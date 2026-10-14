#include "pid.h"

/**
  * @brief  pidInit
  * @param  None
  * @retval None
  */
void pidInit(PidObject* pid)
{
	pid->ap	= pid->kp * (1 + pid->dt / pid->ki + pid->kd / pid->dt);
	pid->ai	= pid->kp * (1 + 2 * pid->kd / pid->dt);
	pid->ad	= pid->kp * pid->kd / pid->dt;
}

/**
  * @brief  pidUpdate
  * @param  None
  * @retval None
  */
float pidUpdate(PidObject* pid, const float desired, const float now_value)
{
	float output=0.0f;
	
	pid->error = desired - now_value;

	pid->inc = (pid->ap * pid->error) - (pid->ai * pid->prevError1) + (pid->ad * pid->prevError2);
	
	output = pid->inc + pid->outLast1;
	
	if(output > pid->iLimitHigh)
	{
		output = pid->iLimitHigh;
	}
	
	if(output < pid->iLimitLow)
	{
		output = pid->iLimitLow;
	}
	
	pid->prevError2	= pid->prevError1;
	pid->prevError1	= pid->error;
	pid->outLast1	= output;
	
	return output;
}

/**
  * @brief  pid calear
  * @param  None
  * @retval None
  */
void pidClear(PidObject* pid)
{
	pid->error     	= 0;
	pid->prevError1 = 0;
	pid->prevError2 = 0;
	
	pid->outLast1	= 0;
	pid->inc		= 0;
}


/**
  * @brief  pidReset
  * @param  None
  * @retval None
  */
void pidReset(PidObject* pid)
{
	pid->error     	= 0.0f;
	pid->prevError1 = 0.0f;
	pid->prevError2 = 0.0f;
	pid->outLast1	= 0.0f;
	pid->inc		= 0.0f;
	
	pid->desired = 0.0f;
	pid->iLimitHigh = 0.0f;
	pid->iLimitLow = 0.0f;
	
	pid->dt = 0.0f;
	pid->kp = 0.0f;
	pid->ki = 0.0f;
	pid->kd = 0.0f;
	
	pid->ad = 0.0f;
	pid->ai = 0.0f;
	pid->ap = 0.0f;
	
}







