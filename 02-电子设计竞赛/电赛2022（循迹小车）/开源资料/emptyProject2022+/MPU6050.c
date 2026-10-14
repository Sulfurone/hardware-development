/********************************************
*********************************************************/
#include "MPU6050.h"

/**********  寄存器地址   *************/
#define	SMPLRT_DIV		0x19	//陀螺仪采样率，典型值：0x07(125Hz)
#define	CONFIG			0x1A	//低通滤波频率，典型值：0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	ACCEL_CONFIG	0x1C	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42
#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48
#define	PWR_MGMT_1		0x6B	//电源管理，典型值：0x00(正常启用)
#define	WHO_AM_I		  0x75	//IIC地址寄存器(默认数值0x68，只读)
#define	SlaveAddress	0xD0	//IIC写入时的地址字节数据，+1为读取
#define IICSPEED      0x24
/*------------------------------------------------------------------*/

//I^C时序中延时设置，具体参见各芯片的数据手册  6050推荐最小1.3us 但是会出问题，这里延时实际1.9us左右
void Delay2us(void)
{
	u8 i;
	i = 11;		// @24MHZ, 6 + 33 = 39T, 1.625us
	while (--i){_nop();}
}

//**************************************
//I2C起始信号
//**************************************
void I2C_Start(void)
{
	Delay2us();                 //延时
	Delay2us();                 //延时
	GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN2);
	//SDA = 1;                    //拉高数据线
	GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
	//SCL = 1;                    //拉高时钟线
	Delay2us();                 //延时
	Delay2us();                 //延时
	//SDA = 0;                    //产生下降沿
	GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
	Delay2us();                 //延时
	GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
	//SCL = 0;                    //拉低时钟线
	Delay2us();                 //延时
}

//**************************************
//I2C停止信号
//**************************************
void I2C_Stop(void)
{
    Delay2us();                 //延时
    //SDA = 0;                    //拉低数据线
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
    //SCL = 1;                    //拉高时钟线
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
	Delay2us();                 //延时
    //SDA = 1;                    //产生上升沿
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN2);
	Delay2us();                 //延时
}

//**************************************
//I2C接收应答信号
//**************************************
uint8_t I2C_RecvACK(void)
{
    uint8_t CY;
	//SDA = 1;
	GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN2);
	Delay2us();                 //延时
    //SCL = 1;                    //拉高时钟线
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
    Delay2us();                 //延时
    GPIO_setAsInputPin(GPIO_PORT_P4, GPIO_PIN2);
    CY = GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN2);                  //读应答信号
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN2);
    //SCL = 0;                    //拉低时钟线
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
    Delay2us();                 //延时
    return CY;
}

//**************************************
//向I2C总线发送一个字节数据
//**************************************
void I2C_SendByte(u8 dat)
{
    u8 i,CY;
    for (i=0; i<8; i++)         //8位计数器
    {
        CY = dat & 0x80;
        dat <<= 1;              //移出数据的最高位
       // SDA = CY;               //送数据口
        if(CY){
            GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN2);
        }else{
            GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
        }
        //SCL = 1;                //拉高时钟线
        GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
        Delay2us();             //延时
        //SCL = 0;                //拉低时钟线
        GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
        Delay2us();             //延时
    }
    I2C_RecvACK();
}

//**************************************
//从I2C总线接收一个字节数据
//**************************************
u8 I2C_RecvByte(void)
{
    u8 i;
    u8 dat = 0;
    //SDA = 1;                    //使能内部上拉,准备读取数据,
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN2);
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;
        //SCL = 1;                //拉高时钟线
        GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
        Delay2us();             //延时
        GPIO_setAsInputPin(GPIO_PORT_P4, GPIO_PIN2);
        dat |= GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN2);                  //读应答信号
        GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN2);
       // dat |= SDA;             //读数据
        //SCL = 0;                //拉低时钟线
        GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
        Delay2us();             //延时
    }
    return dat;
}

//**************************************
//向I2C设备写入一个字节数据
//**************************************
void Single_WriteI2C(u8 REG_Address,u8 REG_data)
{
    I2C_Start();                  //起始信号
    I2C_SendByte(SlaveAddress);   //发送设备地址+写信号
    I2C_SendByte(REG_Address);    //内部寄存器地址，
    I2C_SendByte(REG_data);       //内部寄存器数据，
    I2C_Stop();                   //发送停止信号
}
//**************************************
//从I2C设备读取一个字节数据
//**************************************
/*
u8 Single_ReadI2C(u8 REG_Address)
{
	u8 REG_data;
	I2C_Start();                   //起始信号
	I2C_SendByte(SlaveAddress);    //发送设备地址+写信号
	I2C_SendByte(REG_Address);     //发送存储单元地址，从0开始
	I2C_Start();                   //起始信号
	I2C_SendByte(SlaveAddress+1);  //发送设备地址+读信号
	REG_data=I2C_RecvByte();       //读出寄存器数据
	
	SDA = 1;                    //写应答信号
	SCL = 1;                    //拉高时钟线
	Delay2us();                 //延时
	SCL = 0;                    //拉低时钟线
	Delay2us();                 //延时
	
	I2C_Stop();                    //停止信号
	return REG_data;
}
*/

//**************************************
//初始化MPU6050
//**************************************
void InitMPU6050(void)
{
	//P2n_open_drain(0x30);	//P2.4 -- SDA,   P2.5 -- SCL
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN1+GPIO_PIN2);
	Delay2us();
	Delay2us();
	Delay2us();
	Delay2us();

	Single_WriteI2C(PWR_MGMT_1, 0x00);	//解除休眠状态
	Single_WriteI2C(SMPLRT_DIV, 0x07);  //陀螺仪125hz
	Single_WriteI2C(CONFIG, 0x04);      //21HZ滤波 延时A8.5ms G8.3ms  此处取值应相当注意，延时与系统周期相近为宜
	Single_WriteI2C(GYRO_CONFIG, 0x08); //陀螺仪500度/S 65.5LSB/g
	Single_WriteI2C(ACCEL_CONFIG, 0x08);//加速度+-4g  8192LSB/g
}

void Read_MPU6050(u8 *buf)
{
	u8	i;
	
	I2C_Start();                  //起始信号
	I2C_SendByte(SlaveAddress);   //发送设备地址+写信号
	I2C_SendByte(ACCEL_XOUT_H);    //内部寄存器地址，
	//I2C_SendByte(WHO_AM_I);    //内部寄存器地址，
	I2C_Start();                   //起始信号
	I2C_SendByte(SlaveAddress+1);  //发送设备地址+读信号
	for(i=0; i<13; i++)
	//for(i=0; i<1; i++)
	{
		buf[i] = I2C_RecvByte();       //读出寄存器数据
		//SDA = 0;                    //写应答信号
		GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
		//SCL = 1;                    //拉高时钟线
		GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
		Delay2us();                 //延时
		//SCL = 0;                    //拉低时钟线
		GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
		Delay2us();                 //延时
	}
		buf[i] = I2C_RecvByte();	//最后一个字节
		//SDA = 1;                    //写非应答信号
		GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN2);
		//SCL = 1;                    //拉高时钟线
		GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
		Delay2us();                 //延时
		//SCL = 0;                    //拉低时钟线
		GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
		Delay2us();                 //延时
	I2C_Stop();                    //停止信号
}
