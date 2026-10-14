#include "stm32f10x.h"       
#include "generic.h"										//主要存储Delay等常用的
#include "OLED_I2C.h"
#include "MPU6050.h"
#include "Serial.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "Kalman_filter.h"
#include "string.h"
#include "uart2.h"
#include "hc05.h"


MPU6050 MM;	//创建一个结构体用来储存欧拉角
uint8_t RxSTA = 1;
// 新增：保存上一次的角速度
static float lastGyroX = 0, lastGyroY = 0, lastGyroZ = 0;

#define FILTER_N 10
#define ACC_FILTER_N 10
static float gyroX_buf[FILTER_N] = {0};
static float gyroY_buf[FILTER_N] = {0};
static float gyroZ_buf[FILTER_N] = {0};
static float accX_buf[ACC_FILTER_N] = {0};
static float accY_buf[ACC_FILTER_N] = {0};
static float accZ_buf[ACC_FILTER_N] = {0};
static int filter_index = 0;
static int acc_filter_index = 0;

/**
 * @brief  对陀螺仪数据做均值滤波
 * @param  newVal 新采样值
 * @param  buf    滤波缓冲区
 * @retval        滤波后的值
 */
float filter_gyro(float newVal, float *buf) {
    buf[filter_index] = newVal;
    float sum = 0;
    for(int i=0; i<FILTER_N; i++) sum += buf[i];
    return sum / FILTER_N;
}

/**
 * @brief  对角加速度数据做均值滤波
 * @param  newVal 新采样值
 * @param  buf    滤波缓冲区
 * @retval        滤波后的值
 */
float filter_acc(float newVal, float *buf) {
    buf[acc_filter_index] = newVal;
    float sum = 0;
    for(int i=0; i<ACC_FILTER_N; i++) sum += buf[i];
    return sum / ACC_FILTER_N;
}

int main(void)
{

	  float newroll = 0;
	  float newpitch = 0;
    KalmanFilter KK1;
	  KalmanFilter KK2;
    KalmanFilter_Init(&KK2,0,0.01,0.1);
    KalmanFilter_Init(&KK1,0,0.01,0.1);

	Serial_Init(USART1,9600);//串口初始化
	OLED_Init(GPIOB,GPIO_Pin_10,GPIO_Pin_11);//初始OLED屏幕*(sda11,scl10)
	MPU6050_init(GPIOA,GPIO_Pin_0,GPIO_Pin_1);//初始化MPU6050(SCL:A0,SDA:A1)
	HC05_Init();
	while (1)
	{
//		float dt = 0.02f; // 20ms，实际可根据你的主循环速度调整

//		OLED_Clear();
//		MPU6050_Get_Angle(&MM);

//		// 获取陀螺仪原始数据
//		MPU6050_raw raw;
//		MPU6050_Get_Raw(&raw);

//		// 角速度单位转换（假设量程±2000°/s，16位数据）
//		float gyroX = raw.GyroX * 2000.0f / 32768.0f;
//		float gyroY = raw.GyroY * 2000.0f / 32768.0f;
//		float gyroZ = raw.GyroZ * 2000.0f / 32768.0f;

//		// 三轴都做均值滤波
//		filter_index = (filter_index + 1) % FILTER_N;
//		float gyroX_f = filter_gyro(gyroX, gyroX_buf);
//		float gyroY_f = filter_gyro(gyroY, gyroY_buf);
//		float gyroZ_f = filter_gyro(gyroZ, gyroZ_buf);

//		// 计算角加速度
//		float accGyroX = (gyroX_f - lastGyroX) / dt;
//		float accGyroY = (gyroY_f - lastGyroY) / dt;
//		float accGyroZ = (gyroZ_f - lastGyroZ) / dt;

//		// 更新上一次的角速度
//		lastGyroX = gyroX_f;
//		lastGyroY = gyroY_f;
//		lastGyroZ = gyroZ_f;

//		// 角加速度再做一次均值滤波
//		acc_filter_index = (acc_filter_index + 1) % ACC_FILTER_N;
//		float accGyroX_f = filter_acc(accGyroX, accX_buf);
//		float accGyroY_f = filter_acc(accGyroY, accY_buf);
//		float accGyroZ_f = filter_acc(accGyroZ, accZ_buf);

//		// 显示欧拉角
//		OLED_Printf(0,0,6,"roll:%.2f",MM.roll);
//		OLED_Printf(0,8,6,"pitch:%.2f",MM.pitch);
//		OLED_Printf(0,16,6,"yaw:%.2f",MM.yaw);

//		// 显示角加速度（用滤波后的）
//		OLED_Printf(0,24,6,"aGx:%.2f", accGyroX_f);
//		OLED_Printf(0,32,6,"aGy:%.2f", accGyroY_f);
//		OLED_Printf(0,40,6,"aGz:%.2f", accGyroZ_f);

//		OLED_Update();

		// 串口输出（可选）
    //		printf("aGx:%.2f, aGy:%.2f, aGz:%.2f\n", accGyroX_f, accGyroY_f, accGyroZ_f);
    // printf("roll:%.2f, pitch:%.2f, yaw:%.2f\n", MM.roll, MM.pitch, MM.yaw);
//    printf("%.3f,%.3f,%.3f\n",MM.roll,MM.pitch,MM.yaw);//串口发送数据
    HC05_SendString("11111");
		Delay_ms(20); // 建议加延时，保证dt与实际一致
	}
}
