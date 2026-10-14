/********************************************
*********************************************************/
#include "SR04.h"

/**********  寄存器地址   *************/
//P64 SCL P70 SDA
#define	SlaveAddress	0xAE	//IIC写入时的地址字节数据，+1为读取
/*------------------------------------------------------------------*/

//I^C时序中延时设置，具体参见各芯片的数据手册  6050推荐最小1.3us 但是会出问题，这里延时实际1.9us左右
void SRDelay2us(void)
{
	u8 i;
	i = 11;		// @24MHZ, 6 + 33 = 39T, 1.625us
	while (--i){_nop();}
}

//**************************************
//I2C起始信号
//**************************************
void SRI2C_Start(void)
{
	SRDelay2us();                 //延时
	SRDelay2us();                 //延时
	GPIO_setOutputHighOnPin(GPIO_PORT_P7, GPIO_PIN0);
	//SDA = 1;                    //拉高数据线
	GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN4);
	//SCL = 1;                    //拉高时钟线
	SRDelay2us();                 //延时
	SRDelay2us();                 //延时
	//SDA = 0;                    //产生下降沿
	GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN0);
	SRDelay2us();                 //延时
	GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);
	//SCL = 0;                    //拉低时钟线
	SRDelay2us();                 //延时
}

//**************************************
//I2C停止信号
//**************************************
void SRI2C_Stop(void)
{
    SRDelay2us();                 //延时
    //SDA = 0;                    //拉低数据线
    GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN0);
    //SCL = 1;                    //拉高时钟线
    GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN4);
	SRDelay2us();                 //延时
    //SDA = 1;                    //产生上升沿
    GPIO_setOutputHighOnPin(GPIO_PORT_P7, GPIO_PIN0);
	SRDelay2us();                 //延时
}

//**************************************
//I2C接收应答信号
//**************************************
uint8_t SRI2C_RecvACK(void)
{
    uint8_t CY;
	//SDA = 1;
	GPIO_setOutputHighOnPin(GPIO_PORT_P7, GPIO_PIN0);
	SRDelay2us();                 //延时
    //SCL = 1;                    //拉高时钟线
    GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN4);
    SRDelay2us();                 //延时
    GPIO_setAsInputPin(GPIO_PORT_P7, GPIO_PIN0);
    CY = GPIO_getInputPinValue(GPIO_PORT_P7, GPIO_PIN0);                  //读应答信号
    GPIO_setAsOutputPin(GPIO_PORT_P7, GPIO_PIN0);
    //SCL = 0;                    //拉低时钟线
    GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);
    SRDelay2us();                 //延时
    return CY;
}

//**************************************
//向I2C总线发送一个字节数据
//**************************************
void SRI2C_SendByte(u8 dat)
{
    u8 i,CY;
    for (i=0; i<8; i++)         //8位计数器
    {
        CY = dat & 0x80;
        dat <<= 1;              //移出数据的最高位
       // SDA = CY;               //送数据口
        if(CY){
            GPIO_setOutputHighOnPin(GPIO_PORT_P7, GPIO_PIN0);
        }else{
            GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN0);
        }
        //SCL = 1;                //拉高时钟线
        GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN4);
        SRDelay2us();             //延时
        //SCL = 0;                //拉低时钟线
        GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);
        SRDelay2us();             //延时
    }
    SRI2C_RecvACK();
}

//**************************************
//从I2C总线接收一个字节数据
//**************************************
u8 SRI2C_RecvByte(void)
{
    u8 i;
    u8 dat = 0;
    //SDA = 1;                    //使能内部上拉,准备读取数据,
    GPIO_setOutputHighOnPin(GPIO_PORT_P7, GPIO_PIN0);
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;
        //SCL = 1;                //拉高时钟线
        GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN4);
        SRDelay2us();             //延时
        GPIO_setAsInputPin(GPIO_PORT_P7, GPIO_PIN0);
        dat |= GPIO_getInputPinValue(GPIO_PORT_P7, GPIO_PIN0);                  //读应答信号
        GPIO_setAsOutputPin(GPIO_PORT_P7, GPIO_PIN0);
       // dat |= SDA;             //读数据
        //SCL = 0;                //拉低时钟线
        GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);
        SRDelay2us();             //延时
    }
    return dat;
}

//**************************************
//向I2C设备写入一个字节数据
//**************************************
void SRSingle_WriteI2C(u8 REG_Address,u8 REG_data)
{
    SRI2C_Start();                  //起始信号
    SRI2C_SendByte(SlaveAddress);   //发送设备地址+写信号
    //I2C_SendByte(REG_Address);    //内部寄存器地址，
    SRI2C_SendByte(REG_data);       //内部寄存器数据，
    SRI2C_Stop();                   //发送停止信号
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
//初始化SR04
//**************************************
void InitSR04(void)
{
	//P2n_open_drain(0x30);	//P2.4 -- SDA,   P2.5 -- SCL
    GPIO_setAsOutputPin(GPIO_PORT_P7, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN4);
	SRDelay2us();
	SRDelay2us();
	SRDelay2us();
	SRDelay2us();
	SRSingle_WriteI2C(SlaveAddress,0x01);//开启一次测量;
}

void SR04Start(void)
{
    //P2n_open_drain(0x30); //P2.4 -- SDA,   P2.5 -- SCL
    GPIO_setAsOutputPin(GPIO_PORT_P7, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN4);
    SRDelay2us();
    SRDelay2us();
    SRDelay2us();
    SRDelay2us();
    SRSingle_WriteI2C(SlaveAddress,0x01);//开启一次测量;
}

void SR04Read(u8 *buf)
{
	u8	i;
	
	SRI2C_Start();                  //起始信号
	//SRI2C_SendByte(SlaveAddress);   //发送设备地址+写信号
	//SRI2C_SendByte(ACCEL_XOUT_H);    //内部寄存器地址，
	//I2C_SendByte(WHO_AM_I);    //内部寄存器地址，
	//I2C_Start();                   //起始信号
	SRI2C_SendByte(SlaveAddress+1);  //发送设备地址+读信号
	for(i=0; i<2; i++)
	//for(i=0; i<1; i++)
	{
		buf[i] = SRI2C_RecvByte();       //读出寄存器数据
		//SDA = 0;                    //写应答信号
		GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN0);
		//SCL = 1;                    //拉高时钟线
		GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN4);
		SRDelay2us();                 //延时
		//SCL = 0;                    //拉低时钟线
		GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);
		SRDelay2us();                 //延时
	}
		buf[i] = SRI2C_RecvByte();	//最后一个字节
		//SDA = 1;                    //写非应答信号
		GPIO_setOutputHighOnPin(GPIO_PORT_P7, GPIO_PIN0);
		//SCL = 1;                    //拉高时钟线
		GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN4);
		SRDelay2us();                 //延时
		//SCL = 0;                    //拉低时钟线
		GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);
		SRDelay2us();                 //延时
	SRI2C_Stop();                    //停止信号
}

unsigned long SR04Cal(u8 *buf){
    return ((unsigned long)buf[0]*65536+(unsigned long)buf[1]*256+(unsigned long)buf[2])/1000;
}
