#include "oled.h"
#include "oledfont.h"  


// OLED显存，8页，每页128列
uint16_t OLED_GRAM[128][8];

// 刷新显存到OLED屏幕
void OLED_Refresh_Gram(void)
{
    uint8_t i,n;
    for(i=0;i<8;i++)  // 8页
    {
        OLED_WR_Byte (0xb0+i,OLED_CMD);    // 设置页地址
        OLED_WR_Byte (0x00,OLED_CMD);      // 设置列低地址
        OLED_WR_Byte (0x10,OLED_CMD);      // 设置列高地址
        for(n=0;n<128;n++) OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); // 写入一页数据
    }
}
#if OLED_MODE==1
// 并口写入一个字节
void OLED_WR_Byte(uint8_t dat,uint8_t cmd)
{
    if(cmd)
      OLED_RS_Set();
    else 
      OLED_RS_Clr();
    OLED_WR_Clr();
    OLED_WR_Set();
    OLED_CS_Set();
    OLED_RS_Set();
}
#else
// 模拟SPI写入一个字节
void OLED_WR_Byte(uint8_t dat,uint8_t cmd)
{
    uint8_t i;
    if(cmd)
      OLED_RS_Set();
    else 
      OLED_RS_Clr();
    for(i=0;i<8;i++)
    {
        OLED_SCLK_Clr();
        if(dat&0x80)
           OLED_SDIN_Set();
        else 
           OLED_SDIN_Clr();
        OLED_SCLK_Set();
        dat<<=1;
    }
    OLED_RS_Set();
}
#endif

// 开启OLED显示
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  // SET DCDC命令
    OLED_WR_Byte(0X14,OLED_CMD);  // DCDC ON
    OLED_WR_Byte(0XAF,OLED_CMD);  // DISPLAY ON
}
// 关闭OLED显示
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  // SET DCDC命令
    OLED_WR_Byte(0X10,OLED_CMD);  // DCDC OFF
    OLED_WR_Byte(0XAE,OLED_CMD);  // DISPLAY OFF
}
// 清屏，显存全部清零
void OLED_Clear(void)
{
    uint8_t i,n;
    for(i=0;i<8;i++)for(n=0;n<130;n++)OLED_GRAM[n][i]=0X00;
    OLED_Refresh_Gram(); // 刷新显示
}
// 画点
// x:0~127, y:0~63, t:1填充,0清空
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t)
{
    uint8_t pos,bx,temp=0;
    if(x>131||y>63)return; // 越界保护
    pos=7-y/8;
    bx=y%8;
    temp=1<<(7-bx);
    if(t)OLED_GRAM[x][pos]|=temp;
    else OLED_GRAM[x][pos]&=~temp;
}
// 区域填充
// x1,y1,x2,y2:对角坐标，dot:0清空,1填充
void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot)
{
    uint8_t x,y;
    for(x=x1;x<=x2;x++)
    {
        for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
    }
    OLED_Refresh_Gram();
}
// 显示单个字符
// x:0~127, y:0~63, mode:0反白,1正常, size:字体大小
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode)
{
    uint8_t temp,t,t1;
    uint8_t y0=y;
    chr=chr-' ';// 字符偏移
    for(t=0;t<size;t++)
    {
        if(size==12)temp=oled_asc2_1206[chr][t];
        else temp=oled_asc2_1608[chr][t];
        for(t1=0;t1<8;t1++)
        {
            if(temp&0x80)OLED_DrawPoint(x,y,mode);
            else OLED_DrawPoint(x,y,!mode);
            temp<<=1;
            y++;
            if((y-y0)==size)
            {
                y=y0;
                x++;
                break;
            }
        }
    }
}
// 幂运算
uint32_t oled_pow(uint8_t m,uint8_t n)
{
    uint32_t result=1;
    while(n--)result*=m;
    return result;
}
// 显示数字
// x,y:起点坐标, len:位数, size:字体, num:数值
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size)
{
    uint8_t t,temp;
    uint8_t enshow=0;
    for(t=0;t<len;t++)
    {
        temp=(num/oled_pow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
                continue;
            }else enshow=1;
        }
        OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1);
    }
}
// 显示字符串，16号字体
void OLED_ShowString(uint8_t x,uint8_t y,const uint8_t *p)
{
#define MAX_CHAR_POSX 126
#define MAX_CHAR_POSY 58
    while(*p!='\0')
    {
        if(x>MAX_CHAR_POSX){x=0;y+=16;}
        if(y>MAX_CHAR_POSY){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,16,1);
        x+=8;
        p++;
    }
}
// OLED初始化（硬件+寄存器）
void OLED_Init1(void)
{
    OLED_RST_Clr();
    HAL_Delay(100);
    OLED_RST_Set();
    OLED_WR_Byte(0xAE,OLED_CMD); // 关闭显示
    OLED_WR_Byte(0xD5,OLED_CMD); // 设置时钟分频
    OLED_WR_Byte(80,OLED_CMD);
    OLED_WR_Byte(0xA8,OLED_CMD); // 设置驱动路数
    OLED_WR_Byte(0X3F,OLED_CMD);
    OLED_WR_Byte(0xD3,OLED_CMD); // 设置显示偏移
    OLED_WR_Byte(0X00,OLED_CMD);
    OLED_WR_Byte(0x40,OLED_CMD); // 设置显示开始行
    OLED_WR_Byte(0x8D,OLED_CMD); // 电荷泵设置
    OLED_WR_Byte(0x14,OLED_CMD);
    OLED_WR_Byte(0x20,OLED_CMD); // 设置内存地址模式
    OLED_WR_Byte(0x02,OLED_CMD);
    OLED_WR_Byte(0xA1,OLED_CMD); // 段重定义
    OLED_WR_Byte(0xC0,OLED_CMD); // COM扫描方向
    OLED_WR_Byte(0xDA,OLED_CMD); // COM硬件引脚配置
    OLED_WR_Byte(0x12,OLED_CMD);
    OLED_WR_Byte(0x81,OLED_CMD); // 对比度
    OLED_WR_Byte(0xEF,OLED_CMD);
    OLED_WR_Byte(0xD9,OLED_CMD); // 预充电周期
    OLED_WR_Byte(0xf1,OLED_CMD);
    OLED_WR_Byte(0xDB,OLED_CMD); // VCOMH电压倍率
    OLED_WR_Byte(0x30,OLED_CMD);
    OLED_WR_Byte(0xA4,OLED_CMD); // 全局显示
    OLED_WR_Byte(0xA6,OLED_CMD); // 显示方式
    OLED_WR_Byte(0xAF,OLED_CMD); // 开启显示
    OLED_Clear();
}
// OLED相关GPIO初始化
void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    // 使能对应端口时钟
#if OLED_GPIO_CLK == A
    __HAL_RCC_GPIOA_CLK_ENABLE();
