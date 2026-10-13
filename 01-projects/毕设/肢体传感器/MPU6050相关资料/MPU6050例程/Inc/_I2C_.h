#ifndef EXTEND_HEADFIEL__I2C__H_
#define EXTEND_HEADFIEL__I2C__H_
#include "stm32f10x.h"
#include "generic.h"//主要装Delay.h

/*
bilibili:小努班 UID:437280309
@time:2025.4.12
@verson:V1_4
@updata: Fixed an issue when multiple I2C cannot be used.Using object oriented programming thinking,Hide a lot lof API function,softerware I2C have arrived 390K
修复了多个I2C无法使用的问题。使用面向对象程序设计思维.隐藏大量API函数接口。软件I2C最快速度已经达到390Khz,与硬件IIC快速模式400k类似

注意:调用函数的时候，问为什么引脚没有波形，因为这是开漏模式，你需要上拉电阻，不然引脚电平无法发生变化，庆幸的是很多模块会自带上拉电阻,4.7K上拉速度比10K快可以适配高速IIC*/
/*硬件I2C初始化对应引脚for STM32F103C8
SCL1：PB6		SCL2：PB10
SDA1：PB7		SDA2：PB11
*/


/*//////////////////////////////////////////////////
使用示例:(对于操作8位寄存器)格式I2C(你的对象名字)->行为(函数)
I2C_BUS myI2C;
void main(){
	myI2C = Create_SI2C(GPIOA,GPIO_Pin_1,GPIO_Pin_2,0x40);//创建一个名字叫myI2C的对象,7位地址是0x40
	uint8_t add = 0;
	//I2C(INA266_I2C)->Mode16bit = 1;//升级为16位寄存器操作方法(如果需要对16位寄存器操作)
	while(1){
		add = I2C(myI2C)->ScanAdress(); //扫描IIC地址
		print("add:%d",add);			//打印扫描到的IIC地址(返回值是10进制)
	}
}
*///////////////////////////////////////////////////
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

	#define SI2C_delay_time 0																			//设置硬件I2C的延时速度
	#define I2C(obj) (Pthis_I2C = &obj)																	//Pthis全局指针宏定义
	typedef struct I2C_Private I2C_Private;		
	typedef struct I2C_BUS
	{
		//严禁使用该指针!(私有变量) It is strictly forbidden to use this pointer
		I2C_Private* Private;

		///////////////////////常用函数//////////////////////////////////                   			 				//用户API函数接口
		void (*Write_Reg)(uint8_t RegAddress, uint16_t Data);																						//写寄存器函数(默认8bit操作),write register by I2C bus
		uint16_t (*Read_Reg)(uint8_t RegAddress);																												//读寄存器函数(默认8bit操作),read register by I2C bus

		///////////////////////扩展函数//////////////////////////////////  
		uint8_t (*ScanAdress)();																																				//扫描IIC地址并返回,Scan IIC address and return it
		uint8_t (*AckTest)();																																						//响应接口(1 success,0 failed),it can check that if our I2C bus is init succese
		uint8_t Mode16bit;																																							//将IIC升级为16位操作(置1为升级),boost reg operation to 16bit
		void (*Rest_Speed)(uint32_t Speed);																															//硬件I2C重新设置速度,you can reset your Hardware I2C Speed	
		uint8_t (*Write_Reg_continue)(uint8_t Device_Add,uint8_t RegAddress,uint8_t Count,uint8_t* Data);//连续写寄存器函数,continue write register by I2C bus，为了适应移植
		uint8_t (*Read_Reg_continue)(uint8_t Device_Add,uint8_t RegAddress,uint8_t Count,uint8_t* Data); //连续读寄存器函数,continue read register by I2C bus，为了适应移植
	}I2C_BUS;
	extern I2C_BUS* Pthis_I2C;
		///////////////////////初始化函数//////////////////////////////// 
	I2C_BUS Create_SI2C(GPIO_TypeDef* GPIOx,uint16_t SCL,uint16_t SDA,uint8_t Address);					//创建软件I2C对象,create a softwere I2C
	I2C_BUS Create_HI2C(I2C_TypeDef* I2Cx,uint8_t Address);												//创建硬件I2C对象,create a hardware I2C

#ifdef __cplusplus
extern "C"
}
#endif /* __cplusplus */

#endif /* EXTEND_HEADFIEL__I2C__H_ */
