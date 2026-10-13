#include "GPS.h"
#include "string.h"
#include "OLED.h"
#include "Delay.h"
#include <stdio.h>

// 全局变量
uint8_t GPS_RX_BUF[USART_MAX_RECV_LEN];
uint16_t GPS_RX_LEN = 0;
uint16_t point1 = 0;

// ATGM336H 数据结构
ATGM336H_Data Save_Data;

/**
  * @brief  计算字符串中第n个逗号的位置
  * @param  buf 字符串
  * @param  cx 逗号序号
  * @retval 位置
  */
uint8_t NMEA_Comma_Pos(uint8_t *buf, uint8_t cx)
{
    uint8_t *p = buf;
    while(cx)
    {
        if(*buf == '*' || *buf < ' ' || *buf > 'z') return 0XFF;
        if(*buf == ',') cx--;
        buf++;
    }
    return buf - p;
}

/**
  * @brief  计算幂
  * @param  m 底数
  * @param  n 指数
  * @retval 结果
  */
uint32_t NMEA_Pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while(n--) result *= m;
    return result;
}

/**
  * @brief  字符串转数字
  * @param  buf 字符串
  * @param  dx 小数位数
  * @retval 结果
  */
int NMEA_Str2num(uint8_t *buf, uint8_t* dx)
{
    uint8_t *p = buf;
    uint32_t ires = 0, fres = 0;
    uint8_t ilen = 0, flen = 0, i;
    uint8_t mask = 0;
    int res;
    while(1)
    {
        if(*p == '-') { mask |= 0X02; p++; }
        if(*p == ',' || (*p == '*')) break;
        if(*p == '.') { mask |= 0X01; p++; }
        else if(*p > '9' || (*p < '0'))
        {
            ilen = 0;
            flen = 0;
            break;
        }
        if(mask & 0X01) flen++;
        else ilen++;
        p++;
    }
    if(mask & 0X02) buf++;
    for(i = 0; i < ilen; i++)
    {
        ires += NMEA_Pow(10, ilen - 1 - i) * (buf[i] - '0');
    }
    if(flen > 5) flen = 5;
    *dx = flen;
    for(i = 0; i < flen; i++)
    {
        fres += NMEA_Pow(10, flen - 1 - i) * (buf[ilen + 1 + i] - '0');
    }
    res = ires * NMEA_Pow(10, flen) + fres;
    if(mask & 0X02) res = -res;
    return res;
}

/**
  * @brief  解析GPGSV语句
  * @param  gpsx GPS数据结构体
  * @param  buf 数据缓冲区
  * @retval 无
  */
void NMEA_GPGSV_Analysis(nmea_msg *gpsx, uint8_t *buf)
{
    uint8_t *p, *p1, dx;
    uint8_t len, i, j, slx = 0;
    uint8_t posx;
    p = buf;
    p1 = (uint8_t*)strstr((const char *)p, "$GPGSV");
    if(p1 == 0) p1 = (uint8_t*)strstr((const char *)p, "$GNGSV");
    if(p1 == 0) return;
    len = p1[7] - '0';
    posx = NMEA_Comma_Pos(p1, 3);
    if(posx != 0XFF) gpsx->svnum = NMEA_Str2num(p1 + posx, &dx);
    for(i = 0; i < len; i++)
    {
        p1 = (uint8_t*)strstr((const char *)p, "$GPGSV");
        if(p1 == 0) p1 = (uint8_t*)strstr((const char *)p, "$GNGSV");
        if(p1 == 0) break;
        for(j = 0; j < 4; j++)
        {
            posx = NMEA_Comma_Pos(p1, 4 + j * 4);
            if(posx != 0XFF) gpsx->slmsg[slx].num = NMEA_Str2num(p1 + posx, &dx);
            else break;
            posx = NMEA_Comma_Pos(p1, 5 + j * 4);
            if(posx != 0XFF) gpsx->slmsg[slx].eledeg = NMEA_Str2num(p1 + posx, &dx);
            else break;
            posx = NMEA_Comma_Pos(p1, 6 + j * 4);
            if(posx != 0XFF) gpsx->slmsg[slx].azideg = NMEA_Str2num(p1 + posx, &dx);
            else break;
            posx = NMEA_Comma_Pos(p1, 7 + j * 4);
            if(posx != 0XFF) gpsx->slmsg[slx].sn = NMEA_Str2num(p1 + posx, &dx);
            else break;
            slx++;
        }
        p = p1 + 1;
    }
}

/**
  * @brief  解析GPGGA语句
  * @param  gpsx GPS数据结构体
  * @param  buf 数据缓冲区
  * @retval 无
  */
