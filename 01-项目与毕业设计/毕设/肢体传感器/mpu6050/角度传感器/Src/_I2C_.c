#include "_I2C_.h"
#include "stdlib.h"

I2C_BUS* Pthis_I2C = 0;//全局指针
typedef struct I2C_Private
{
	GPIO_TypeDef* GPIOx;
	I2C_TypeDef* I2Cx;
	uint16_t SCL;
	uint16_t SDA;
	uint16_t I2C_Add;
	uint8_t Hard_I2C_EN;
}I2C_Private;
/////////////////////////////////////////////////软件IIC////////////////////////////////////////////////
void MyI2C_W_SCL(uint8_t BitValue){//SCL write data
	//GPIO_WriteBit(Pthis_I2C->Private->GPIOx, Pthis_I2C->Private->SCL, (BitAction)BitValue);
	if (BitValue != Bit_RESET)
    	Pthis_I2C->Private->GPIOx->BSRR = Pthis_I2C->Private->SCL;
  	else
   		Pthis_I2C->Private->GPIOx->BRR = Pthis_I2C->Private->SCL;

	Delay_us(SI2C_delay_time);
}

void MyI2C_W_SDA(uint8_t BitValue){//SDA write data
	//GPIO_WriteBit(Pthis_I2C->Private->GPIOx, Pthis_I2C->Private->SDA, (BitAction)BitValue);
	if (BitValue != Bit_RESET)
    	Pthis_I2C->Private->GPIOx->BSRR = Pthis_I2C->Private->SDA;
  	else
   		Pthis_I2C->Private->GPIOx->BRR = Pthis_I2C->Private->SDA;
	
	Delay_us(SI2C_delay_time);
}
//0成功
uint8_t MyI2C_R_SDA(){//SDA read data
	uint8_t BitValue = GPIO_ReadInputDataBit(Pthis_I2C->Private->GPIOx, Pthis_I2C->Private->SDA);
	Delay_us(SI2C_delay_time);
	return BitValue;
}

void SI2C_Start(){
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(0);
}

void SI2C_Stop(){
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(1);
}

void SI2C_WriteByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i ++)
	{
		MyI2C_W_SDA(Byte & (0x80 >> i));// for example 1010 1100  -> 1010 1100 -> 1010 1100
		Delay_us(SI2C_delay_time);					//             |              |      		|
		MyI2C_W_SCL(1);
		Delay_us(SI2C_delay_time);
		MyI2C_W_SCL(0);
		Delay_us(SI2C_delay_time);
	}
}

uint8_t SI2C_ReceiveByte(){
	uint8_t i, Byte = 0x00;
	MyI2C_W_SDA(1);
	for (i = 0; i < 8; i ++)
	{
		MyI2C_W_SCL(1);
		if (MyI2C_R_SDA() == 1){Byte |= (0x80 >> i);}
		MyI2C_W_SCL(0);
	}
	return Byte;
}
//0应答ACK , 1 is Nack
void SI2C_WriteAck(uint8_t AckBit){
	MyI2C_W_SDA(AckBit);
	MyI2C_W_SCL(1);
	MyI2C_W_SCL(0);
}

//1成功   0失败(failed)
uint8_t SI2C_ReceiveAck(){//receive ask
	uint8_t AckBit;
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	AckBit = MyI2C_R_SDA();
	MyI2C_W_SCL(0);
	return AckBit;
}
//1成功   0失败(failed)
uint8_t SI2C_ACK_Test(){
	uint8_t Ack;
	SI2C_Start();
	SI2C_WriteByte(Pthis_I2C->Private->I2C_Add);
	Ack = SI2C_ReceiveAck();
	SI2C_Stop();
	return Ack;
}

/////////////////////////////////////////////硬件IIC//////////////////////////////////////////////////////////
uint8_t I2C_AdressScan(){
	uint8_t address;
	if(Pthis_I2C->Private->Hard_I2C_EN){


	}else{//软件IIC
		for (address = 1; address < 128; address++) {//一般地址只有7位所以是128
			SI2C_Start();
			SI2C_WriteByte(address << 1);
			if (!SI2C_ReceiveAck()) {//如果扫描到地址
				SI2C_Stop();//结束通信
				return address;//返回地址(10进制)
			}
			SI2C_Stop();
		}
	}
	return 0;
}

void HI2C_Rest_Speed(uint32_t Speed){
	I2C_InitTypeDef I2C_InitStructure;
 	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
 	I2C_InitStructure.I2C_ClockSpeed = Speed;		
 	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
 	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
 	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
 	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
 	I2C_Init(Pthis_I2C->Private->I2Cx, &I2C_InitStructure);
}

