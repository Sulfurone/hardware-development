#include "stm32f10x.h"       
#include "generic.h"										//主要存储Delay等常用的
#include "OLED_I2C.h"
#include "MPU6050.h"
#include "Serial.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "Kalman_filter.h"

MPU6050 MM;	//创建一个结构体用来储存欧拉角

int main(void)
{
	/*取消注释1打开卡尔曼滤波
	  float newroll = 0;
	  float newpitch = 0;
    KalmanFilter KK1;
	  KalmanFilter KK2;
    KalmanFilter_Init(&KK2,0,0.01,0.1);
    KalmanFilter_Init(&KK1,0,0.01,0.1);
	*/
	Serial_Init(USART1,9600);//串口初始化
	OLED_Init(GPIOB,GPIO_Pin_10,GPIO_Pin_11);//初始OLED屏幕
	MPU6050_init(GPIOA,GPIO_Pin_0,GPIO_Pin_1);//初始化MPU6050(SCL:A0,SDA:A1)
	
	//mpu_dmp_init(GPIOA,GPIO_Pin_1,GPIO_Pin_0);//初始化dmp库,取消注释2
	while (1)
	{
		OLED_Clear();
		MPU6050_Get_Angle(&MM);
		//mpu_dmp_get_data(&MM.pitch,&MM.roll,&MM.yaw);注释2
		/*注释1
	 	newroll = KalmanFilter_Update(&KK1,MM.roll);
		newpitch = KalmanFilter_Update(&KK2,MM.pitch);
		printf("%.3f,%.3f,%.3f\n",newroll,newpitch,MM.yaw);
		OLED_Printf(0,0,6,"roll:%.2f",newroll);
		OLED_Printf(0,8,6,"pitch:%.2f",newpitch);
		OLED_Printf(0,16,6,"yaw:%.2f",MM.yaw);
		*/
		printf("%.3f,%.3f,%.3f\n",MM.roll,MM.pitch,MM.yaw);//串口发送数据
		OLED_Printf(0,0,6,"roll:%.2f",MM.roll);
		OLED_Printf(0,8,6,"pitch:%.2f",MM.pitch);
		OLED_Printf(0,16,6,"yaw:%.2f",MM.yaw);
		OLED_Update();
	}
}
