/****************************************************/
// MSP432P401R
// 配置Keil独立空工程 (已将ti固件库打包至工程)
// Keil工程已配置开启FPU
// Bilibili：m-RNA
// E-mail:m-RNA@qq.com
// 创建日期:2021/9/28
/****************************************************/

#include "sysinit.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "tim32.h"

int main(void)
{
    SysInit();         // 第3讲 时钟配置
    uart_init(115200); // 第7讲 串口配置
    delay_init();      // 第4讲 滴答延时

    /*开始填充初始化代码*/
//    void setMotor(int speedpin,int dirpin, int speed)
//      {
//   if (speed == 0)
//   {
//   digitalWrite(speedpin, LOW);
//   }
//   else if (speed > 0)
//   {
//   digitalWrite(dirpin, LOW);
//   analogWrite(speedpin, speed);
//   }
//   else
//   {
//   digitalWrite(dirpin, HIGH);
//   analogWrite(speedpin, speed);
//   }
//   }

//   void setup(){
//     pinMode(2, INPUT);//右边寻线传感器引脚
//     pinMode(3, INPUT);//左边寻线传感器引脚
//     pinMode(5, OUTPUT);
//     pinMode(4, OUTPUT);
//     digitalWrite(5, LOW);
//     digitalWrite(4, LOW);
//     pinMode(6, OUTPUT);
//     pinMode(7, OUTPUT);
//     digitalWrite(6, LOW);
//     digitalWrite(7, LOW);
//     // 右边传感器
//     pinMode(2, INPUT);
//     // 左边传感器
//     pinMode(3, INPUT);
//   }

//   void loop(){
//     if (digitalRead(2) == true && digitalRead(3) == false) {
//       setMotor(5, 4,50);
//       setMotor(6, 7,0);

//     } else if (digitalRead(2) == false && digitalRead(3) == true) {
//       setMotor(5, 4,0);
//       setMotor(6, 7,50);
//     } else if (digitalRead(2) == false && digitalRead(3) == false) {
//       setMotor(5, 4,50);
//       setMotor(6, 7,50);
//     } else {
//       setMotor(5, 4,0);
//       setMotor(6, 7,0);

//     }

//   }
    /*停止填充初始化代码*/

    printf("Hello,MSP432!\r\n");
    MAP_Interrupt_enableMaster(); // 开启总中断
    while (1)
    {
        /*开始填充用户代码*/

        /*停止填充用户代码*/
    }
}
