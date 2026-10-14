#include "kai.h"

#include "main.h"


void kaiInit(void)
{
GPIO_setAsInputPin(KAI1_PORT, KAI1_PIN); //选择端口及引脚作为输入端口
GPIO_setAsInputPinWithPullUpResistor(KAI1_PORT, KAI1_PIN);//上拉
GPIO_disableInterrupt(KAI1_PORT, KAI1_PIN);//禁止中断
GPIO_setAsInputPin(KAI2_PORT, KAI2_PIN); //选择端口及引脚作为输入端口
GPIO_setAsInputPinWithPullUpResistor(KAI2_PORT, KAI2_PIN);//上拉
GPIO_disableInterrupt(KAI2_PORT, KAI2_PIN);//禁止中断

}




