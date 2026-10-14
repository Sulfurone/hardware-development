#ifndef __GPS_H
#define __GPS_H
#include "stm32f10x.h"
#include <string.h>
#include <stdbool.h>

#define GPS_USART        USART2
#define GPS_USART_CLK    RCC_APB1Periph_USART2
#define GPS_GPIO_CLK     RCC_APB2Periph_GPIOA
#define GPS_TX_PIN       GPIO_Pin_2
#define GPS_RX_PIN       GPIO_Pin_3
#define GPS_GPIO_PORT    GPIOA

#define USART_MAX_SEND_LEN 1024
#define USART_MAX_RECV_LEN 1024

//ATGM336H 数据结构
typedef struct
{
    char GPS_Buffer[USART_MAX_RECV_LEN];
    char UTCTime[11];      // UTC时间
    char latitude[11];      // 纬度
    char N_S[2];           // 北纬/南纬
    char longitude[12];     // 经度
    char E_W[2];           // 东经/西经
    bool isGetData;        // 是否获取到数据
    bool isParseData;      // 是否解析完成
    bool isUsefull;        // 数据是否有效
}ATGM336H_Data;

//卫星信息
typedef struct  
{
    uint8_t num;        //卫星编号
    uint8_t eledeg;    //卫星仰角
    uint16_t azideg;    //卫星方位角
    uint8_t sn;        //信噪比           
}nmea_slmsg;

//UTC时间信息
typedef struct  
{
    uint16_t year;    //年份
    uint8_t month;    //月份
    uint8_t date;    //日期
    uint8_t hour;     //小时
    uint8_t min;      //分钟
    uint8_t sec;      //秒钟
}nmea_utc_time;

//NMEA 0183 协议解析后数据存放结构体
typedef struct  
{
    uint8_t svnum;                    //可见卫星数
    nmea_slmsg slmsg[12];        //最多12颗卫星
    nmea_utc_time utc;            //UTC时间
    uint32_t latitude;                //纬度 分扩大100000倍,实际要除以100000
    uint8_t nshemi;                    //北纬/南纬,N:北纬;S:南纬                  
    uint32_t longitude;                //经度 分扩大100000倍,实际要除以100000
    uint8_t ewhemi;                    //东经/西经,E:东经;W:西经
    uint8_t gpssta;                    //GPS状态:0,未定位;1,非差分定位;2,差分定位;6,正在估算.                  
    uint8_t posslnum;                //用于定位的卫星数,0~12.
    uint8_t possl[12];                //用于定位的卫星编号
    uint8_t fixmode;                    //定位类型:1,没有定位;2,2D定位;3,3D定位
    uint16_t pdop;                    //位置精度因子 0~500,对应实际值0~50.0
    uint16_t hdop;                    //水平精度因子 0~500,对应实际值0~50.0
    uint16_t vdop;                    //垂直精度因子 0~500,对应实际值0~50.0

    int altitude;                 //海拔高度,放大了10倍,实际除以10.单位:0.1m     
    uint16_t speed;                    //地面速率,放大了1000倍,实际除以10.单位:0.001公里/小时     
}nmea_msg;

extern uint8_t GPS_RX_BUF[USART_MAX_RECV_LEN];
extern uint16_t GPS_RX_LEN;
extern ATGM336H_Data Save_Data;

void GPS_Init(void);
void GPS_SendByte(uint8_t byte);
void GPS_SendString(char* str);
uint8_t GPS_ReadByte(void);
void GPS_Analysis(nmea_msg *gpsx, uint8_t *buf);
void Gps_Msg_Show(nmea_msg *gpsx);
void parseGpsBuffer(void);
void clrStruct(void);

#endif