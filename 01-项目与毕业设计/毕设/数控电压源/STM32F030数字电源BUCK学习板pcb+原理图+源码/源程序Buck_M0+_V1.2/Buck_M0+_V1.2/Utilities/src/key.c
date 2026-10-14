#include "key.h"
#include "pid.h"
#include "adc.h"
#include "delay.h"

#include "control.h"
#include "calibration.h"


_uEncoder_Type	ec11;


/**
  * @brief  Key configuration
  * @param  None
  * @retval None
  */
void Key_Config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;

	/* GPIOA、B Periph clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	
	/* Key */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_11|GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* Buzzer */
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	ec11.active = false;
	ec11.set_vi = 1;	//默认调节电压
	ec11.step = 0.1f;	//默认步进0.1
	ec11.trigDis = false;
	ec11.set_i = iPID.desired;
	ec11.set_v = vPID.desired;
	
	delay_us(50000);
}

/**
  * @brief  Key scan
  * @param  None
  * @retval None
  */
uint8_t Key_Scan(void)
{
	static uint8_t keyUpState=0x00;
	uint8_t keyNum=0;
	
	if(VI_KEY==Bit_RESET)
	{
		keyNum = KEY_VI_FLAG;
	}
	else if(OUT_KEY==Bit_RESET)
	{
		keyNum = KEY_OUT_FLAG;
	}
	else if(D_EC11==Bit_RESET)
	{
		keyNum = KEY_RANGE_FLAG;
	}

	if(keyUpState&&keyNum)
	{
		keyUpState = 0;
		return keyNum;
	}
	else if(keyNum==0)
	{
		keyUpState = 0xFF;
	}
	
	return 0;
}


/**
  * @brief  This function ...
  * @param  None
  * @retval None
  */
uint8_t EC11Code_Scan(void)
{
	static uint8_t isEC11StartActive=0;
//	float isTemp=0;
	
	if(A_EC11==Bit_RESET && B_EC11==Bit_RESET)
	{
		isEC11StartActive = 1;
		return 0;
	}

	if(A_EC11!=Bit_RESET && B_EC11!=Bit_RESET && isEC11StartActive)
	{
		BUZZER_ON;
		ec11.active = true;
		if(ec11.set_vi!=0)
		{	//设置电压
			ec11.set_v += ec11.step;
			//isTemp = (float)((uint16_t)(power.Vi* 9)) / 10;
			vPID.desired = (ec11.set_v>PW_VOLTAGV_OUT_MAX)?PW_VOLTAGV_OUT_MAX:ec11.set_v;
			ec11.set_v = vPID.desired;
		}
		else
		{	//设置电流
			ec11.set_i += ec11.step;
			iPID.desired = (ec11.set_i>PW_CURRENT_OUT_MAX)?PW_CURRENT_OUT_MAX:ec11.set_i;
			ec11.set_i = iPID.desired;
		}
	}
	
	if(A_EC11!=Bit_RESET && B_EC11==Bit_RESET && isEC11StartActive)
	{
		BUZZER_ON;
		ec11.active = true;
		if(ec11.set_vi!=0)
		{	//设置电压
			ec11.set_v -= ec11.step;
			vPID.desired = (ec11.set_v<0)?0:ec11.set_v;
			ec11.set_v = vPID.desired;
		}
		else
		{	//设置电流
			ec11.set_i -= ec11.step;
			iPID.desired = (ec11.set_i<0)?0:ec11.set_i;
			ec11.set_i = iPID.desired;
		}
	}
	
	isEC11StartActive = 0;
	
	return 0;
}


/**
  * @brief  read calibration key
  * @param  None
  * @retval None
  */
void Read_CalibKey(void)
{
	if(VI_KEY!=Bit_RESET && OUT_KEY==Bit_RESET && D_EC11==Bit_RESET)
	{
		delay_us(1000000);
		BUZZER_ON;
		delay_us(100000);
		BUZZER_ON;
		if(VI_KEY!=Bit_RESET && OUT_KEY==Bit_RESET && D_EC11==Bit_RESET)
		{
			CalibrationOutput();
		}
	}
	
}




