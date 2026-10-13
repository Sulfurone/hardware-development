#include "freeRTOS_task.h"
#include "adc.h"
#include "led.h"
#include "key.h"
#include "oled.h"
#include "pwm.h"
#include "pid.h"

#include "control.h"
#include "calibration.h"


/* Private variables ---------------------------------------------------------*/
uint8_t set_step_flag='>';
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  This function ...
  * @param  None
  * @retval None
  */
void myDisplayTask(void * pvParameters)
{
	float t_v=0.0f;
	float t_i=0.0f;
	uint8_t ec_cnt=0;
	
	for(;;)
	{
		t_v = power.Vo;
		t_i = power.Io;
		if(ec11.trigDis!=false)
		{
			ec11.trigDis = false;
			if(ec11.set_vi!=0)
			{	//设置电压
				OLED_ShowChar(X_UNIT+8, Y_VO, set_step_flag, 16);
				OLED_ShowChar(X_UNIT+8, Y_IO, ' ', 16);
			}
			else
			{	//设置电流
				OLED_ShowChar(X_UNIT+8, Y_VO, ' ', 16);
				OLED_ShowChar(X_UNIT+8, Y_IO, set_step_flag, 16);
			}
		}
		if(ec11.active!=false)
		{
			if(ec11.set_vi!=0)
			{	//设置电压
				t_v = ec11.set_v;
			}
			else
			{	//设置电流
				t_i = ec11.set_i;
			}
			
			ec_cnt++;
			if(ec_cnt>5)
			{
				ec_cnt = 0;
				ec11.active = false;
			}
		}
		
		if(device.outputState==0)
		{
			t_v = ec11.set_v;
			t_i = ec11.set_i;
		}
		OLED_ShowVI(t_v, t_i);
		
		vTaskDelay(50);
	}
}

/**
  * @brief  This function ...
  * @param  None
  * @retval None
  */
void myInputTask(void * pvParameters)
{
	uint8_t keyCode=0;
	
	for(;;)
	{
		ADC_GetNewSample();
		keyCode = Key_Scan();
		if(keyCode!=0)
		{
			BUZZER_ON;
		}
		
		if(keyCode==KEY_OUT_FLAG)
		{
			device.outputState = 1 - device.outputState;
			if(device.outputState)
			{
				LED_OUT_ON;
				device.recoveryCnt = 0;
			}
			else
			{
				PWM_Stop();
				LED_OUT_OFF;
				LED_CV_CC_OFF;
			}
		}
		else if(keyCode==KEY_VI_FLAG)
		{
			ec11.set_vi = 1 - ec11.set_vi;
			ec11.trigDis = true;
		}
		else if(keyCode==KEY_RANGE_FLAG)
		{
			if(ec11.step==0.1f)
			{
				ec11.step = 1.0f;
				set_step_flag = '<';
			}
			else
			{
				ec11.step = 0.1f;
				set_step_flag = '>';
			}
			ec11.trigDis = true;
		}
		
		vTaskDelay(5);
	}
}

/**
  * @brief  This function ...
  * @param  None
  * @retval None
  */
static uint16_t cnt=0;
void myControlTask(void)
{
	float fb_io=0.0f;
	float fb_vo=0.0f;
	float fb_w=0.0f;
	
	if(device.outputState!=0)
	{
		device.iovp = (power.Vi>PW_VOLTAGV_IN_MAX)?1:0;  //输入过压保护
		device.iuvp = (power.Vi<PW_VOLTAGV_IN_MIN)?1:0;  //输入欠压保护
		device.oocp = (power.Io>PW_CURRENT_OUT_MAX)?1:0; //输出过流保护
		device.oovp = (power.Vo>(PW_VOLTAGV_OUT_MAX+0.5f))?1:0; //输出过压保护
		
		if(device.iovp||device.iuvp||device.oocp||device.oovp)
		{
			PWM_Stop();
			device.recoveryCnt = 0;
			LED_OUT_OFF;
			LED_CV_CC_OFF;
		}
		else
		{	//故障恢复
			device.recoveryCnt = (device.recoveryCnt>device.faultTime)?device.recoveryCnt:device.recoveryCnt+1;
		}
	
		fb_io = ((float)power.channel[1] - calib.ioZero) * calib.ioRatio;
		fb_vo = power.channel[2] * calib.voRatio;
		if(fb_io<0)
		{
			fb_io = 0;
		}
		/* 大电流补偿压差 */
		fb_vo -= (fb_io * 0.208f);
		if(fb_vo<0)
		{
			fb_vo = 0;
		}
		/*  */
		
		/* 恒功率 */
		#if 0 
		fb_w = fb_vo*fb_io;
		if(fb_w>250.0f)
		{
			PWM_Stop();
			device.recoveryCnt = 0;
			LED_OUT_OFF;
			LED_CV_CC_OFF;
		}
		#endif
		/* */

		if(device.recoveryCnt==device.faultTime) //恢复PWM波形输出
		{
			pidClear(&vPID);
			pidClear(&iPID);
			device.controlPWM = 0;
			PWM_Start();
			PWM_Update(device.controlPWM);
			LED_OUT_ON;
		}
	
		if(device.recoveryCnt>=device.faultTime)
		{
			switch(device.controlMode)
			{
				case CV_MODE:
					device.controlPWM = pidUpdate(&vPID, vPID.desired, fb_vo);	//电压环PID计算					
					iPID.outLast1 = device.controlPWM;
					device.controlMode = (fb_io<iPID.desired)?CV_MODE:CC_MODE;
					if(cnt%50==0) { LED_CV_ON; }
					break;
				case CC_MODE:
					device.controlPWM = pidUpdate(&iPID, iPID.desired, fb_io);	//电流环PID计算	
					vPID.outLast1 = device.controlPWM;
					device.controlMode = (fb_vo>vPID.desired)?CV_MODE:CC_MODE;
					if(cnt%50==0) { LED_CC_ON; }
					break;
				default:
					device.controlMode = CV_MODE;
					break;
			}
			
			PWM_Update(device.controlPWM);
		}
	
		cnt++;
	}
}

/**
  * @brief  This function is freeRTOS task configuration.
  * @param  None
  * @retval None
  */
//uint32_t pps=0;
void FreeRTOS_Task_Config(void)
{

	taskENTER_CRITICAL();
	xTaskCreate(myDisplayTask, "MYDISPLAYTASK", 128, NULL, 1, NULL);
	xTaskCreate(myInputTask, "MYINPUTASK", 128, NULL, 2, NULL);
//	pps = xPortGetFreeHeapSize();
	taskEXIT_CRITICAL();
	
	/* Start scheduler */
	vTaskStartScheduler();
}















