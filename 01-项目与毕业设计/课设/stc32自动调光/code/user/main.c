#include "zf_common_headfile.h"

/*
 * Smart desk lamp
 * - Default auto mode: the relay turns on when the light sensor reports dark.
 * - Press the key to enter manual mode and toggle the relay.
 * - If there is no key operation for 10 seconds in manual mode, return to auto.
 *
 * Wiring can be changed here:
 * LIGHT_ADC_CHANNEL : light sensor ADC input, P1.0.
 * KEY_PIN           : manual key, P4.6, active low with pull-up.
 * RELAY_PIN         : relay control pin, P2.0.
 */
#define LIGHT_ADC_CHANNEL           ADC_P10
#define KEY_PIN                     IO_P46
#define RELAY_PIN                   IO_P20

#define KEY_PRESSED_LEVEL           0
#define LIGHT_ADC_RESOLUTION        ADC_12BIT
#define LIGHT_ADC_CLOCK             ADC_SYSclk_DIV_2
#define LIGHT_DARK_THRESHOLD        2048
#define LIGHT_DARK_WHEN_ADC_BELOW   1
#define RELAY_ON_LEVEL              1
#define RELAY_OFF_LEVEL             0

#define LOOP_PERIOD_MS              10
#define KEY_DEBOUNCE_MS             30
#define MANUAL_TIMEOUT_MS           10000

typedef enum
{
    LAMP_MODE_AUTO = 0,
    LAMP_MODE_MANUAL
} lamp_mode_enum;

static lamp_mode_enum lamp_mode = LAMP_MODE_AUTO;
static uint8 lamp_on = 0;
static uint8 light_dark = 0;
static uint8 key_last_level = 1;
static uint16 light_adc_value = 0;
static uint16 manual_idle_ms = 0;

static void lamp_set(uint8 on)
{
    lamp_on = on ? 1 : 0;
    gpio_set_level(RELAY_PIN, lamp_on ? RELAY_ON_LEVEL : RELAY_OFF_LEVEL);
}

static uint8 light_is_dark(void)
{
    return light_dark;
}

static void light_adc_update(void)
{
    light_adc_value = adc_once(LIGHT_ADC_CHANNEL, LIGHT_ADC_RESOLUTION);

#if LIGHT_DARK_WHEN_ADC_BELOW
    light_dark = (light_adc_value < LIGHT_DARK_THRESHOLD);
#else
    light_dark = (light_adc_value > LIGHT_DARK_THRESHOLD);
#endif
}

static uint8 key_scan_click(void)
{
    uint8 key_level = gpio_get_level(KEY_PIN);
    uint8 clicked = 0;

    if((key_last_level != KEY_PRESSED_LEVEL) && (key_level == KEY_PRESSED_LEVEL))
    {
        system_delay_ms(KEY_DEBOUNCE_MS);
        if(gpio_get_level(KEY_PIN) == KEY_PRESSED_LEVEL)
        {
            clicked = 1;
        }
    }

    key_last_level = key_level;
    return clicked;
}

static void auto_control_update(void)
{
    if(light_is_dark())
    {
        lamp_set(1);
    }
    else
    {
        lamp_set(0);
    }
}

static void display_update(void)
{
    ips114_show_string(0, 0, "MODE:");
    ips114_show_string(6 * 8, 0, (lamp_mode == LAMP_MODE_AUTO) ? "AUTO  " : "MANUAL");

    ips114_show_string(0, 16, "LAMP:");
    ips114_show_string(6 * 8, 16, lamp_on ? "ON " : "OFF");

    ips114_show_string(0, 32, "LIGHT:");
    ips114_show_string(7 * 8, 32, light_is_dark() ? "DARK  " : "BRIGHT");
}

void main(void)
{
    clock_init(SYSTEM_CLOCK_30M);
    debug_init();

    adc_init(LIGHT_ADC_CHANNEL, LIGHT_ADC_CLOCK, LIGHT_ADC_RESOLUTION);
    gpio_init(KEY_PIN, GPI, 1, GPI_PULL_UP);
    gpio_init(RELAY_PIN, GPO, RELAY_OFF_LEVEL, GPO_PUSH_PULL);

    ips114_init();
    lamp_set(0);

    while(1)
    {
        light_adc_update();

        if(key_scan_click())
        {
            lamp_mode = LAMP_MODE_MANUAL;
            manual_idle_ms = 0;
            lamp_set(!lamp_on);
        }

        if(lamp_mode == LAMP_MODE_AUTO)
        {
            auto_control_update();
        }
        else
        {
            if(manual_idle_ms >= MANUAL_TIMEOUT_MS)
            {
                lamp_mode = LAMP_MODE_AUTO;
                manual_idle_ms = 0;
                auto_control_update();
            }
            else
            {
                manual_idle_ms += LOOP_PERIOD_MS;
            }
        }

        display_update();
        system_delay_ms(LOOP_PERIOD_MS);
    }
}
