#include "MotionControl.h"
#include "Motor.h"
#include "PID.h"

extern float MPU6050_GetAngle_Z(void);

#define STRAIGHT_HEADING_KP         18.0f
#define STRAIGHT_HEADING_KI         0.10f
#define STRAIGHT_HEADING_KD         10.0f
#define STRAIGHT_CORRECTION_LIMIT   220.0f
#define STRAIGHT_HEADING_DEADBAND   0.5f

static int16_t current_pwm = 0;
static float start_angle = 0;
static PID_TypeDef straight_pid;

static float NormalizeAngleError(float angle)
{
    while (angle > 180.0f)
    {
        angle -= 360.0f;
    }

    while (angle < -180.0f)
    {
        angle += 360.0f;
    }

    return angle;
}

static int16_t LimitPwmValue(int32_t pwm)
{
    if (pwm > 1000)
    {
        return 1000;
    }

    if (pwm < -1000)
    {
        return -1000;
    }

    return (int16_t)pwm;
}

void MotionControl_Init(void)
{
    current_pwm = 0;
    start_angle = MPU6050_GetAngle_Z();
    PID_Init(&straight_pid,
             STRAIGHT_HEADING_KP,
             STRAIGHT_HEADING_KI,
             STRAIGHT_HEADING_KD,
             STRAIGHT_CORRECTION_LIMIT);
}

void MotionControl_ResetAngle(void)
{
    start_angle = MPU6050_GetAngle_Z();
    PID_Reset(&straight_pid);
}

void MotionControl_Straight(int16_t pwm)
{
    float current_angle = MPU6050_GetAngle_Z();
    float angle_error;
    float correction;
    int16_t left_pwm;
    int16_t right_pwm;

    if (current_pwm == 0 ||
        ((current_pwm > 0) && (pwm < 0)) ||
        ((current_pwm < 0) && (pwm > 0)))
    {
        MotionControl_ResetAngle();
        current_angle = MPU6050_GetAngle_Z();
    }

    angle_error = NormalizeAngleError(current_angle - start_angle);
    if ((angle_error < STRAIGHT_HEADING_DEADBAND) &&
        (angle_error > -STRAIGHT_HEADING_DEADBAND))
    {
        angle_error = 0.0f;
    }

    correction = PID_Calculate(&straight_pid, 0.0f, -angle_error);

    if (pwm >= 0)
    {
        left_pwm = LimitPwmValue((int32_t)pwm + (int32_t)correction);
        right_pwm = LimitPwmValue((int32_t)pwm - (int32_t)correction);
    }
    else
    {
        left_pwm = LimitPwmValue((int32_t)pwm + (int32_t)correction);
        right_pwm = LimitPwmValue((int32_t)pwm - (int32_t)correction);
    }
    Motor_SetSpeed(left_pwm, right_pwm);
    current_pwm = pwm;
}

void MotionControl_Turn(int16_t pwm_left, int16_t pwm_right)
{
    PID_Reset(&straight_pid);
    current_pwm = 0;
    Motor_SetSpeed(pwm_left, pwm_right);
}

void MotionControl_Stop(void)
{
    Motor_Stop();
    PID_Reset(&straight_pid);
    current_pwm = 0;
}

void MotionControl_Update(void)
{
}
