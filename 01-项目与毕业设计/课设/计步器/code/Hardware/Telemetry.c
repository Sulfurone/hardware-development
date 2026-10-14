#include "Telemetry.h"
#include <stdio.h>
#include <string.h>

void Telemetry_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    // USART1 TX (PA9)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // USART1 RX (PA10)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 9600; // 适配常规蓝牙模块(如HC-05/HC-08)的默认波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStructure);

    USART_Cmd(USART1, ENABLE);
}

void Telemetry_SendByte(uint8_t byte)
{
    USART_SendData(USART1, byte);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void Telemetry_SendString(char *str)
{
    while (*str != '\0')
    {
        Telemetry_SendByte(*str++);
    }
}

uint8_t Telemetry_ReadByte(uint8_t *byte)
{
    if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
    {
        return 0;
    }

    *byte = (uint8_t)USART_ReceiveData(USART1);
    return 1;
}

void Telemetry_ReportMotionFault(float lat, float lon)
{
    char buf[128];
    sprintf(buf, "{\"Alarm\":\"MOTION_FAULT\",\"Lat\":%.6f,\"Lon\":%.6f}\r\n", lat, lon);
    Telemetry_SendString(buf);
}

void Telemetry_ReportRoadFault(float lat, float lon)
{
    char buf[128];
    sprintf(buf, "{\"Alarm\":\"ROAD_FAULT\",\"Lat\":%.6f,\"Lon\":%.6f}\r\n", lat, lon);
    Telemetry_SendString(buf);
}

void Telemetry_ReportLearnPoint(uint16_t index, float lat, float lon, float temp, float hum, float smoke, uint8_t gps_state)
{
    char buf[256];
    sprintf(buf, "{\"Learn\":%u,\"Lat\":%.6f,\"Lon\":%.6f,\"T\":%.1f,\"H\":%.1f,\"S\":%.1f,\"Gps\":%u}\r\n",
            index, lat, lon, temp, hum, smoke, gps_state);
    Telemetry_SendString(buf);
}

void Telemetry_ReportSmokeAlarm(float temp, float smoke, float lat, float lon)
{
    char buf[256];
    sprintf(buf, "{\"Alarm\":\"SMOKE_FIRE\",\"T\":%.1f,\"S\":%.1f,\"Lat\":%.6f,\"Lon\":%.6f}\r\n",
            temp, smoke, lat, lon);
    Telemetry_SendString(buf);
}

// JSON 格式上报状态给控制中心
void Telemetry_ReportStatus(float temp, float hum, float smoke, float lat, float lon, int is_stuck, int is_smoke_alarm, int is_road_fault)
{
    char buf[256];
    sprintf(buf, "{\"T\":%.1f,\"H\":%.1f,\"S\":%.1f,\"Lat\":%.6f,\"Lon\":%.6f,\"Stuck\":%d,\"SmokeAlarm\":%d,\"Fault\":%d}\r\n",
            temp, hum, smoke, lat, lon, is_stuck, is_smoke_alarm, is_road_fault);
    Telemetry_SendString(buf);
}
