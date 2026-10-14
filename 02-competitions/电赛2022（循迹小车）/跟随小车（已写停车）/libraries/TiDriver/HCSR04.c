
#include "HCSR04.h"

/**********  ?????   *************/
//P64 SCL P70 SDA
#define	SlaveAddress	0xAE	//IIC??????????,+1???

void SRDelay2us(void)
{
	u8 i;
	i = 11;		// @24MHZ, 6 + 33 = 39T, 1.625us
	while (--i){__NOP();}
}


void SRI2C_Start(void)
{
	SRDelay2us();                 //??
	SRDelay2us();                 //??
	GPIO_setOutputHighOnPin(GPIO_PORT_P7, GPIO_PIN0);
	//SDA = 1;                    //?????
	GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN4);
	//SCL = 1;                    //?????
	SRDelay2us();                 //??
	SRDelay2us();                 //??
	//SDA = 0;                    //?????
	GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN0);
	SRDelay2us();                 //??
	GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);
	//SCL = 0;                    //?????
	SRDelay2us();                 //??
}

//**************************************
//I2C????
//**************************************
void SRI2C_Stop(void)
{
    SRDelay2us();                 //??
    //SDA = 0;                    //?????
    GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN0);
    //SCL = 1;                    //?????
    GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN4);
	SRDelay2us();                 //??
    //SDA = 1;                    //?????
    GPIO_setOutputHighOnPin(GPIO_PORT_P7, GPIO_PIN0);
	SRDelay2us();                 //??
}

//**************************************
//I2C??????
//**************************************
uint8_t SRI2C_RecvACK(void)
{
    uint8_t CY;
	//SDA = 1;
	GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6);
	SRDelay2us();                 //??
    //SCL = 1;                    //?????
    GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN7);
    SRDelay2us();                 //??
    GPIO_setAsInputPin(GPIO_PORT_P1, GPIO_PIN6);
    CY = GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN6);                  //?????
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN6);
    //SCL = 0;                    //?????
    GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN7);
    SRDelay2us();                 //??
    return CY;
}


void SRI2C_SendByte(u8 dat)
{
    u8 i,CY;
    for (i=0; i<8; i++)         //8????
    {
        CY = dat & 0x80;
        dat <<= 1;              //????????
       // SDA = CY;               //????
        if(CY){
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6);
        }else{
            GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);
        }
        //SCL = 1;                //?????
        GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN7);
        SRDelay2us();             //??
        //SCL = 0;                //?????
        GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN7);
        SRDelay2us();             //??
    }
    SRI2C_RecvACK();
}


u8 SRI2C_RecvByte(void)
{
    u8 i;
    u8 dat = 0;
    //SDA = 1;                    //??????,??????,
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6);
    for (i=0; i<8; i++)         //8????
    {
        dat <<= 1;
        //SCL = 1;                //?????
        GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN7);
        SRDelay2us();             //??
        GPIO_setAsInputPin(GPIO_PORT_P1, GPIO_PIN6);
        dat |= GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN6);                  //?????
        GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN6);
       // dat |= SDA;             //???
        //SCL = 0;                //?????
        GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN7);
        SRDelay2us();             //??
    }
    return dat;
}

//**************************************
//?I2C??????????
//**************************************
void SRSingle_WriteI2C(u8 REG_Address,u8 REG_data)
{
    SRI2C_Start();                  //????
    SRI2C_SendByte(SlaveAddress);   //??????+???
    //I2C_SendByte(REG_Address);    //???????,
    SRI2C_SendByte(REG_data);       //???????,
    SRI2C_Stop();                   //??????
}

void InitSR04(void)
{
	//P2n_open_drain(0x30);	//P2.4 -- SDA,   P2.5 -- SCL
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN6);
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN7);
	SRDelay2us();
	SRDelay2us();
	SRDelay2us();
	SRDelay2us();
	SRSingle_WriteI2C(SlaveAddress,0x01);//??????;
}

void SR04Start(void)
{
    //P2n_open_drain(0x30); //P2.4 -- SDA,   P2.5 -- SCL
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN6);
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN7);
    SRDelay2us();
    SRDelay2us();
    SRDelay2us();
    SRDelay2us();
    SRSingle_WriteI2C(SlaveAddress,0x01);//??????;
}

void SR04Read(u8 *buf)
{
	u8	i;
	
	SRI2C_Start();                  //????
	//SRI2C_SendByte(SlaveAddress);   //??????+???
	//SRI2C_SendByte(ACCEL_XOUT_H);    //???????,
	//I2C_SendByte(WHO_AM_I);    //???????,
	//I2C_Start();                   //????
	SRI2C_SendByte(SlaveAddress+1);  //??????+???
	for(i=0; i<2; i++)
	//for(i=0; i<1; i++)
	{
		buf[i] = SRI2C_RecvByte();       //???????
		//SDA = 0;                    //?????
		GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);
		//SCL = 1;                    //?????
		GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN7);
		SRDelay2us();                 //??
		//SCL = 0;                    //?????
		GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN7);
		SRDelay2us();                 //??
	}
		buf[i] = SRI2C_RecvByte();	//??????
		//SDA = 1;                    //??????
		GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6);
		//SCL = 1;                    //?????
		GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN7);
		SRDelay2us();                 //??
		//SCL = 0;                    //?????
		GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN7);
		SRDelay2us();                 //??
	SRI2C_Stop();                    //????
}

unsigned long SR04Cal(u8 *buf){
    return ((unsigned long)buf[0]*65536+(unsigned long)buf[1]*256+(unsigned long)buf[2])/1000;
}