void NMEA_GPGGA_Analysis(nmea_msg *gpsx, uint8_t *buf)
{
    uint8_t *p1, dx;
    uint8_t posx;
    p1 = (uint8_t*)strstr((const char *)buf, "$GPGGA");
    if(p1 == 0) p1 = (uint8_t*)strstr((const char *)buf, "$GNGGA");
    if(p1 == 0) return;
    posx = NMEA_Comma_Pos(p1, 6);
    if(posx != 0XFF) gpsx->gpssta = NMEA_Str2num(p1 + posx, &dx);
    posx = NMEA_Comma_Pos(p1, 7);
    if(posx != 0XFF) gpsx->posslnum = NMEA_Str2num(p1 + posx, &dx);
    posx = NMEA_Comma_Pos(p1, 9);
    if(posx != 0XFF) gpsx->altitude = NMEA_Str2num(p1 + posx, &dx);
}

/**
  * @brief  解析GPGSA语句
  * @param  gpsx GPS数据结构体
  * @param  buf 数据缓冲区
  * @retval 无
  */
void NMEA_GPGSA_Analysis(nmea_msg *gpsx, uint8_t *buf)
{
    uint8_t *p1, dx;
    uint8_t posx;
    uint8_t i;
    p1 = (uint8_t*)strstr((const char *)buf, "$GPGSA");
    if(p1 == 0) p1 = (uint8_t*)strstr((const char *)buf, "$GNGSA");
    if(p1 == 0) return;
    posx = NMEA_Comma_Pos(p1, 2);
    if(posx != 0XFF) gpsx->fixmode = NMEA_Str2num(p1 + posx, &dx);
    for(i = 0; i < 12; i++)
    {
        posx = NMEA_Comma_Pos(p1, 3 + i);
        if(posx != 0XFF) gpsx->possl[i] = NMEA_Str2num(p1 + posx, &dx);
        else break;
    }
    posx = NMEA_Comma_Pos(p1, 15);
    if(posx != 0XFF) gpsx->pdop = NMEA_Str2num(p1 + posx, &dx);
    posx = NMEA_Comma_Pos(p1, 16);
    if(posx != 0XFF) gpsx->hdop = NMEA_Str2num(p1 + posx, &dx);
    posx = NMEA_Comma_Pos(p1, 17);
    if(posx != 0XFF) gpsx->vdop = NMEA_Str2num(p1 + posx, &dx);
}

/**
  * @brief  解析GPRMC语句
  * @param  gpsx GPS数据结构体
  * @param  buf 数据缓冲区
  * @retval 无
  */
void NMEA_GPRMC_Analysis(nmea_msg *gpsx, uint8_t *buf)
{
    uint8_t *p1, dx;
    uint8_t posx;
    uint32_t temp;
    float rs;
    p1 = (uint8_t*)strstr((const char *)buf, "$GPRMC");
    if(p1 == 0) p1 = (uint8_t*)strstr((const char *)buf, "$GNRMC");
    if(p1 == 0) return;
    posx = NMEA_Comma_Pos(p1, 1);
    if(posx != 0XFF)
    {
        temp = NMEA_Str2num(p1 + posx, &dx) / NMEA_Pow(10, dx);
        gpsx->utc.hour = temp / 10000;
        gpsx->utc.min = (temp / 100) % 100;
        gpsx->utc.sec = temp % 100;
    }
    posx = NMEA_Comma_Pos(p1, 3);
    if(posx != 0XFF)
    {
        temp = NMEA_Str2num(p1 + posx, &dx);
        gpsx->latitude = temp / NMEA_Pow(10, dx + 2);
        rs = temp % NMEA_Pow(10, dx + 2);
        gpsx->latitude = gpsx->latitude * NMEA_Pow(10, 5) + (rs * NMEA_Pow(10, 5 - dx)) / 60;
    }
    posx = NMEA_Comma_Pos(p1, 4);
    if(posx != 0XFF) gpsx->nshemi = *(p1 + posx);
    posx = NMEA_Comma_Pos(p1, 5);
    if(posx != 0XFF)
    {
        temp = NMEA_Str2num(p1 + posx, &dx);
        gpsx->longitude = temp / NMEA_Pow(10, dx + 2);
        rs = temp % NMEA_Pow(10, dx + 2);
        gpsx->longitude = gpsx->longitude * NMEA_Pow(10, 5) + (rs * NMEA_Pow(10, 5 - dx)) / 60;
    }
    posx = NMEA_Comma_Pos(p1, 6);
    if(posx != 0XFF) gpsx->ewhemi = *(p1 + posx);
    posx = NMEA_Comma_Pos(p1, 9);
    if(posx != 0XFF)
    {
        temp = NMEA_Str2num(p1 + posx, &dx);
        gpsx->utc.date = temp / 10000;
        gpsx->utc.month = (temp / 100) % 100;
        gpsx->utc.year = 2000 + temp % 100;
    }
}

