//#include "calibration.h"
//#include "delay.h"
//#include "pwm.h"
////#include "led.h"
//#include "oled.h"
//#include "adc.h"
//#include "key.h"

//_uCalibration_Type	calib;

///**
//  * @brief  This function is...
//  * @param  None
//  * @retval None
//  */
//void CalibrationOutput(void)
//{
//	uint8_t calibFlag=0;
//	float v=0.0f;
//	float i=0.0f;
//	uint8_t keyCode=0;
//	
//	//输出校准模式
//	//电压15.000V校准
//	//调节输入电压使输出电压稳定在15.000V,按"切换"键缓存校准电压值,并进入电流校准
//	//电流1.000A校准
//	//使用电子负载恒流输出1.000A,按"切换"键缓存校准电流值,并保存电压和电流的校准值至flash
////	
////	OLED_ClearAll();
////	OLED_ShowString(24, 0, "Calib Mode", 10, 16);
////	OLED_ShowString(0, 3, "Buck Vout 15V", 13, 16);
////	
////	PWM_Start();
////	PWM_PULSE = (uint16_t)(0.75f*pwm.period);
//	
//	while(1)
//	{
//		i = i*0.7f + 0.3f*power.channel[1];
//		v = v*0.7f + 0.3f*power.channel[2];

//		keyCode = Key_Scan();
//		if(keyCode==KEY_VI_FLAG)
//		{
//////			BUZZER_ON;
////			if(calibFlag==0)
////			{	//加入电压校准值到缓存
////				calibFlag = 1;
////				calib.voRatio = CALIBRATION_V / v;
////				OLED_ShowString(0, 3, "Buck Iout 0A ", 13, 16);
////			}
////			else if(calibFlag==1)
////			{	//加入零点电流校准值到缓存
////				calibFlag = 2;
////				calib.ioZero = i;
////				OLED_ShowString(0, 3, "Buck Iout 2A ", 13, 16);
////			}
////			else if(calibFlag==2)
////			{	//加入1A电流校准值到缓存
////				calibFlag = 3;
////				calib.ioRatio = CALIBRATION_I / (i-calib.ioZero);
////				break;
////			}
////			delay_us(120000);
////		}
////		delay_us(500);
////	}
////	
////	//
////	PWM_PULSE = 0;
////	
////	delay_us(500000);
////	//保存校准数据
////	Save_CalibData();
////	//复位
////	NVIC_SystemReset();
//	while(1)
//	{}
//	
//}

///**
//  * @brief  This function is...
//  * @param  None
//  * @retval None
//  */
//void Save_CalibData(void)
//{
//	uint8_t cnt=0;
//	uint32_t temp=0;
//	
//	/* Unlock the Flash to enable the flash control register access *************/ 
//	FLASH_Unlock();

//	/* Clear pending flags (if any) */  
//	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 
//	
//	if(FLASH_ErasePage(FLASH_USER_START_ADDR)!= FLASH_COMPLETE)
//    {	//擦除故障
//		while(1)
//		{	
////			BUZZER_ON;
////			LED_OUT_TOGGLE;
//			delay_us(800000);
//		}
//    }
//	
//	//保存校准电压比率
//	temp = (uint32_t)(100000 * calib.voRatio);
//	if(FLASH_ProgramWord(FLASH_USER_START_ADDR, temp)==FLASH_COMPLETE)
//	{
//		cnt++;
//	}
//	
//	//保存零点电流
//	temp = (uint32_t)(100000 * calib.ioZero);
//	if(FLASH_ProgramWord(FLASH_USER_START_ADDR+4, temp)==FLASH_COMPLETE)
//	{
//		cnt++;
//	}
//	
//	//保存校准电流比率
//	temp = (uint32_t)(100000 * calib.ioRatio);
//	if(FLASH_ProgramWord(FLASH_USER_START_ADDR+8, temp)==FLASH_COMPLETE)
//	{
//		cnt++;
//	}
//	
//	FLASH_Lock();
//	
//	if(cnt==3)
//	{	//保存正确
////		BUZZER_ON;
////		delay_us(120000);
////		BUZZER_ON;
////		delay_us(120000);
//	}
//	else
//	{	//保存故障
//		while(1)
//		{	
////			BUZZER_ON;
////			LED_OUT_TOGGLE;
//			delay_us(800000);
//		}
//	}
//	
//}

///**
//  * @brief  This function is...
//  * @param  None
//  * @retval None
//  */
//void Read_CalibData(void)
//{
//	uint32_t temp=0;
//	uint32_t address=FLASH_USER_START_ADDR;
//	
//	//读电压
//	temp = *(__IO uint32_t *)address;
//	calib.voRatio = (float)temp / 100000;
//	calib.viRatio = calib.voRatio;

//	//读零点电流
//	address += 4;
//	temp = *(__IO uint32_t *)address;
//	calib.ioZero = (float)temp / 100000;
//	
//	//读电流
//	address += 4;
//	temp = *(__IO uint32_t *)address;
//	calib.ioRatio = (float)temp / 100000;
//}












