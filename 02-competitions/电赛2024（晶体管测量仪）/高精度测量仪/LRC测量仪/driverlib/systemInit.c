#include  "systemInit.h"


//  定义全局的系统时钟变量
unsigned long TheSysClock = 12000000UL;


//  定义KEY
#define  KEY_PERIPH             SYSCTL_PERIPH_GPIOB
#define  KEY_PORT               GPIO_PORTB_BASE
#define  KEY_PIN                GPIO_PIN_5


//  防止JTAG失效
void jtagWait(void)
{
    SysCtlPeripheralEnable(KEY_PERIPH);                           //  使能KEY所在的GPIO端口
    GPIOPinTypeGPIOInput(KEY_PORT, KEY_PIN);                       //  设置KEY所在管脚为输入

    if (GPIOPinRead(KEY_PORT, KEY_PIN) == 0x00)             //  若复位时按下KEY，则进入
    {
        for (;;);                                           //  死循环，以等待JTAG连接
    }

    SysCtlPeripheralDisable(KEY_PERIPH);                          //  禁止KEY所在的GPIO端口
}


//  系统时钟初始化
void clockInit(void)
{
    SysCtlLDOSet(SYSCTL_LDO_2_50V);                         //  设置LDO输出电压

    SysCtlClockSet(SYSCTL_USE_OSC |                         //  系统时钟设置
                   SYSCTL_OSC_MAIN |                        //  采用主振荡器
                   SYSCTL_XTAL_6MHZ |                       //  外接6MHz晶振
                   SYSCTL_SYSDIV_1);                        //  不分频

/*
    SysCtlLDOSet(SYSCTL_LDO_2_75V);                         //  配置PLL前须将LDO设为2.75V

    SysCtlClockSet(SYSCTL_USE_PLL |                         //  系统时钟设置，采用PLL
                   SYSCTL_OSC_MAIN |                        //  主振荡器
                   SYSCTL_XTAL_6MHZ |                       //  外接6MHz晶振
                   SYSCTL_SYSDIV_10);                       //  分频结果为20MHz
*/

    TheSysClock = SysCtlClockGet();                         //  获取当前的系统时钟频率
}

