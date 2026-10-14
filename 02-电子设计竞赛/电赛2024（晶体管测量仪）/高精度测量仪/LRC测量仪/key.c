#include"key.h"
//延时函数
void Key_Deley(unsigned int n)  //(延时N)
{
  while(n--);  
}

//查询方式获取当前按键值,独热码
unsigned char Key_Get(void)
{
  unsigned int value = 0x0000;
  unsigned char i;
  
    //if(KEY_IRQ_PIN == 0)  //无按键按下（P5.0为中断标志脚，只要有键按下P5.0=0） 
	if(GPIOPinRead(KEY_DATA_PORT,KEY_IRQ_PIN))
    {                 //==0 ???
      return 0;
    }
    else              //有按键按下
    {
      Key_Deley(3000); //延时去抖
      if(GPIOPinRead(KEY_DATA_PORT,KEY_IRQ_PIN)) 
      {
        KEY_LD_H;  //(CS=1)   
        __nop();    //保持
        __nop();
        KEY_LD_L;       //片选使能
        //_NOP();         //硬件连接？？
        //_NOP();
        __nop();
		__nop();


        for(i=16;i>0;i--)//读取按键的值,为独热码
        {										  
          //if(KEYIN & KEY_DIN)  //P5.1为1则置位相应位，->移位寄存器
		  if(GPIOPinRead(KEY_DATA_PORT,KEY_DIN_PIN))
            value |= 0x01<<(i-0x01);

          KEY_CLK_L;
          __nop();
		  __nop();
          KEY_CLK_H;    //时钟移位
          __nop();
		  __nop();  
        }
        return value;//16位独热码
      }
      else
      {
         return 0;
      }
    }
}


void Key_Initial(void )
{
	SysCtlPeriEnable(KEY_DATA_PERIPH); 
    GPIOPinTypeOut(KEY_DATA_PORT,KEY_LD_PIN|KEY_CLK_PIN);
    GPIOPinTypeIn(KEY_DATA_PORT,KEY_DIN_PIN|KEY_IRQ_PIN);
}


//将按键值翻译
unsigned char Key_Id(unsigned int n)
{  
  unsigned char i;
  if(n==0) 
    return 0;
  else 
  {
   for(i=0;i<16;i++)
     if(n&(1<<i))
     break;
  return (i+1);
  }
}