/**
  * @brief  解析GPVTG语句
  * @param  gpsx GPS数据结构体
  * @param  buf 数据缓冲区
  * @retval 无
  */
void NMEA_GPVTG_Analysis(nmea_msg *gpsx, uint8_t *buf)
{
    uint8_t *p1, dx;
    uint8_t posx;
    p1 = (uint8_t*)strstr((const char *)buf, "$GPVTG");
    if(p1 == 0) p1 = (uint8_t*)strstr((const char *)buf, "$GNVTG");
    if(p1 == 0) return;
    posx = NMEA_Comma_Pos(p1, 7);
    if(posx != 0XFF)
    {
        gpsx->speed = NMEA_Str2num(p1 + posx, &dx);
        if(dx < 3) gpsx->speed *= NMEA_Pow(10, 3 - dx);
    }
}

/**
  * @brief  解析GPGLL语句
  * @param  gpsx GPS数据结构体
  * @param  buf 数据缓冲区
  * @retval 无
  */
void NMEA_GPGLL_Analysis(nmea_msg *gpsx, uint8_t *buf)
{
    uint8_t *p1, dx;
    uint8_t posx;
    uint32_t temp;
    float rs;
    p1 = (uint8_t*)strstr((const char *)buf, "$GPGLL");
    if(p1 == 0) p1 = (uint8_t*)strstr((const char *)buf, "$GNGLL");
    if(p1 == 0) return;
    posx = NMEA_Comma_Pos(p1, 1);
    if(posx != 0XFF)
    {
        temp = NMEA_Str2num(p1 + posx, &dx);
        gpsx->latitude = temp / NMEA_Pow(10, dx + 2);
        rs = temp % NMEA_Pow(10, dx + 2);
        gpsx->latitude = gpsx->latitude * NMEA_Pow(10, 5) + (rs * NMEA_Pow(10, 5 - dx)) / 60;
    }
    posx = NMEA_Comma_Pos(p1, 2);
    if(posx != 0XFF) gpsx->nshemi = *(p1 + posx);
    posx = NMEA_Comma_Pos(p1, 3);
    if(posx != 0XFF)
    {
        temp = NMEA_Str2num(p1 + posx, &dx);
        gpsx->longitude = temp / NMEA_Pow(10, dx + 2);
        rs = temp % NMEA_Pow(10, dx + 2);
        gpsx->longitude = gpsx->longitude * NMEA_Pow(10, 5) + (rs * NMEA_Pow(10, 5 - dx)) / 60;
    }
    posx = NMEA_Comma_Pos(p1, 4);
    if(posx != 0XFF) gpsx->ewhemi = *(p1 + posx);
    posx = NMEA_Comma_Pos(p1, 5);
    if(posx != 0XFF)
    {
        temp = NMEA_Str2num(p1 + posx, &dx);
        gpsx->utc.hour = temp / 10000;
        gpsx->utc.min = (temp / 100) % 100;
        gpsx->utc.sec = temp % 100;
    }
    posx = NMEA_Comma_Pos(p1, 6);
    if(posx != 0XFF)
    {
        if(*(p1 + posx) == 'A') gpsx->fixmode = 3;
        else gpsx->fixmode = 1;
    }
}

/**
  * @brief  GPS数据解析
  * @param  gpsx GPS数据结构体
  * @param  buf 数据缓冲区
  * @retval 无
  */
void GPS_Analysis(nmea_msg *gpsx, uint8_t *buf)
{
    NMEA_GPGSV_Analysis(gpsx, buf);
    NMEA_GPGGA_Analysis(gpsx, buf);
    NMEA_GPGSA_Analysis(gpsx, buf);
    NMEA_GPRMC_Analysis(gpsx, buf);
    NMEA_GPVTG_Analysis(gpsx, buf);
    NMEA_GPGLL_Analysis(gpsx, buf);
}

/**
  * @brief  GPS模块初始化
  * @param  无
  * @retval 无
  */
void GPS_Init(void)
{
    // 使能GPIO时钟
    RCC_APB2PeriphClockCmd(GPS_GPIO_CLK, ENABLE);
    
    // 使能USART3时钟
    RCC_APB1PeriphClockCmd(GPS_USART_CLK, ENABLE);
    
    // 配置GPIO
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 配置TX引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPS_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPS_GPIO_PORT, &GPIO_InitStructure);
    
    // 配置RX引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPS_RX_PIN;
    GPIO_Init(GPS_GPIO_PORT, &GPIO_InitStructure);
    
    // 配置USART3
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(GPS_USART, &USART_InitStructure);
    
    // 配置NVIC
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // 启用USART2接收中断
    USART_ITConfig(GPS_USART, USART_IT_RXNE, ENABLE);
    
    // 使能USART2
    USART_Cmd(GPS_USART, ENABLE);
}

