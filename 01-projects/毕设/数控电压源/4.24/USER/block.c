//#include "stm32f10x.h"                  // Device header
//#include "lk.h"
//#include "sys.h"
//#include "delay.h"
//#include "block.h"


//int blockflag=0;

//void Block_Init(void)
//{
//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

//GPIO_InitTypeDef GPIO_InitStructure;
//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  // 改为下拉输入模式
//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 输入模式下速度不影响，但需填写
//GPIO_Init(GPIOB, &GPIO_InitStructure);


//}



//void block(void)//障碍检测
//{
//if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 1)
//	{
//		delay_ms(20);
//		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 1);
//		delay_ms(20);
//		blockflag=1;
//	}
////else 
////{
////blockflag=0;
////}
//}
