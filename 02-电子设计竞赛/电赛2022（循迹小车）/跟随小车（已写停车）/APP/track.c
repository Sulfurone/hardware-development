

#include "MOTOR.h"
#include "track.h"

uint16_t time1=0;
uint8_t   line_flag;            //停车标志位;

float  BASICPWM=0;              //  PWM输出值;
uint16_t       Pulse_Count;     //速度脉冲计数;
extern   uint8_t   angle;
extern   uint8_t   start_flag;
/*PID参数修改处*/
/*
float   kp = 150,
        ki = 0,
        kd = 0;
*/
float   kp = -100,
        ki = 0,
        kd = 0;

uint16_t  Ting=0; 



void Infrared_Init(void)
{
        /* Configuring P4 as an input and enabling interrupts */
        //4路红外循迹
         MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN0);//x4
         MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN1);//x3
         MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN2);//x2
         MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN3);//x1



}

void bsp_GetLineWalking(int *p_iL1, int *p_iL2, int *p_iR1, int *p_iR2)
{
    *p_iL1 = GPIO_getInputPinValue(GPIO_PORT_P4,GPIO_PIN2);   //左1
    *p_iL2 = GPIO_getInputPinValue(GPIO_PORT_P4,GPIO_PIN3);   //左2
    *p_iR1 = GPIO_getInputPinValue(GPIO_PORT_P4,GPIO_PIN0);   //右1
    *p_iR2 = GPIO_getInputPinValue(GPIO_PORT_P4,GPIO_PIN1);   //右2
}

void app_LineWalking(void)
{
    int LineL1 = 1, LineL2 = 1, LineR1 = 1, LineR2 = 1;

    bsp_GetLineWalking(&LineL1, &LineL2, &LineR1, &LineR2); //获取黑线检测状态

	
	time1++;
	if(time1>64&&time1<1000000000000000000)
 {
    Ting=1;
	  setPWM(0,0);    
	}

	
  if(Ting==0)
	{
     if (LineL1&&!LineL2&&!LineR1&&!LineR2) // 0 1  1  1
    {
			Car_Forward_Function();
			setPWM(0,350);

    }
    else if(LineL1&&LineL2&&!LineR1&&!LineR2)  // 0  0   1   1
    {
			Car_Forward_Function();
			setPWM(0,400);

    }
    else if(!LineL1&&LineL2&&!LineR1&&!LineR2)  // 1  0   1  1
    {
			Car_Forward_Function();
			setPWM(0,250);

    }
    else if(LineL1&&!LineL2&&LineR1&&!LineR2)  // 1  0   0  1
      {
			Car_Forward_Function();
			setPWM(0,0);

      }

    else if(!LineL1&&!LineL2&&LineR1&&!LineR2)   // 1  1   0   1
    {
			Car_Forward_Function();
			setPWM(200,0);
 }
//   bvn                                                                                              `

    else if(!LineL1&&!LineL2&&!LineR1&&LineR2)    // 1  1   1   0
    {
			Car_Forward_Function();
			setPWM(200,0);

    }

		else if(LineL1&&LineL2&&LineR1&&LineR2)   //4路黑  停车  0  0 0  0
    {
			
			Car_Forward_Function();
			setPWM(0,0);
			Ting=1;
        }
		
   
		 else 
    {
			Car_Forward_Function();
			setPWM(210,210);
		}
    }  
if(Ting==1)
{
		setPWM(0,0);
}
}
//void Tracking_Work(void)
//{
//    float  value;
//    float  left_value,right_value;
//		
//	
//  //  value=app_LineWalking();

////    left_value=100+value;          //匀速value为0，进入弯道Value起作用； 左轮加速
////    right_value=100-value;         //右轮减速

//    if(left_value>512)left_value=512;  //限幅防止PWM 超量程

//    if(left_value<0)left_value=0;

//    if(right_value>512)right_value=512;

//    if(right_value<0)right_value=0;

//		Car_Forward_Function();
//    setPWM(left_value,right_value); //PWM加载到电机上



