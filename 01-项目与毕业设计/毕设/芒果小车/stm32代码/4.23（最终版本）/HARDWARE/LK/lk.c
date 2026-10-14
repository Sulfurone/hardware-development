#include "lk.h"            // 本模块头文件
#include "sys.h"           // 系统初始化
#include "delay.h"         // 延时函数
#include "control.h"       // 控制逻辑

// 全局变量定义
int sensor[4] = {0, 0, 0, 0};   // 红外传感器返回值数组，sensor[0]~sensor[3]分别对应不同位置
extern int stopflag;            // 停车标志（外部变量）
int Prestopconfig = 0;          // 终点线识别标志
int OUTPUT, OUTPUT1, OUTPUT2;   // 舵机PWM输出值，左右轮PWM输出值
int duty = 0;                   // 占空比调整量
int c = 0;                      // 小车位置偏移量（误差）
int a = 0;                      // 小车基础速度标志变量
enum {SENSOR_LEFT1, SENSOR_LEFT2, SENSOR_RIGHT2, SENSOR_RIGHT1}; // 传感器索引宏
int stopconfig = 0;             // 终点线检测标志
int error = 0, error1, error2;  // 误差相关变量

// 红外传感器IO初始化
void hongwai_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // 使能GPIOB时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  // 使能复用功能时钟

    // PA6, PA7 浮空输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // PB1, PB3 浮空输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 关闭JTAG，释放PB3、PB4为普通IO
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}

// 读取红外传感器数据并根据结果设置PWM输出
// 传感器识别到黑线返回1，未识别到黑线返回0
void read_sensor(void)
{
    // 读取各个红外传感器的状态
    sensor[0] = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1); // 左1
    sensor[1] = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3); // 左2
    sensor[2] = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7); // 右2
    sensor[3] = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6); // 右1

    // 传感器全为0，直行
    if (sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 0 && sensor[3] == 0)
    {
        OUTPUT1 = 4600 + duty;
        OUTPUT2 = 4600 + duty;
    }
    // 左1检测到黑线，右转
    if (sensor[0] == 1)
    {
        OUTPUT1 = 0;
        OUTPUT2 = 5600;
    }
    // 左2检测到黑线，右急转
    if (sensor[1] == 1)
    {
        OUTPUT1 = 0;
        OUTPUT2 = 5200;
    }
    // 右2检测到黑线，左急转
    if (sensor[2] == 1)
    {
        OUTPUT1 = 5200;
        OUTPUT2 = 0;
    }
    // 右1检测到黑线，左转
    if (sensor[3] == 1)
    {
        OUTPUT1 = 5600;
        OUTPUT2 = 0;
    }
    // 左1和右1同时检测到黑线，停车
    if (sensor[0] == 1 && sensor[3] == 1)
    {
        stopflag = 1;
        OUTPUT1 = 0;
        OUTPUT2 = 0;
    }
}
