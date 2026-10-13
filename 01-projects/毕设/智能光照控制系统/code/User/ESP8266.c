#include "ESP8266.h"
#include "USART2.h"

void ESP8266_Init(void)
{
    Serial_Init();
    // 这里可以添加AT指令初始化WiFi模块
}

void ESP8266_SendData(uint8_t temp, uint8_t humi, uint16_t light, uint8_t human)
{
    Serial_Printf("@DATA,%d,%d,%d,%d\r\n", temp, humi, light, human);
}

void ESP8266_ReceiveCmd(void)
{
    // 这里可以解析串口接收到的命令并做相应处理
    // if(Serial_RxFlag) { ... }
}

