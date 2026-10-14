#include "vi_pid.h"
#include "pid.h"
#include <stdbool.h>
#include "freeRTOS_task.h"

PidObject	vControl;
PidObject	iControl;

static configParam_t configParam;
	
/**
  * @brief  vi PID Config Param
  * @param  None
  * @retval None
  */
static void viPIDConfigParam(void)
{
	configParam.v.kp = 0.12f;	   //0.32f;
	configParam.v.ki = 0.035f;	//0.02f;	   //0.16f;
	configParam.v.kd = 0.00001f;   //0.00001f;

	configParam.i.kp = 0.01f;
	configParam.i.ki = 0.0042f;//0.0042f;
	configParam.i.kd = 0.00000f;//0.0024f;
}

/**
  * @brief  VI Control Init
  * @param  None
  * @retval None
  */
void VI_Control_Init(void)
{
	viPIDConfigParam();
	
	/* 电压环 */
	pidInit(&vControl, 0, configParam.v, PID_UPDATE_DT);
	pidSetOutLimit(&vControl, PID_BUCK_V_LIMIT_MIN, PID_BUCK_V_LIMIT_MAX);
	
	/* 电流环 */
	pidInit(&iControl, 0, configParam.i, PID_UPDATE_DT);
	pidSetOutLimit(&iControl, PID_BUCK_C_LIMIT_MIN, PID_BUCK_C_LIMIT_MAX);
}

/**
  * @brief  voltage PID
  * @param  None
  * @retval None
  */
void vContolPID(uint16_t actualVout, uint16_t desiredVout, float *output)
{
	*output = pidUpdate(&vControl, desiredVout-actualVout);
//	iControl.outLast1 = *output;
//	iControl.outLast1 = *output;
}

/**
  * @brief  current PID
  * @param  None
  * @retval None
  */
void iContolPID(uint16_t actualIout, uint16_t desiredIout, float *output)
{
	*output = pidUpdate(&iControl, desiredIout-actualIout);
//	vControl.outLast1 = *output;
}

/**
  * @brief  PID reset
  * @param  None
  * @retval None
  */
void VresetContolPID(void)
{
	pidReset(&vControl);
}

/**
  * @brief  PID reset
  * @param  None
  * @retval None
  */
void IresetContolPID(void)
{
	pidReset(&iControl);
}

/**
  * @brief  
  * @param  PID cycle timer. (Unit second)
  * @retval None
  */
void iSetContolPIDdt(const float dt)
{
	iControl.dt = dt;
}

/**
  * @brief  
  * @param  PID cycle timer. (Unit second)
  * @retval None
  */
void vSetContolPIDdt(const float dt)
{
	vControl.dt = dt;
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void pidRestToBuck(void)
{
	pidReset(&vControl);
	pidReset(&iControl);
	pidSetOutLimit(&vControl, PID_BUCK_V_LIMIT_MIN, PID_BUCK_V_LIMIT_MAX);
	pidSetOutLimit(&iControl, PID_BUCK_C_LIMIT_MIN, PID_BUCK_C_LIMIT_MAX);
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void pidRestToBoost(void)
{
	pidReset(&vControl);
	pidReset(&iControl);
	pidSetOutLimit(&vControl, PID_BOOST_V_LIMIT_MIN, PID_BOOST_V_LIMIT_MAX);
	pidSetOutLimit(&iControl, PID_BOOST_C_LIMIT_MIN, PID_BOOST_C_LIMIT_MAX);
}



