#endif
#if OLED_GPIO_CLK == B
    __HAL_RCC_GPIOB_CLK_ENABLE();
#endif
#if OLED_GPIO_CLK == C
    __HAL_RCC_GPIOC_CLK_ENABLE();
#endif
    GPIO_InitStruct.Pin = OLED_SCLK|OLED_SDIN|OLED_RST|OLED_RS;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DATE_GPIO, &GPIO_InitStruct);
    HAL_GPIO_WritePin(DATE_GPIO,OLED_SCLK|OLED_SDIN|OLED_RST|OLED_RS,GPIO_PIN_SET); // 默认高电平
}
// OLED初始化（推荐用此函数）
void OLED_Init(void)
{
    GPIO_Config();
    OLED_RST_Clr();
    HAL_Delay(100);
    OLED_RST_Set();
    OLED_WR_Byte(0xAE,OLED_CMD); // 关闭显示
    OLED_WR_Byte(0xD5,OLED_CMD); // 设置时钟分频
    OLED_WR_Byte(80,OLED_CMD);
    OLED_WR_Byte(0xA8,OLED_CMD); // 设置驱动路数
    OLED_WR_Byte(0X3F,OLED_CMD);
    OLED_WR_Byte(0xD3,OLED_CMD); // 设置显示偏移
    OLED_WR_Byte(0X00,OLED_CMD);
    OLED_WR_Byte(0x40,OLED_CMD); // 设置显示开始行
    OLED_WR_Byte(0x8D,OLED_CMD); // 电荷泵设置
    OLED_WR_Byte(0x14,OLED_CMD);
    OLED_WR_Byte(0x20,OLED_CMD); // 设置内存地址模式
    OLED_WR_Byte(0x02,OLED_CMD);
    OLED_WR_Byte(0xA1,OLED_CMD); // 段重定义
    OLED_WR_Byte(0xC0,OLED_CMD); // COM扫描方向
    OLED_WR_Byte(0xDA,OLED_CMD); // COM硬件引脚配置
    OLED_WR_Byte(0x12,OLED_CMD);
    OLED_WR_Byte(0x81,OLED_CMD); // 对比度
    OLED_WR_Byte(0xEF,OLED_CMD);
    OLED_WR_Byte(0xD9,OLED_CMD); // 预充电周期
    OLED_WR_Byte(0xf1,OLED_CMD);
    OLED_WR_Byte(0xDB,OLED_CMD); // VCOMH电压倍率
    OLED_WR_Byte(0x30,OLED_CMD);
    OLED_WR_Byte(0xA4,OLED_CMD); // 全局显示
    OLED_WR_Byte(0xA6,OLED_CMD); // 显示方式
    OLED_WR_Byte(0xAF,OLED_CMD); // 开启显示
    OLED_Clear();
}
