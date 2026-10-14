/**
************************************************************
* @file         gizwits_product.c
* @brief        Gizwits control protocol processing, and platform-related hardware initialization 
* @author       Gizwits
* @date         2017-07-19
* @version      V03030000
* @copyright    Gizwits
*
* @note         Gizwits is only for smart hardware
*               Gizwits Smart Cloud for Smart Products
*               Links | Value Added | Open | Neutral | Safety | Own | Free | Ecology
*               www.gizwits.com
*
***********************************************************/
#include <stdio.h>
#include <string.h>
#include "gizwits_product.h"
#include "usart2.h"
#include "led.h"
#include "light_mode.h"  // 包含灯光模式定义

extern u8 temp;
extern u8 humi;
extern u16 light2;
extern volatile uint8_t light_power;      // 灯光电源状态
extern volatile uint8_t online_control;   // 在线控制标志
extern volatile LightMode_t light_mode;   // 控制模式
extern volatile uint16_t pwm_value;       // PWM值

static uint32_t timerMsCount;

extern int human;
/** Current datapoint */
dataPoint_t currentDataPoint;

int8_t gizwitsEventProcess(eventInfo_t *info, uint8_t *gizdata, uint32_t len)
{
    uint8_t i = 0;
    dataPoint_t *dataPointPtr = (dataPoint_t *)gizdata;
    moduleStatusInfo_t *wifiData = (moduleStatusInfo_t *)gizdata;
    protocolTime_t *ptime = (protocolTime_t *)gizdata;
    
    // 保存当前湿度值，以便在处理APP控制命令后恢复
    uint32_t currentHumi = currentDataPoint.valuehumi;
    
#if MODULE_TYPE
    gprsInfo_t *gprsInfoData = (gprsInfo_t *)gizdata;
#else
    moduleInfo_t *ptModuleInfo = (moduleInfo_t *)gizdata;
#endif

    if((NULL == info) || (NULL == gizdata))
    {
        return -1;
    }

    for(i=0; i<info->num; i++)
    {
        switch(info->event[i])
        {
        case EVENT_led:
            // 只在在线模式下处理LED控制命令
            if(light_mode == MODE_ONLINE)
            {
                currentDataPoint.valueled = dataPointPtr->valueled;
                GIZWITS_LOG("Evt: EVENT_led %d \n", currentDataPoint.valueled);
                if(0x01 == currentDataPoint.valueled)
                {
                    LED_ON();
                }
                else
                {
                    LED_OFF();
                }
            }
            else
            {
                // 不在在线模式下，忽略APP控制命令
                GIZWITS_LOG("Ignore LED control command: not in ONLINE mode\n");
            }
            break;
            
        case EVENT_light:
            // 只在在线模式下处理灯光控制命令
            if(light_mode == MODE_ONLINE)
            {
                currentDataPoint.valuelight = dataPointPtr->valuelight;
                GIZWITS_LOG("Evt: EVENT_light %d \n", currentDataPoint.valuelight);
                
                // 更新灯光状态
                light_power = currentDataPoint.valuelight;
                
                // 根据灯光状态控制LED灯
                if(light_power)
                {
                    LED_ON();
                }
                else
                {
                    LED_OFF();
                }
            }
            else
            {
                // 不在在线模式下，忽略APP控制命令
                GIZWITS_LOG("Ignore LIGHT control command: not in ONLINE mode\n");
            }
            break;

        case WIFI_SOFTAP:
            break;
        case WIFI_AIRLINK:
            break;
        case WIFI_STATION:
            break;
        case WIFI_CON_ROUTER:
 
            break;
        case WIFI_DISCON_ROUTER:
 
            break;
        case WIFI_CON_M2M:
 
            break;
        case WIFI_DISCON_M2M:
            break;
        case WIFI_RSSI:
            GIZWITS_LOG("RSSI %d\n", wifiData->rssi);
            break;
        case TRANSPARENT_DATA:
            GIZWITS_LOG("TRANSPARENT_DATA \n");
            //user handle , Fetch data from [data] , size is [len]
            break;
        case WIFI_NTP:
            GIZWITS_LOG("WIFI_NTP : [%d-%d-%d %02d:%02d:%02d][%d] \n",ptime->year,ptime->month,ptime->day,ptime->hour,ptime->minute,ptime->second,ptime->ntp);
            break;
        case MODULE_INFO:
            GIZWITS_LOG("MODULE INFO ...\n");
#if MODULE_TYPE
            GIZWITS_LOG("GPRS MODULE ...\n");
            //Format By gprsInfo_t
            GIZWITS_LOG("moduleType : [%d] \n",gprsInfoData->Type);
#else
            GIZWITS_LOG("WIF MODULE ...\n");
            //Format By moduleInfo_t
            GIZWITS_LOG("moduleType : [%d] \n",ptModuleInfo->moduleType);
#endif
        break;
        default:
            break;
        }
    }
    
    // 恢复湿度值，防止APP控制命令覆盖本地湿度数据
    currentDataPoint.valuehumi = currentHumi;

    return 0;
}

