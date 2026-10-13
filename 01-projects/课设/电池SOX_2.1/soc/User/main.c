#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "tftlcd.h"
#include "key.h"
#include "touch.h"
#include "battery.h"
#include "stdio.h"

// 显示缓冲区
char disp_buf[32];

// 绘制进度条
void DrawBar(u16 x, u16 y, u16 w, u16 h, u8 percent, u16 barColor, u16 bgColor)
{
    u16 fill_w;

    // 背景
    LCD_Fill(x, y, x + w - 1, y + h - 1, bgColor);

    // 前景
    fill_w = (u16)((u32)w * percent / 100);
    if (fill_w > w) fill_w = w;
    if (fill_w > 0)
        LCD_Fill(x, y, x + fill_w - 1, y + h - 1, barColor);

    // 边框
    FRONT_COLOR = BLACK;
    LCD_DrawRectangle(x, y, x + w - 1, y + h - 1);
}

// 屏幕初始化
void Battery_DisplayInit(void)
{
    LCD_Clear(WHITE);

    FRONT_COLOR = BLUE;
    LCD_ShowString(10, 5, tftlcd_data.width, tftlcd_data.height, 16, (u8 *)"=== Battery Monitor ===");

    // 标签
    FRONT_COLOR = BLACK;
    LCD_ShowString(10, 30,  tftlcd_data.width, tftlcd_data.height, 16, (u8 *)"VOUTV:");
    LCD_ShowString(10, 55,  tftlcd_data.width, tftlcd_data.height, 16, (u8 *)"VOUTI:");
    LCD_ShowString(10, 85,  tftlcd_data.width, tftlcd_data.height, 16, (u8 *)"Bat Volt:");
    LCD_ShowString(10, 110, tftlcd_data.width, tftlcd_data.height, 16, (u8 *)"Current:");
    LCD_ShowString(10, 145, tftlcd_data.width, tftlcd_data.height, 16, (u8 *)"SOC:");

    LCD_ShowString(10, tftlcd_data.height - 20, tftlcd_data.width, tftlcd_data.height, 16,
                   (u8 *)"SOC Monitor  V1.3");
}

int main()
{
    u8   i = 0;
    u8   key;
    u8   soc_percent;
    float voutv, vouti, bat_v, curr, soc;

    SysTick_Init(72);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    LED_Init();
    USART1_Init(115200);
    TFTLCD_Init();
    KEY_Init();
    TP_Init();

    BATTERY_ADC_Init();         // ADC初始化

    Battery_DisplayInit();      // 屏幕初始画面

    while (1)
    {
        // 按键触摸屏校准
        key = KEY_Scan(0);
        if (key == KEY_UP_PRESS)
        {
            TP_Adjust();
            Battery_DisplayInit();
        }

        // 每500ms刷新一次（i每10ms +1，50次=500ms）
        i++;
        if (i >= 50)
        {
            i = 0;

            // 更新电池数据（库仑计积分），距上次调用0.5秒
            BATTERY_Update(0.5f);

            // 读取缓存数据
            voutv = BATTERY_GetVOUTV_V();
            vouti = BATTERY_GetVOUTI_V();
            bat_v = BATTERY_GetVoltage();
            curr  = BATTERY_GetCurrent();
            soc   = BATTERY_GetSOC();

            soc_percent = (u8)(soc + 0.5f);

            // ---- 显示VOUTV ----
            FRONT_COLOR = BLUE;
            sprintf(disp_buf, "%.3f V", voutv);
            LCD_ShowString(80, 30, tftlcd_data.width, tftlcd_data.height, 16, (u8 *)disp_buf);

            // ---- 显示VOUTI ----
            sprintf(disp_buf, "%.3f V", vouti);
            LCD_ShowString(80, 55, tftlcd_data.width, tftlcd_data.height, 16, (u8 *)disp_buf);

            // ---- 显示电池电压 ----
            sprintf(disp_buf, "%.2f V", bat_v);
            LCD_ShowString(80, 85, tftlcd_data.width, tftlcd_data.height, 16, (u8 *)disp_buf);

            // ---- 显示电流（带方向符号） ----
            if (curr >= 0)
                sprintf(disp_buf, "+%.3f A", curr);
            else
                sprintf(disp_buf, "%.3f A",  curr);
            LCD_ShowString(80, 110, tftlcd_data.width, tftlcd_data.height, 16, (u8 *)disp_buf);

            // ---- 显示SOC（进度条+百分比） ----
            FRONT_COLOR = BLACK;
            sprintf(disp_buf, "%3d %%", soc_percent);
            LCD_ShowString(80, 145, tftlcd_data.width, tftlcd_data.height, 16, (u8 *)disp_buf);
            DrawBar(130, 145, 100, 16, soc_percent, GREEN, LIGHTGRAY);

            // LED 心跳
            LED1 = !LED1;
        }

        delay_ms(10);
    }
}
