#include "bsp_delay.h"
#include "oled_ssd1306.h"
#include "tester_hw.h"
#include "tester_measure.h"
#include <stdio.h>

static char TP_Name(uint8_t tp)
{
    return (char)('1' + tp);
}

static void Format_Resistance(char *buf, uint32_t ohm)
{
    if (ohm >= 1000000UL) {
        sprintf(buf, "%lu.%02lu MOHM", ohm / 1000000UL, (ohm % 1000000UL) / 10000UL);
    } else if (ohm >= 1000UL) {
        sprintf(buf, "%lu.%02lu KOHM", ohm / 1000UL, (ohm % 1000UL) / 10UL);
    } else {
        sprintf(buf, "%lu OHM", ohm);
    }
}

static void Format_Capacitance(char *buf, uint32_t nf)
{
    if (nf >= 1000UL) {
        sprintf(buf, "%lu.%02lu UF", nf / 1000UL, (nf % 1000UL) / 10UL);
    } else {
        sprintf(buf, "%lu NF", nf);
    }
}

static void UI_ShowBoot(void)
{
    char line[24];

    OLED_Clear();
    OLED_Text(0, 0, "STM32 T-TESTER");
    OLED_Text(0, 2, "OLED I2C SSD1306");

    sprintf(line, "BAT:%uMV", TesterHW_BatteryMv());
    OLED_Text(0, 4, line);

    sprintf(line, "REF:%uMV", TesterHW_RefMv());
    OLED_Text(0, 6, line);
}

static void UI_ShowMeasuring(void)
{
    OLED_Clear();
    OLED_Text(0, 0, "MEASURING...");
    OLED_Text(0, 2, "TP1 TP2 TP3");
}

static void UI_ShowResult(const PartResult *result)
{
    char line[24];

    OLED_Clear();

    switch (result->type) {
    case PART_DIODE:
        OLED_Text(0, 0, "DIODE");
        sprintf(line, "A:%c  K:%c", TP_Name(result->pin_a), TP_Name(result->pin_b));
        OLED_Text(0, 2, line);
        sprintf(line, "VF:%u MV", result->extra_mv);
        OLED_Text(0, 4, line);
        break;

    case PART_DUAL_DIODE:
        OLED_Text(0, 0, "DUAL DIODE");
        sprintf(line, "DIO:%u", result->diodes.count);
        OLED_Text(0, 2, line);
        break;

    case PART_RESISTOR:
        OLED_Text(0, 0, "RESISTOR");
        sprintf(line, "P%c-P%c", TP_Name(result->pin_a), TP_Name(result->pin_b));
        OLED_Text(0, 2, line);
        Format_Resistance(line, result->value);
        OLED_Text(0, 4, line);
        break;

    case PART_CAPACITOR:
        OLED_Text(0, 0, "CAPACITOR");
        sprintf(line, "P%c-P%c", TP_Name(result->pin_a), TP_Name(result->pin_b));
        OLED_Text(0, 2, line);
        Format_Capacitance(line, result->value);
        OLED_Text(0, 4, line);
        break;

    case PART_NPN:
        OLED_Text(0, 0, "NPN TRANS");
        sprintf(line, "BASE:%c", TP_Name(result->pin_b));
        OLED_Text(0, 2, line);
        OLED_Text(0, 4, "C/E NEED GAIN TEST");
        break;

    case PART_PNP:
        OLED_Text(0, 0, "PNP TRANS");
        sprintf(line, "BASE:%c", TP_Name(result->pin_b));
        OLED_Text(0, 2, line);
        OLED_Text(0, 4, "C/E NEED GAIN TEST");
        break;

    default:
        OLED_Text(0, 0, "NO/UNKNOWN PART");
        OLED_Text(0, 2, "INSERT AND PRESS");
        break;
    }
}

static void Wait_NextPress(void)
{
    while (!TesterHW_ButtonPressed()) {
        Delay_Ms(20);
    }

    Delay_Ms(30);
    while (TesterHW_ButtonPressed()) {
        Delay_Ms(20);
    }
}

int main(void)
{
    PartResult result;

    Delay_Init();
    TesterHW_Init();
    OLED_Init();

    UI_ShowBoot();
    Delay_Ms(1200);

    while (1) {
        TesterHW_Led(1);
        UI_ShowMeasuring();

        result = TesterMeasure_Detect();

        TesterHW_Led(0);
        UI_ShowResult(&result);
        Wait_NextPress();
    }
}