/**
* User data acquisition

* Here users need to achieve in addition to data points other than the collection of data collection, can be self-defined acquisition frequency and design data filtering algorithm

* @param none
* @return none
*/
void userHandle(void)
{
    // 温度数据采集
    currentDataPoint.valuetemp = temp;
    
    // 湿度数据采集
    currentDataPoint.valuehumi = humi;
    
    // 光照强度数据采集 - 确保非零值
    if (light2 > 0) {
        currentDataPoint.valuelight2 = light2;
    } else {
        // 如果light2为0，使用最小默认值
        currentDataPoint.valuelight2 = 1;
    }
    
    // 灯光开关状态
    currentDataPoint.valuelight = light_power;
    
    // 人体存在状态数据采集
    currentDataPoint.valuehuman = human;
}

/**
* Data point initialization function

* In the function to complete the initial user-related data
* @param none
* @return none
* @note The developer can add a data point state initialization value within this function
*/
void userInit(void)
{
    memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));
    
    /** Warning !!! DataPoint Variables Init , Must Within The Data Range **/ 
    currentDataPoint.valueled = 0;
    currentDataPoint.valuetemp = 25;  // 设置一个合理的初始温度值
    currentDataPoint.valuehumi = 50;  // 设置一个合理的初始湿度值
    currentDataPoint.valuelight2 = 0;  // 初始化光照强度数据点
    currentDataPoint.valuelight = 0;   // 初始化灯光控制状态数据点
    currentDataPoint.valuehuman = 0;  // 初始化人体存在状态数据点
}


/**
* @brief  gizTimerMs

* millisecond timer maintenance function ,Millisecond increment , Overflow to zero

* @param none
* @return none
*/
void gizTimerMs(void)
{
    timerMsCount++;
}

/**
* @brief gizGetTimerCount

* Read system time, millisecond timer

* @param none
* @return System time millisecond
*/
uint32_t gizGetTimerCount(void)
{
    return timerMsCount;
}

/**
* @brief mcuRestart

* MCU Reset function

* @param none
* @return none
*/
void mcuRestart(void)
{
	__set_FAULTMASK(1);
	NVIC_SystemReset();
}
/**@} */

/**
* @brief TIMER_IRQ_FUN

* Timer Interrupt handler function

* @param none
* @return none
*/
void TIMER_IRQ_FUN(void)
{
  gizTimerMs();
}

/**
* @brief UART_IRQ_FUN

* UART Serial interrupt function ，For Module communication

* Used to receive serial port protocol data between WiFi module

* @param none
* @return none
*/
//void UART_IRQ_FUN(void)
//{
//  uint8_t value = 0;
//  //value = USART_ReceiveData(USART2);//STM32 test demo
//  gizPutData(&value, 1);
//}


/**
* @brief uartWrite

* Serial write operation, send data to the WiFi module

* @param buf      : Data address
* @param len       : Data length
*
* @return : Not 0,Serial send success;
*           -1，Input Param Illegal
*/
int32_t uartWrite(uint8_t *buf, uint32_t len)
{
    uint32_t i = 0;
    
    if(NULL == buf)
    {
        return -1;
    }
    
    #ifdef PROTOCOL_DEBUG
    GIZWITS_LOG("MCU2WiFi[%4d:%4d]: ", gizGetTimerCount(), len);
    for(i=0; i<len; i++)
    {
        GIZWITS_LOG("%02x ", buf[i]);
    }
    GIZWITS_LOG("\n");
    #endif

    for(i=0; i<len; i++)
    {
        USART_SendData(USART2, buf[i]);//STM32 test demo
				while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
        //Serial port to achieve the function, the buf[i] sent to the module
        if(i >=2 && buf[i] == 0xFF)
        {
					USART_SendData(USART2,0x55);
					while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
          //Serial port to achieve the function, the 0x55 sent to the module
          //USART_SendData(UART, 0x55);//STM32 test demo
        }
    }

    return len;
}


