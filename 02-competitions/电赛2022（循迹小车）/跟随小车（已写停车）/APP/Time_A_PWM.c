#include "Time_A_PWM.h"



/***********************************
//-------------------------------------------------------------------------------------------------------------------
//  @brief      PWM初始化
//  @param      ta_n            选择模块
//  @param      ta_chn              选择通道
//  @param      pwm_freq            需要设置的频率（单位HZ）
//  @param      pwm_duty            需要设置的占空比
//  @return     void
//  @since      v1.0
//  Sample usage:               Time_A_Pwm_Init(TA0,TA_ch0,50,500);		//ftm0模块，0通道，50HZ，百分之50的占空比
************************************/
void Time_A_Pwm_Init(TA_n  ta_n, TA_chn  ta_chn, unsigned long pwm_freq, unsigned short pwm_duty)
{
	Timer_A_PWMConfig My_Timer_A_PWMConfig;
	uint32_t pwm_smlk = 0;
	pwm_smlk = CS_getMCLK();
	if(pwm_duty >= Timer_A_PWM_MAX_DUTY)  pwm_duty = Timer_A_PWM_MAX_DUTY;
	
	if(pwm_freq >= pwm_smlk )  pwm_freq = pwm_smlk;
	
	(My_Timer_A_PWMConfig).clockSource = TIMER_A_CLOCKSOURCE_SMCLK;//选择时钟来源
	(My_Timer_A_PWMConfig).clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;//不分频
	(My_Timer_A_PWMConfig).timerPeriod = (pwm_smlk)/pwm_freq;//设置计数周期，也就是输出频率
	(My_Timer_A_PWMConfig).dutyCycle =(long)(((pwm_smlk)/pwm_freq)*(pwm_duty*(1.0f/Timer_A_PWM_MAX_DUTY)));//设置输出占空比
	(My_Timer_A_PWMConfig).compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;//选择输出模式
	
	
	switch(ta_n)
	{
	
		case TA0:
							switch(ta_chn)
							{
								
								case TA_ch1: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,GPIO_PIN4,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								case TA_ch2: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,GPIO_PIN5,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								case TA_ch3: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_3;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,GPIO_PIN6,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								case TA_ch4: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_4;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,GPIO_PIN7,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								default:
									break;
							
							}
						Timer_A_generatePWM(TIMER_A0_BASE,&My_Timer_A_PWMConfig);
						break;
	
		case TA1:
							switch(ta_chn)
							{
								
								case TA_ch1: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P7,GPIO_PIN7,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								case TA_ch2: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P7,GPIO_PIN6,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								case TA_ch3: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_3;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P7,GPIO_PIN5,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								case TA_ch4: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_4;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P7,GPIO_PIN4,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								default:
									break;
							}
						Timer_A_generatePWM(TIMER_A1_BASE,&My_Timer_A_PWMConfig);
						break;
	
		case TA2:
							switch(ta_chn)
							{
								
								case TA_ch1: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;//选择输出引脚 
															
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5,GPIO_PIN6,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								case TA_ch2: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5,GPIO_PIN7,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								
								default:
									break;
							}
						Timer_A_generatePWM(TIMER_A2_BASE,&My_Timer_A_PWMConfig);
						break;
		case TA3:
							switch(ta_chn)
							{
								case TA_ch1: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P10,GPIO_PIN5,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								case TA_ch2: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P8,GPIO_PIN2,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								case TA_ch3: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_3;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P9,GPIO_PIN2,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								case TA_ch4: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_4;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P9,GPIO_PIN3,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								default:
										break;
							}
						Timer_A_generatePWM(TIMER_A3_BASE,&My_Timer_A_PWMConfig);
						break;
	default:
		break;
	
	}
}

/***********************************
//-------------------------------------------------------------------------------------------------------------------
//  @brief      PWM初始化
//  @param      ta_n            选择模块
//  @param      ta_chn              选择通道
//  @param      pwm_duty            需要设置的占空比
//  @return     void
//  @since      v1.0
//  Sample usage:               Time_A_Pwm_Init(TA0,TA_ch0,50,500);		//ftm0模块，0通道，50HZ，百分之50的占空比
************************************/
void Time_A_Pwm_Duty(TA_n  ta_n, TA_chn  ta_chn, unsigned short pwm_duty)
{
	Timer_A_PWMConfig My_Timer_A_PWMConfig;
	uint32_t pwm_smlk = 0;
	pwm_smlk = CS_getMCLK();
	if(pwm_duty >= Timer_A_PWM_MAX_DUTY)  pwm_duty = Timer_A_PWM_MAX_DUTY;
	
	
	(My_Timer_A_PWMConfig).clockSource = TIMER_A_CLOCKSOURCE_SMCLK;//选择时钟来源
	(My_Timer_A_PWMConfig).clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;//不分频
	//(My_Timer_A_PWMConfig).timerPeriod = (pwm_smlk)/pwm_freq;//设置计数周期，也就是输出频率
	(My_Timer_A_PWMConfig).dutyCycle =(long)(((pwm_smlk)/(pwm_smlk/(My_Timer_A_PWMConfig).timerPeriod))*(pwm_duty*(1.0f/Timer_A_PWM_MAX_DUTY)));//设置输出占空比
	(My_Timer_A_PWMConfig).compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;//选择输出模式
	
	
	switch(ta_n)
	{
	
		case TA0:
							switch(ta_chn)
							{
								
								case TA_ch1: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,GPIO_PIN4,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								case TA_ch2: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,GPIO_PIN5,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								case TA_ch3: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_3;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,GPIO_PIN6,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								case TA_ch4: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_4;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,GPIO_PIN7,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								default:
									break;
							
							}
						Timer_A_generatePWM(TIMER_A0_BASE,&My_Timer_A_PWMConfig);
						break;
	
		case TA1:
							switch(ta_chn)
							{
								
								case TA_ch1: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P7,GPIO_PIN7,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								case TA_ch2: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P7,GPIO_PIN6,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								case TA_ch3: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_3;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P7,GPIO_PIN5,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								case TA_ch4: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_4;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P7,GPIO_PIN4,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								default:
									break;
							}
						Timer_A_generatePWM(TIMER_A1_BASE,&My_Timer_A_PWMConfig);
						break;
	
		case TA2:
							switch(ta_chn)
							{
								
								case TA_ch1: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;//选择输出引脚 
															
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5,GPIO_PIN6,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								case TA_ch2: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5,GPIO_PIN7,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								
								default:
									break;
							}
						Timer_A_generatePWM(TIMER_A2_BASE,&My_Timer_A_PWMConfig);
						break;
		case TA3:
							switch(ta_chn)
							{
								case TA_ch1: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P10,GPIO_PIN5,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								case TA_ch2: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P8,GPIO_PIN2,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								case TA_ch3: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_3;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P9,GPIO_PIN2,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								case TA_ch4: (My_Timer_A_PWMConfig).compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_4;//选择输出引脚 
															GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P9,GPIO_PIN3,GPIO_PRIMARY_MODULE_FUNCTION);//配置输出引脚	
															break;
								default:
										break;
							}
						Timer_A_generatePWM(TIMER_A3_BASE,&My_Timer_A_PWMConfig);
						break;
	default:
		break;
	
	}

}
