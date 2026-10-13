#ifndef __VI_PID_H
#define __VI_PID_H
#include "stm32f0xx.h"   
#include "stdbool.h"
#include "freeRTOS_task.h"
#include "pwm.h"

/***********************************************************************************
**
***********************************************************************************/
/* Buck模式下电压的输入范围 */
#define I_BUCK_VOLGATE_MAX			38500u
#define I_BUCK_VOLGATE_MIN			24000u
/* Buck模式下电压的输出范围 */
#define O_BUCK_VOLGATE_MAX			23500u
#define O_BUCK_VOLGATE_MIN			18000u
/* Buck模式下电流的输出范围 */
#define O_BUCK_CURRENT_MAX			2500u
#define O_BUCK_CURRENT_MIN			100u
	
/* Buck模式下占空比的理论值 */
#define BUCK_DUTY_MIN				((1.0f*O_BUCK_VOLGATE_MIN)/I_BUCK_VOLGATE_MAX)
#define BUCK_DUTY_MAX				((1.0f*O_BUCK_VOLGATE_MAX)/I_BUCK_VOLGATE_MIN)

/* Buck恒压模式下PWM的起调理论值 */		
#define PWM_BUCK_V_BASIC_MIN		(uint16_t)(0.85f*BUCK_DUTY_MIN*PWM_PULSE_MAX)
/* Buck恒流模式下PWM的起调理论值 */	
#define PWM_BUCK_I_BASIC			(uint16_t)(0.01f*BUCK_DUTY_MAX*PWM_PULSE_MAX)

/* Buck恒压模式下限制PID的增量范围 */
#define PID_BUCK_V_LIMIT_MIN		(float)(0.01f*BUCK_DUTY_MAX*PWM_PULSE_MAX)
#define PID_BUCK_V_LIMIT_MAX		(float)(0.6f*PWM_PULSE_MAX)
/* Buck恒流模式下限制PID的增量范围 */
#define PID_BUCK_C_LIMIT_MIN		(float)(-0.5f*BUCK_DUTY_MAX*PWM_PULSE_MAX)
#define PID_BUCK_C_LIMIT_MAX		(float)(0.98f*BUCK_DUTY_MAX*PWM_PULSE_MAX)

/* Buck模式下输入输出保护门限值 */
#define __BUCK_IUVP_VOLGATE			23500u  //输入欠压
#define __BUCK_IOVP_VOLGATE			38500u  //输入过压


/***********************************************************************************
**
***********************************************************************************/
/* Boost模式下电压的输入范围 */
#define I_BOOST_VOLGATE_MAX			21000u
#define I_BOOST_VOLGATE_MIN			18000u
/* Boost模式下电压的输出范围 */
#define O_BOOST_VOLGATE_MAX			32000u
#define O_BOOST_VOLGATE_MIN			24000u
/* Boost模式下电压的输出范围 */
#define O_BOOST_CURRENT_MAX			2500u
#define O_BOOST_CURRENT_MIN			100u

/* Boost模式下占空比的理论值 */
#define BOOST_DUTY_MIN				((1.0f*I_BOOST_VOLGATE_MAX)/O_BOOST_VOLGATE_MIN)
#define BOOST_DUTY_MAX				((1.0f*I_BOOST_VOLGATE_MIN)/O_BOOST_VOLGATE_MAX)

/* Boost恒压模式下PWM的起调理论值 */			
#define PWM_BOOST_V_BASIC_MIN		(uint16_t)(BOOST_DUTY_MIN*PWM_PULSE_MAX) //83-占空比增大boost输出减小
/* Boost恒流模式下PWM的起调理论值 */	
#define PWM_BOOST_I_BASIC			(uint16_t)(0.98f*PWM_BOOST_V_BASIC_MIN)

/* Boost恒压模式下限制PID的增量范围 */
#define PID_BOOST_V_LIMIT_MIN		(float)(0.0f)
#define PID_BOOST_V_LIMIT_MAX		(float)(0.4f*PWM_PULSE_MAX)
/* Boost恒流模式下限制PID的增量范围 */
#define PID_BOOST_C_LIMIT_MIN		(float)(-0.5f*PWM_PULSE_MAX)
#define PID_BOOST_C_LIMIT_MAX		(float)(0.0f*PWM_PULSE_MAX)

/* Boost模式下输入输出保护门限值 */
#define __BOOST_OUT_IUVP_VOLGATE	17500u   //输入欠压
#define __BOOST_OUT_OOCP_CURRENT	2500u   //输出过流


/***********************************************************************************
**
***********************************************************************************/



/***********************************************************************************
**PID更新速率,单位Hz
***********************************************************************************/
#define PID_UPDATE_RATE_HZ			20000
#define PID_UPDATE_DT				(1.0f/PID_UPDATE_RATE_HZ)

/***********************************************************************************
**
***********************************************************************************/
void VI_Control_Init(void);
void vContolPID(uint16_t actualVout, uint16_t desiredVout, float *output);
void iContolPID(uint16_t actualIout, uint16_t desiredIout, float *output);
void IresetContolPID(void);
void VresetContolPID(void);
void iSetContolPIDdt(const float dt);
void vSetContolPIDdt(const float dt);

void pidRestToBuck(void);
void pidRestToBoost(void);

#endif
















