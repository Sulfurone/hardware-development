#include "stm32f10x.h"                  // 设备头文件
#include <stdio.h>                        // 标准输入输出
#include <stdarg.h>                       // 可变参数
#include "Serial.h"                      // 串口相关
#include "OLED.h"                        // OLED 显示
#include "string.h"                      // 字符串处理

extern volatile uint32_t msTicks;         // 系统节拍计数器（外部变量声明）

char Serial_RxPacket[100];                // 接收数据包数组，数据包格式"@MSG\r\n"
uint8_t Serial_RxFlag = 0;                // 接收数据包标志位
extern int startflag;                     // 启动标志
extern int caiflag;                       // 采样标志

static uint32_t lastRecvTime = 0;         // 记录上次接收处理的时间

// 串口初始化函数
void Serial_Init(void)
{
	// 开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);   // USART1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);    // GPIOA时钟

	// GPIO初始化
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;          // 复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;                // TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;            // 上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;               // RX
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// USART初始化
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;               // 波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;                // 收发模式
	USART_InitStructure.USART_Parity = USART_Parity_No;      // 无校验
	USART_InitStructure.USART_StopBits = USART_StopBits_1;   // 1位停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 8位数据位
	USART_Init(USART1, &USART_InitStructure);

	// 开启串口接收中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	// NVIC中断分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	// NVIC配置
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);

	// 使能USART1
	USART_Cmd(USART1, ENABLE);
}

// 发送一个字节
void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

// 发送一个数组
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	for (uint16_t i = 0; i < Length; i++)
	{
		Serial_SendByte(Array[i]);
	}
}

// 发送一个字符串
void Serial_SendString(char *String)
{
	for (uint8_t i = 0; String[i] != '\0'; i++)
	{
		Serial_SendByte(String[i]);
	}
}

// 幂运算（内部使用）
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

// 发送数字（定长）
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	for (uint8_t i = 0; i < Length; i++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

// printf重定向底层函数
int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);
	return ch;
}

// 自定义printf函数
void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}

// USART1中断服务函数
void USART1_IRQHandler(void)
{
	static uint8_t RxState = 0;     // 状态机状态
	static uint8_t pRxPacket = 0;   // 当前接收数据位置
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		uint8_t RxData = USART_ReceiveData(USART1);
		// 状态机处理数据包
		if (RxState == 0)
		{
			if (RxData == '@' && Serial_RxFlag == 0)
			{
				RxState = 1;
				pRxPacket = 0;
			}
		}
		else if (RxState == 1)
		{
			if (RxData == '\r')
			{
				RxState = 2;
			}
			else
			{
				Serial_RxPacket[pRxPacket] = RxData;
				pRxPacket++;
			}
		}
		else if (RxState == 2)
		{
			if (RxData == '\n')
			{
				RxState = 0;
				Serial_RxPacket[pRxPacket] = '\0';
				Serial_RxFlag = 1;
			}
		}
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

// 串口数据包接收处理函数
void jieshou()
{
	if (startflag == 1 && Serial_RxFlag == 1)
	{
		// 判断距离上次处理是否超过5秒
		if (msTicks - lastRecvTime >= 5000 || lastRecvTime == 0)
		{
			lastRecvTime = msTicks; // 更新时间戳
			OLED_ShowString(4, 1, Serial_RxPacket); // OLED显示接收到的数据
			if (strcmp(Serial_RxPacket, "Green") == 0)
			{
				OLED_ShowString(2, 1, "Green");
			}
			else if (strcmp(Serial_RxPacket, "Yellow") == 0)
			{
				caiflag = 1;
				OLED_ShowString(2, 1, "Yellow");
			}
			else
			{
				Serial_SendString("ERROR_COMMAND\r\n");
				OLED_ShowString(2, 1, "                ");
				OLED_ShowString(2, 1, "ERROR_COMMAND");
			}
			Serial_RxFlag = 0; // 清除标志位
		}
		else
		{
			// 5秒内收到新数据，直接丢弃或提示
			Serial_RxFlag = 0; // 清除标志位
			Serial_SendString("WAIT_5S\r\n");
		}
	}
}