/**
  * @brief  发送一个字节
  * @param  byte 要发送的字节
  * @retval 无
  */
void GPS_SendByte(uint8_t byte)
{
    USART_SendData(GPS_USART, byte);
    while(USART_GetFlagStatus(GPS_USART, USART_FLAG_TXE) == RESET);
}

/**
  * @brief  发送字符串
  * @param  str 要发送的字符串
  * @retval 无
  */
void GPS_SendString(char* str)
{
    while(*str != '\0')
    {
        GPS_SendByte(*str++);
    }
}

/**
  * @brief  读取一个字节
  * @param  无
  * @retval 读取的字节
  */
uint8_t GPS_ReadByte(void)
{
    while(USART_GetFlagStatus(GPS_USART, USART_FLAG_RXNE) == RESET);
    return USART_ReceiveData(GPS_USART);
}

/**
  * @brief  显示GPS信息
  * @param  gpsx GPS数据结构体
  * @retval 无
  */
void Gps_Msg_Show(nmea_msg *gpsx)
{
    char dtbuf[30];
    float tp;
    
    // 显示经度
    tp = gpsx->longitude;
    sprintf(dtbuf, "%.5f %c", tp / 100000, gpsx->ewhemi);
    OLED_ShowString(1, 1, "Lng:");
    OLED_ShowString(1, 5, dtbuf);
    
    // 显示纬度
    tp = gpsx->latitude;
    sprintf(dtbuf, "%.5f %c", tp / 100000, gpsx->nshemi);
    OLED_ShowString(2, 1, "Lat:");
    OLED_ShowString(2, 5, dtbuf);
    
    // 显示GPS状态
    if(gpsx->gpssta == 1 || gpsx->gpssta == 2)
    {
        OLED_ShowString(3, 1, "Status:FIX");
    }
    else
    {
        OLED_ShowString(3, 1, "Status:NOFIX");
    }
    
    // 显示卫星数量
    sprintf(dtbuf, "Sat:%d", gpsx->svnum);
    OLED_ShowString(4, 1, dtbuf);
}

/**
  * @brief  解析GPS缓冲区数据
  * @param  无
  * @retval 无
  */
void parseGpsBuffer(void)
{
    char *subString;
    char *subStringNext;
    char i = 0;
    if (Save_Data.isGetData)
    {
        Save_Data.isGetData = false;

        for (i = 0 ; i <= 6 ; i++)
        {
            if (i == 0)
            {
                if ((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL)
                {
                    return; // 解析失败
                }
            }
            else
            {
                subString++;
                if ((subStringNext = strstr(subString, ",")) != NULL)
                {
                    char usefullBuffer[2];
                    uint16_t len = subStringNext - subString;
                    
                    switch(i)
                    {
                        case 1:
                            if(len < 11) memcpy(Save_Data.UTCTime, subString, len);
                            break;   // 获取UTC时间
                        case 2:
                            if(len < 2) memcpy(usefullBuffer, subString, len);
                            break;   // 获取有效性
                        case 3:
                            if(len < 11) memcpy(Save_Data.latitude, subString, len);
                            break;   // 获取纬度信息
                        case 4:
                            if(len < 2) memcpy(Save_Data.N_S, subString, len);
                            break;   // 获取N/S
                        case 5:
                            if(len < 12) memcpy(Save_Data.longitude, subString, len);
                            break;   // 获取经度信息
                        case 6:
                            if(len < 2) memcpy(Save_Data.E_W, subString, len);
                            break;   // 获取E/W

                        default:break;
                    }

                    subString = subStringNext;
                    Save_Data.isParseData = true;
                    if(usefullBuffer[0] == 'A')
                        Save_Data.isUsefull = true;
                    else if(usefullBuffer[0] == 'V')
                        Save_Data.isUsefull = false;

                }
                else
                {
                    return; // 解析失败
                }
            }
        }
    }
}

/**
  * @brief  清除GPS数据结构
  * @param  无
  * @retval 无
  */
void clrStruct(void)
{
    Save_Data.isGetData = false;
    Save_Data.isParseData = false;
    Save_Data.isUsefull = false;
    memset(Save_Data.GPS_Buffer, 0, USART_MAX_RECV_LEN);
    memset(Save_Data.UTCTime, 0, 11);
    memset(Save_Data.latitude, 0, 11);
    memset(Save_Data.N_S, 0, 2);
    memset(Save_Data.longitude, 0, 12);
    memset(Save_Data.E_W, 0, 2);
}

/**
  * @brief  清空GPS接收缓冲区
  * @param  无
  * @retval 无
  */
void CLR_Buf(void)
{
    memset(GPS_RX_BUF, 0, USART_MAX_RECV_LEN);
    point1 = 0;
}
