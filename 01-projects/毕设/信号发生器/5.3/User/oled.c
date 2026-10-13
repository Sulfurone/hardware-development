#include "oled.h"
#include "oledfont.h"  
#include "common.h"


// OLED�Դ棬8ҳ��ÿҳ128��
uint16_t OLED_GRAM[128][8];

// ˢ���Դ浽OLED��Ļ
void OLED_Refresh_Gram(void)
{
    uint8_t i,n;
    for(i=0;i<8;i++)  // 8ҳ
    {
        OLED_WR_Byte (0xb0+i,OLED_CMD);    // ����ҳ��ַ
        OLED_WR_Byte (0x00,OLED_CMD);      // �����е͵�ַ
        OLED_WR_Byte (0x10,OLED_CMD);      // �����иߵ�ַ
        for(n=0;n<128;n++) OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); // дһҳ
    }
}
#if OLED_MODE==1
// ����дһֽ
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
// ģSPIдһֽ
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

// ����OLEDʾ
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  // SET DCDC
    OLED_WR_Byte(0X14,OLED_CMD);  // DCDC ON
    OLED_WR_Byte(0XAF,OLED_CMD);  // DISPLAY ON
}
//رOLEDʾ
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  // SET DCDC
    OLED_WR_Byte(0X10,OLED_CMD);  // DCDC OFF
    OLED_WR_Byte(0XAE,OLED_CMD);  // DISPLAY OFF
}
// Դȫ
void OLED_Clear(void)
{
    uint8_t i,n;
    for(i=0;i<8;i++)for(n=0;n<130;n++)OLED_GRAM[n][i]=0X00;
    OLED_Refresh_Gram(); // ˢʾ
}
// 
// x:0~127, y:0~63, t:1,0
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t)
{
    uint8_t pos,bx,temp=0;
    if(x>131||y>63)return; // Խ籣
    pos=7-y/8;
    bx=y%8;
    temp=1<<(7-bx);
    if(t)OLED_GRAM[x][pos]|=temp;
    else OLED_GRAM[x][pos]&=~temp;
}
// 
// x1,y1,x2,y2:Խ꣬dot:0,1
void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot)
{
    uint8_t x,y;
    for(x=x1;x<=x2;x++)
    {
        for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
    }
    OLED_Refresh_Gram();
}
// ʾַ
// x:0~127, y:0~63, mode:0,1, size:С
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode)
{
    uint8_t temp,t,t1;
    uint8_t y0=y;
    chr=chr-' ';// ַƫ
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
// 
uint32_t oled_pow(uint8_t m,uint8_t n)
{
    uint32_t result=1;
    while(n--)result*=m;
    return result;
}
// ʾ
// x,y:,, len:λ, size:, num:ֵ
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
// ʾַ16
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
// OLEDʼӲ+Ĵ
void OLED_Init1(void)
{
    OLED_RST_Clr();
    Delay_ms(100);
    OLED_RST_Set();
    OLED_WR_Byte(0xAE,OLED_CMD); //رʾ
    OLED_WR_Byte(0xD5,OLED_CMD); // ʱӷƵ
    OLED_WR_Byte(80,OLED_CMD);
    OLED_WR_Byte(0xA8,OLED_CMD); // ·
    OLED_WR_Byte(0X3F,OLED_CMD);
    OLED_WR_Byte(0xD3,OLED_CMD); // ʾƫ
    OLED_WR_Byte(0X00,OLED_CMD);
    OLED_WR_Byte(0x40,OLED_CMD); // ʾʼ
    OLED_WR_Byte(0x8D,OLED_CMD); // ɱ
    OLED_WR_Byte(0x14,OLED_CMD);
    OLED_WR_Byte(0x20,OLED_CMD); // ڴַģʽ
    OLED_WR_Byte(0x02,OLED_CMD);
    OLED_WR_Byte(0xA1,OLED_CMD); // ض
    OLED_WR_Byte(0xC0,OLED_CMD); // COMɨ跽
    OLED_WR_Byte(0xDA,OLED_CMD); // COMӲ
    OLED_WR_Byte(0x12,OLED_CMD);
    OLED_WR_Byte(0x81,OLED_CMD); // Աȶ
    OLED_WR_Byte(0xEF,OLED_CMD);
    OLED_WR_Byte(0xD9,OLED_CMD); // Ԥ
    OLED_WR_Byte(0xf1,OLED_CMD);
    OLED_WR_Byte(0xDB,OLED_CMD); // VCOMHѹ
    OLED_WR_Byte(0x30,OLED_CMD);
    OLED_WR_Byte(0xA4,OLED_CMD); // ȫʾ
    OLED_WR_Byte(0xA6,OLED_CMD); // ʾʽ
    OLED_WR_Byte(0xAF,OLED_CMD); // ʾ
    OLED_Clear();
}
// OLEDGPIOʼ
void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // ʹܶӦ˿ʱ
    RCC_APB2PeriphClockCmd(OLED_GPIO_CLK, ENABLE);
    
    // 配置GPIO
    GPIO_InitStructure.GPIO_Pin = OLED_SCLK|OLED_SDIN|OLED_RST|OLED_RS;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DATE_GPIO, &GPIO_InitStructure);
    
    // 默认高电平
    GPIO_SetBits(DATE_GPIO, OLED_SCLK|OLED_SDIN|OLED_RST|OLED_RS);
}
// OLEDʼƼô˺
void OLED_Init(void)
{
    GPIO_Config();
    
    // 复位OLED
    OLED_RST_Clr();
    Delay_ms(100);
    OLED_RST_Set();
    Delay_ms(100);
    
    // 初始化命令
    OLED_WR_Byte(0xAE,OLED_CMD); // 关闭显示
    OLED_WR_Byte(0xD5,OLED_CMD); // 设置时钟分频
    OLED_WR_Byte(80,OLED_CMD);
    OLED_WR_Byte(0xA8,OLED_CMD); // 设置驱动路数
    OLED_WR_Byte(0X3F,OLED_CMD);
    OLED_WR_Byte(0xD3,OLED_CMD); // 设置显示偏移
    OLED_WR_Byte(0X00,OLED_CMD);
    OLED_WR_Byte(0x40,OLED_CMD); // 设置显示起始行
    OLED_WR_Byte(0x8D,OLED_CMD); // 电荷泵
    OLED_WR_Byte(0x14,OLED_CMD);
    OLED_WR_Byte(0x20,OLED_CMD); // 设置内存地址模式
    OLED_WR_Byte(0x02,OLED_CMD);
    OLED_WR_Byte(0xA1,OLED_CMD); // 段重定向
    OLED_WR_Byte(0xC0,OLED_CMD); // COM扫描方向
    OLED_WR_Byte(0xDA,OLED_CMD); // COM硬件引脚配置
    OLED_WR_Byte(0x12,OLED_CMD);
    OLED_WR_Byte(0x81,OLED_CMD); // 对比度
    OLED_WR_Byte(0xEF,OLED_CMD);
    OLED_WR_Byte(0xD9,OLED_CMD); // 预充电
    OLED_WR_Byte(0xf1,OLED_CMD);
    OLED_WR_Byte(0xDB,OLED_CMD); // VCOMH电压调节
    OLED_WR_Byte(0x30,OLED_CMD);
    OLED_WR_Byte(0xA4,OLED_CMD); // 全局显示
    OLED_WR_Byte(0xA6,OLED_CMD); // 显示方式
    OLED_WR_Byte(0xAF,OLED_CMD); // 开启显示
    
    // 清屏
    OLED_Clear();
}

// 绘制直线函数
void OLED_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t mode)
{
    uint8_t dx, dy;
    int8_t x, y, x_add, y_add;
    int16_t e;  // 改为有符号整数
    
    dx = x2 > x1 ? (x2 - x1) : (x1 - x2);
    dy = y2 > y1 ? (y2 - y1) : (y1 - y2);
    
    x = x1;
    y = y1;
    
    if(x2 > x1) x_add = 1;
    else x_add = -1;
    
    if(y2 > y1) y_add = 1;
    else y_add = -1;
    
    if(dx >= dy)
    {
        e = dy - dx;
        while(x != x2)
        {
            OLED_DrawPoint((uint8_t)x, (uint8_t)y, mode);
            if(e >= 0)
            {
                y += y_add;
                e -= dx;
            }
            x += x_add;
            e += dy;
        }
    }
    else
    {
        e = dx - dy;
        while(y != y2)
        {
            OLED_DrawPoint((uint8_t)x, (uint8_t)y, mode);
            if(e >= 0)
            {
                x += x_add;
                e -= dy;
            }
            y += y_add;
            e += dx;
        }
    }
    OLED_DrawPoint((uint8_t)x, (uint8_t)y, mode);
}
