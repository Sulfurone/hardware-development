#include "mpuiic.h"
#include "generic.h"

#define MPUiicDelay 0   //延时时间

void MPU_IIC_Start(void)
{
	MPU_SDA_OUT();     //sda线输出
	MPU_IIC_SDA=1;	  	  
	MPU_IIC_SCL=1;
	Delay_us(MPUiicDelay);
 	MPU_IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	Delay_us(MPUiicDelay);
	MPU_IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}

void MPU_IIC_Stop(void)
{
	MPU_SDA_OUT();//sda线输出
	MPU_IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	Delay_us(MPUiicDelay);
	MPU_IIC_SCL=1; 
	MPU_IIC_SDA=1;//发送I2C总线结束信号
	Delay_us(MPUiicDelay);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t MPU_IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	MPU_SDA_IN();      //SDA设置为输入
	MPU_IIC_SDA=1;
	Delay_us(MPUiicDelay);
	MPU_IIC_SCL=1;
	Delay_us(MPUiicDelay);
	while(MPU_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250){
			MPU_IIC_Stop();
			return 1;
		}
	}
	MPU_IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 

void MPU_WriteAck(uint8_t AckBit){
	MPU_SDA_OUT();
	MPU_IIC_SDA=AckBit;
	Delay_us(MPUiicDelay);
	MPU_IIC_SCL=1;
	Delay_us(MPUiicDelay);
	MPU_IIC_SCL=0;
}
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void MPU_IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	MPU_SDA_OUT(); 	    
    //MPU_IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        MPU_IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		    MPU_IIC_SCL=1;
		    Delay_us(MPUiicDelay); 
		    MPU_IIC_SCL=0;	
		    Delay_us(MPUiicDelay);
    }
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t MPU_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	MPU_SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        Delay_us(MPUiicDelay);
		MPU_IIC_SCL=1;
        receive<<=1;
        if(MPU_READ_SDA)receive++;   
		Delay_us(MPUiicDelay); 
		MPU_IIC_SCL=0; 
    }					 
    if (!ack)
        MPU_WriteAck(1);//发送nACK
    else
        MPU_WriteAck(0); //发送ACK   
    return receive;
}

//IIC连续写
//addr:器件地址
//reg:寄存器地址
//len:写入长度
//buf:数据区
//返回值:0,正常
//    其他,错误代码
uint8_t MPU_Write_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{
	uint8_t i; 
    MPU_IIC_Start(); 
	MPU_IIC_Send_Byte(addr<<1);//发送器件地址+写命令	
	if(MPU_IIC_Wait_Ack())	//等待应答
	{
		printf("MPU_Write_Len中地址出现错误\n");
		MPU_IIC_Stop();		 
		return 1;
	}
    MPU_IIC_Send_Byte(reg);	//写寄存器地址
    MPU_IIC_Wait_Ack();		//等待应答
	for(i=0;i<len;i++)
	{
		MPU_IIC_Send_Byte(buf[i]);	//发送数据
		if(MPU_IIC_Wait_Ack())		//等待ACK
		{
			printf("MPU_Write_Len中数据发送失败\n");
			MPU_IIC_Stop();	 
			return 1;		 
		}
	}    
    MPU_IIC_Stop();
	return 0;	
} 
//IIC连续读
//addr:器件地址
//reg:要读取的寄存器地址
//len:要读取的长度
//buf:读取到的数据存储区
//返回值:0,正常
//    其他,错误代码
uint8_t MPU_Read_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{ 
 	MPU_IIC_Start(); 
	MPU_IIC_Send_Byte(addr<<1);//发送器件地址+写命令	
	if(MPU_IIC_Wait_Ack())	//等待应答
	{
		MPU_IIC_Stop();
		printf("MPU_Read_Len中地址出现错误\n");
		return 1;
	}
    MPU_IIC_Send_Byte(reg);	//写寄存器地址
    MPU_IIC_Wait_Ack();		//等待应答
    MPU_IIC_Start();
	MPU_IIC_Send_Byte((addr<<1)|1);//发送器件地址+读命令	
    MPU_IIC_Wait_Ack();		//等待应答 
	while(len)
	{
		if(len==1)*buf=MPU_IIC_Read_Byte(0);//读数据,发送nACK 
		else *buf=MPU_IIC_Read_Byte(1);		//读数据,发送ACK  
		len--;
		buf++; 
	}    
    MPU_IIC_Stop();	//产生一个停止条件 
	return 0;	
}
