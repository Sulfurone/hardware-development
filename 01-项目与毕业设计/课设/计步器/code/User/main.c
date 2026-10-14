#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "MPU6050.h"
#include "Key.h"

#include <math.h>

#define SAMPLE_PERIOD_MS            20U
#define DISPLAY_PERIOD_MS           200U
#define STEP_MIN_INTERVAL_MS        280U
#define STEP_HIGH_THRESHOLD         1700.0f
#define STEP_LOW_THRESHOLD          450.0f
#define STRIDE_LENGTH_CM            70U
#define KCAL_PER_STEP_X100          4U

typedef struct
{
    uint32_t steps;
    uint32_t elapsed_ms;
    uint32_t last_step_ms;
    uint8_t peak_locked;
    float accel_lp;
    float gravity_lp;
    float motion;
} Pedometer_t;

static MPU6050_Data mpu6050_data;
static Pedometer_t pedometer;

static float AbsFloat(float value)
{
    return (value >= 0.0f) ? value : -value;
}

static void Pedometer_Reset(void)
{
    pedometer.steps = 0;
    pedometer.elapsed_ms = 0;
    pedometer.last_step_ms = 0;
    pedometer.peak_locked = 0;
    pedometer.accel_lp = 16384.0f;
    pedometer.gravity_lp = 16384.0f;
    pedometer.motion = 0.0f;
}

static float Pedometer_GetAccelMagnitude(const MPU6050_Data *data)
{
    float ax = (float)data->accel_x;
    float ay = (float)data->accel_y;
    float az = (float)data->accel_z;

    return (float)sqrt(ax * ax + ay * ay + az * az);
}

static void Pedometer_Update(const MPU6050_Data *data)
{
    float accel_mag = Pedometer_GetAccelMagnitude(data);
    uint32_t interval_ms;

    pedometer.elapsed_ms += SAMPLE_PERIOD_MS;

    pedometer.accel_lp += (accel_mag - pedometer.accel_lp) * 0.20f;
    pedometer.gravity_lp += (pedometer.accel_lp - pedometer.gravity_lp) * 0.02f;
    pedometer.motion = pedometer.accel_lp - pedometer.gravity_lp;

    interval_ms = pedometer.elapsed_ms - pedometer.last_step_ms;

    if (!pedometer.peak_locked &&
        pedometer.motion > STEP_HIGH_THRESHOLD &&
        interval_ms >= STEP_MIN_INTERVAL_MS)
    {
        pedometer.steps++;
        pedometer.last_step_ms = pedometer.elapsed_ms;
        pedometer.peak_locked = 1;
    }
    else if (pedometer.peak_locked &&
             pedometer.motion < STEP_LOW_THRESHOLD)
    {
        pedometer.peak_locked = 0;
    }
}

static void Pedometer_HandleKey(void)
{
    uint8_t key_num = Key_GetNum();

    if (key_num == 1 || key_num == 2)
    {
        Pedometer_Reset();
        OLED_Clear();
        OLED_ShowString(1, 1, "Steps Reset");
        Delay_ms(400);
        OLED_Clear();
    }
}

static void Pedometer_Show(void)
{
    uint32_t show_steps = pedometer.steps;
    uint32_t distance_m;
    uint32_t kcal_x10;
    uint32_t motion_value;

    if (show_steps > 999999U)
    {
        show_steps = 999999U;
    }

    distance_m = pedometer.steps * STRIDE_LENGTH_CM / 100U;
    if (distance_m > 9999U)
    {
        distance_m = 9999U;
    }

    kcal_x10 = pedometer.steps * KCAL_PER_STEP_X100 / 10U;
    if (kcal_x10 > 9999U)
    {
        kcal_x10 = 9999U;
    }

    motion_value = (uint32_t)AbsFloat(pedometer.motion);
    if (motion_value > 9999U)
    {
        motion_value = 9999U;
    }

    OLED_ShowString(1, 1, "Pedometer");

    OLED_ShowString(2, 1, "Steps:000000");
    OLED_ShowNum(2, 7, show_steps, 6);

    OLED_ShowString(3, 1, "Dist:0000m");
    OLED_ShowNum(3, 6, distance_m, 4);

    OLED_ShowString(4, 1, "KCal:000.0 M0000");
    OLED_ShowNum(4, 6, kcal_x10 / 10U, 3);
    OLED_ShowNum(4, 10, kcal_x10 % 10U, 1);
    OLED_ShowNum(4, 13, motion_value, 4);
}

int main(void)
{
    uint32_t display_tick = 0;

    OLED_Init();
    Key_Init();
    MPU6050_Init();

    Pedometer_Reset();

    OLED_Clear();
    OLED_ShowString(1, 1, "Pedometer");
    OLED_ShowString(2, 1, "Ready");
    Delay_ms(800);
    OLED_Clear();

    while (1)
    {
        MPU6050_ReadData(&mpu6050_data);
        Pedometer_Update(&mpu6050_data);
        Pedometer_HandleKey();

        display_tick += SAMPLE_PERIOD_MS;
        if (display_tick >= DISPLAY_PERIOD_MS)
        {
            display_tick = 0;
            Pedometer_Show();
        }

        Delay_ms(SAMPLE_PERIOD_MS);
    }
}