void HI2C_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
{
	uint32_t Timeout;
	Timeout = 1000;									//缁欏畾瓒呮椂璁℃暟鏃堕棿
	while (I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS)	//寰?鐜?绛夊緟鎸囧畾浜嬩欢
	{
		Timeout --;										//绛夊緟鏃讹紝璁℃暟鍊艰嚜鍑?
		if (Timeout == 0)								//鑷?鍑忓埌0鍚庯紝绛夊緟瓒呮椂
			break;										
	}
}

void I2C_Write_Reg(uint8_t RegAddress, uint16_t Data){
	if(Pthis_I2C->Private->Hard_I2C_EN){//如果是硬件IIC
		I2C_GenerateSTART(Pthis_I2C->Private->I2Cx, ENABLE);										
		HI2C_WaitEvent(Pthis_I2C->Private->I2Cx, I2C_EVENT_MASTER_MODE_SELECT);					
		
		I2C_Send7bitAddress(Pthis_I2C->Private->I2Cx, Pthis_I2C->Private->I2C_Add, I2C_Direction_Transmitter);	
		HI2C_WaitEvent(Pthis_I2C->Private->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	
		
		I2C_SendData(Pthis_I2C->Private->I2Cx, RegAddress);											
		HI2C_WaitEvent(Pthis_I2C->Private->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING);			
		
		I2C_SendData(Pthis_I2C->Private->I2Cx, Data);												
		HI2C_WaitEvent(Pthis_I2C->Private->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED);				
		
		I2C_GenerateSTOP(Pthis_I2C->Private->I2Cx, ENABLE);	
		
	}
	else{//如果是软件IIC
		SI2C_Start();
      
		SI2C_WriteByte(Pthis_I2C->Private->I2C_Add);
		SI2C_ReceiveAck();

		SI2C_WriteByte(RegAddress);
		SI2C_ReceiveAck();

		if(Pthis_I2C->Mode16bit == 1){//如果是16位操作模式
			SI2C_WriteByte((uint8_t)Data>>8);//发送高位
			SI2C_WriteByte((uint8_t)(Data&0x00FF));//发送低位
		}else{
			SI2C_WriteByte((uint8_t)Data);
		}
		SI2C_ReceiveAck();
		SI2C_Stop();
	}
}

uint16_t I2C_Read_Reg(uint8_t RegAddress){
	uint16_t Data;
	if(Pthis_I2C->Private->Hard_I2C_EN){
		I2C_GenerateSTART(Pthis_I2C->Private->I2Cx, ENABLE);									
		HI2C_WaitEvent(Pthis_I2C->Private->I2Cx, I2C_EVENT_MASTER_MODE_SELECT);					
		
		I2C_Send7bitAddress(Pthis_I2C->Private->I2Cx, Pthis_I2C->Private->I2C_Add, I2C_Direction_Transmitter);	
		HI2C_WaitEvent(Pthis_I2C->Private->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	
		
		I2C_SendData(Pthis_I2C->Private->I2Cx, RegAddress);											
		HI2C_WaitEvent(Pthis_I2C->Private->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED);				
		
		I2C_GenerateSTART(Pthis_I2C->Private->I2Cx, ENABLE);									
		HI2C_WaitEvent(Pthis_I2C->Private->I2Cx, I2C_EVENT_MASTER_MODE_SELECT);					
		
		I2C_Send7bitAddress(Pthis_I2C->Private->I2Cx, Pthis_I2C->Private->I2C_Add, I2C_Direction_Receiver);		
		HI2C_WaitEvent(Pthis_I2C->Private->I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);		
		
		I2C_AcknowledgeConfig(Pthis_I2C->Private->I2Cx, DISABLE);									
		I2C_GenerateSTOP(Pthis_I2C->Private->I2Cx, ENABLE);											
		
		HI2C_WaitEvent(Pthis_I2C->Private->I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED);				
		Data = I2C_ReceiveData(Pthis_I2C->Private->I2Cx);											
		
		I2C_AcknowledgeConfig(Pthis_I2C->Private->I2Cx, ENABLE);		

		return Data;
	}else{//如果是软件IIC
		SI2C_Start();
		SI2C_WriteByte(Pthis_I2C->Private->I2C_Add);
		SI2C_ReceiveAck();
		SI2C_WriteByte(RegAddress);
		SI2C_ReceiveAck();

		SI2C_Start();
		SI2C_WriteByte(Pthis_I2C->Private->I2C_Add | 0x01);//|0x01读命令
		SI2C_ReceiveAck();
		if(Pthis_I2C->Mode16bit == 1){//如果是16位操作模式
			Data = (uint16_t)SI2C_ReceiveByte()<<8;
			Data |= SI2C_ReceiveByte();
		}else{
			Data = SI2C_ReceiveByte();
		}
		SI2C_WriteAck(1);//直接写1结束这次通信
		SI2C_Stop();
		return Data;
	}
}

/////////////////////////////////////连续转换模式(有BUG)///////////////////////////////////////////
// uint8_t I2C_Write_Reg_continue(uint8_t Device_Add,uint8_t RegAddress,uint8_t Count,uint8_t* Data){
// 	int i = 0;
// 	if(Pthis_I2C->Private->Hard_I2C_EN){
// 		I2C_GenerateSTART(Pthis_I2C->Private->I2Cx, ENABLE);										
// 		HI2C_WaitEvent(Pthis_I2C->Private->I2Cx, I2C_EVENT_MASTER_MODE_SELECT);					
		
// 		I2C_Send7bitAddress(Pthis_I2C->Private->I2Cx, Device_Add<<1, I2C_Direction_Transmitter);	
// 		HI2C_WaitEvent(Pthis_I2C->Private->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	
		
// 		I2C_SendData(Pthis_I2C->Private->I2Cx, RegAddress);											
// 		HI2C_WaitEvent(Pthis_I2C->Private->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING);			
// 		for(;i<Count;i++){
// 			I2C_SendData(Pthis_I2C->Private->I2Cx, Data[i]);												
// 			HI2C_WaitEvent(Pthis_I2C->Private->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED);	
// 		}	
// 		I2C_GenerateSTOP(Pthis_I2C->Private->I2Cx, ENABLE);
// 		return 0;
// 	}
// 	else{
// 		SI2C_Start();
// 		SI2C_WriteByte((Device_Add<<1)|0);
// 		if(SI2C_ReceiveAck()){
// 			printf("I2C初始化地址失败\n");
// 			SI2C_Stop();
// 			return 1;
// 		}
// 		SI2C_WriteByte(RegAddress);
// 		SI2C_ReceiveAck();
// 		for(;i<Count;i++){
// 			SI2C_WriteByte(Data[i]);
// 			if(SI2C_ReceiveAck()){
// 				printf("I2C没有接受应答\n");
// 				SI2C_Stop();
// 				return 1;
// 			}
// 		}
// 		SI2C_Stop();
// 		return 0;
// 	}
// }

// uint8_t I2C_Read_Reg_continue(uint8_t Device_Add,uint8_t RegAddress,uint8_t Count,uint8_t* Data){
// 	int i = 0;
// 	if(Pthis_I2C->Private->Hard_I2C_EN){
// 		I2C_GenerateSTART(Pthis_I2C->Private->I2Cx, ENABLE);									
// 		HI2C_WaitEvent(Pthis_I2C->Private->I2Cx, I2C_EVENT_MASTER_MODE_SELECT);					
		
// 		I2C_Send7bitAddress(Pthis_I2C->Private->I2Cx, Device_Add<<1, I2C_Direction_Transmitter);	
// 		HI2C_WaitEvent(Pthis_I2C->Private->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	
		
// 		I2C_SendData(Pthis_I2C->Private->I2Cx, RegAddress);											
// 		HI2C_WaitEvent(Pthis_I2C->Private->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED);				
		
// 		I2C_GenerateSTART(Pthis_I2C->Private->I2Cx, ENABLE);									
// 		HI2C_WaitEvent(Pthis_I2C->Private->I2Cx, I2C_EVENT_MASTER_MODE_SELECT);					
		
// 		I2C_Send7bitAddress(Pthis_I2C->Private->I2Cx, Device_Add<<1, I2C_Direction_Receiver);		
// 		HI2C_WaitEvent(Pthis_I2C->Private->I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);		
		
// 		I2C_AcknowledgeConfig(Pthis_I2C->Private->I2Cx, DISABLE);									
// 		I2C_GenerateSTOP(Pthis_I2C->Private->I2Cx, ENABLE);											
// 		for(;i<Count;i++){
// 			HI2C_WaitEvent(Pthis_I2C->Private->I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED);				
// 			Data[i] = I2C_ReceiveData(Pthis_I2C->Private->I2Cx);											
// 		}
// 		I2C_AcknowledgeConfig(Pthis_I2C->Private->I2Cx, ENABLE);
// 		return 0;		
// 	}else{
// 		SI2C_Start();
// 		SI2C_WriteByte(Device_Add<<1);
// 		if(SI2C_ReceiveAck()){
// 			printf("I2C地址出错\n");
// 			SI2C_Stop();
// 			return 1;
// 		}
// 		SI2C_WriteByte(RegAddress);
// 		SI2C_ReceiveAck();

// 		SI2C_Start();
// 		SI2C_WriteByte((Device_Add<<1) | 0x01);//|0x01灏嗘渶浣庝綅缃?1锛屼唬琛ㄨ?绘搷浣?
// 		SI2C_ReceiveAck();
// 		for(;i<Count;i++){
// 			Data[i] = SI2C_ReceiveByte();
// 			SI2C_WriteAck(0);
// 		}
// 		SI2C_WriteAck(1);
// 		SI2C_Stop();
// 		return 0;
// 	}
// }
///////////////////////////////////////////////////////////////////////////////////////////////////////
/*
GPIOx:选择你的GPIO口
SCL:时钟线(必须同一个GPIO口)
SDA:数据线(必须同一个GPIO口)
Address:一般是地址没有进行移位过的
*/
I2C_BUS Create_SI2C(GPIO_TypeDef* GPIOx,uint16_t SCL,uint16_t SDA,uint8_t Address){
	struct I2C_BUS this;
	this.Private = malloc(sizeof(I2C_Private));//开辟堆区，这里没有进行内存释放，不过单片机复位之后堆区会重新初始化别害怕
	this.Private->I2C_Add = Address<<1;
	this.Private->GPIOx = GPIOx;
	this.Private->SCL = SCL;
	this.Private->SDA = SDA;
	this.Private->I2Cx = 0;
	this.Private->Hard_I2C_EN = 0;
	
	this.ScanAdress = I2C_AdressScan;
	this.AckTest = SI2C_ACK_Test;
	this.Mode16bit = 0;
	this.Write_Reg = I2C_Write_Reg;
	this.Read_Reg = I2C_Read_Reg;
	this.Rest_Speed = 0;
	// this.Read_Reg_continue = I2C_Read_Reg_continue;
	// this.Write_Reg_continue = I2C_Write_Reg_continue;
	this.Read_Reg_continue = 0;
	this.Write_Reg_continue = 0;

	if(GPIOx==GPIOA)RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	else if(GPIOx==GPIOB)RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	else if(GPIOx==GPIOC)RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	else if(GPIOx==GPIOD)RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	else if(GPIOx==GPIOE)RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	else if(GPIOx==GPIOF)RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);
	else if(GPIOx==GPIOG)RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);
	
	GPIO_InitTypeDef GPIO_Init_Struct;
	GPIO_Init_Struct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init_Struct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init_Struct.GPIO_Pin = SCL|SDA;
	GPIO_Init(GPIOx,&GPIO_Init_Struct);

	GPIO_WriteBit(GPIOx, SCL, (BitAction)1);
	GPIO_WriteBit(GPIOx, SDA, (BitAction)1);

	return this;
}
/*
I2Cx:I2C1 I2C2选择硬件I2C
Address:一般是地址没有进行移位过的7位地址
*/
I2C_BUS Create_HI2C(I2C_TypeDef* I2Cx,uint8_t Address){
	struct I2C_BUS this;
	this.Private = malloc(sizeof(I2C_Private));//给私有成员开辟堆区
	this.Private->I2C_Add = Address<<1;
	this.Private->GPIOx = 0;
	this.Private->SCL = 0;
	this.Private->SDA = 0;
	this.Private->I2Cx = I2Cx;
	this.Private->Hard_I2C_EN = 1;
	
	this.ScanAdress = I2C_AdressScan;
	this.AckTest = SI2C_ACK_Test;
	this.Mode16bit = 0;
	this.Write_Reg = I2C_Write_Reg;
	this.Read_Reg = I2C_Read_Reg;
	this.Rest_Speed = HI2C_Rest_Speed;
	// this.Read_Reg_continue = I2C_Read_Reg_continue;
	// this.Write_Reg_continue = I2C_Write_Reg_continue;
	this.Read_Reg_continue = 0;
	this.Write_Reg_continue = 0;

	if(I2Cx==I2C1)RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	else RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;//!!!!!! OD mode!!!!!!!!开漏模式
	if(I2Cx==I2C1)GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	else GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;//Mode
	I2C_InitStructure.I2C_ClockSpeed = 200000;//I2C速度设置
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;	
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_Init(I2Cx, &I2C_InitStructure);

	I2C_Cmd(I2Cx, ENABLE);

	return this;
}
