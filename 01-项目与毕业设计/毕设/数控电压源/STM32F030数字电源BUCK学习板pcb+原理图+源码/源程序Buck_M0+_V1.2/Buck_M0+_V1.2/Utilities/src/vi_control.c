#include "control.h"
#include "pid.h"
#include "pwm.h"
#include "timer.h"


PidObject	vPID;
PidObject	iPID;
_uDeviceFault_Type	device;


	
/**
  * @brief  vi PID Config Param
  * @param  None
  * @retval None
  */
void PID_Config(void)
{
	//电压
	pidReset(&vPID);
	vPID.dt		= (float)(1.0f/VOLTAGE_CFRE);
	vPID.kp		= 8.0f;
	vPID.ki		= 0.01f;
	vPID.kd		= 0.0f;
	vPID.desired = 5.0f;	//V.默认稳压输出5V
	vPID.iLimitLow = (float)pwm.limitMin;
	vPID.iLimitHigh = (float)pwm.limitMax;
	pidInit(&vPID);
	
	//电流
	pidReset(&iPID);
	iPID.dt		= (float)(1.0f/CURRENT_CFRE);
	iPID.kp		= 14.5f;
	iPID.ki		= 0.01f;
	iPID.kd		= 0.0f;
	iPID.desired = 5.0f;		//A.默认恒流输出1A
	iPID.iLimitLow = (float)pwm.limitMin;
	iPID.iLimitHigh = (float)pwm.limitMax;
	pidInit(&iPID);
	
	//保护参数初始化
	ProtectReset();
	
}

/**
  * @brief  Protect Reset
  * @param  None
  * @retval None
  */
void ProtectReset(void)
{
	device.controlMode = CV_MODE;
	device.controlPWM = 0;
	device.iocp = 0;
	device.iovp = 0;
	device.iuvp = 0;
	device.oocp = 0;
	device.oovp = 0;
	device.faultTime = RESET_FAULT_TIME;
	device.recoveryCnt = 0;
	device.outputState = 0;
}






