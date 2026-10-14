#include "stm32f10x.h"                  // Device header
#include "lk.h"
#include "sys.h"
#include "delay.h"
#include "key.h"

extern	float target_voltage ; // 目标电压，可根据需要修改
extern int OUTPUT,OUTPUT1,OUTPUT2;
extern int startflag;
static uint16_t input_voltage = 0; // 用于保存输入的数字

uint16_t input_integer = 0; // 整数部分
uint8_t input_decimal = 0;  // 小数部分（只支持一位小数）
uint8_t inputting = 0;      // 是否正在输入
uint8_t inputting_decimal = 0; // 是否输入小数
uint8_t decimal_pos = 0;    // 小数点后第几位（只支持一位小数时用0/1）

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;

	// 行输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// 列输入上拉
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// 默认行全高
	GPIO_SetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);
}

// 返回键值：1~9为数字，10=确认，11=清零，0=无按键，12=数字0
uint8_t Key_Scan(void)
{
	uint8_t row, col;
	uint16_t row_pins[3] = {GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14};
	uint16_t col_pins[4] = {GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_7};

	// 键值映射表
	uint8_t key_map[3][4] = {
		{1, 2, 3, 10},   // 行1：1 2 3 确认
		{4, 5, 6, 11},   // 行2：4 5 6 清零
		{7, 8, 9, 13}    // 行3：7 8 9 小数点
	};

	for (row = 0; row < 3; row++)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);
		GPIO_ResetBits(GPIOB, row_pins[row]);
		delay_ms(1);

		for (col = 0; col < 4; col++)
		{
			if (GPIO_ReadInputDataBit(GPIOB, col_pins[col]) == 0)
			{
				delay_ms(20);
				if (GPIO_ReadInputDataBit(GPIOB, col_pins[col]) == 0)
				{
					while (GPIO_ReadInputDataBit(GPIOB, col_pins[col]) == 0);
					return key_map[row][col];
				}
			}
		}
	}
	return 0;
}

// 示例：根据按键编号处理
void Key_Get(void)
{
	uint8_t key = Key_Scan();
	if (key)
	{
		if (key >= 1 && key <= 9) // 数字1~9
		{
			if (!inputting_decimal) {
				input_integer = input_integer * 10 + key;
				if (input_integer > 99) input_integer = 99; // 最大99
			} else if (inputting_decimal && decimal_pos == 0) {
				input_decimal = key;
				decimal_pos = 1; // 只支持一位小数
			}
			inputting = 1;
		}
		else if (key == 12) // 数字0
		{
			if (!inputting_decimal) {
				input_integer = input_integer * 10;
				if (input_integer > 20) input_integer = 20;
			} else if (inputting_decimal && decimal_pos == 0) {
				input_decimal = 0;
				decimal_pos = 1;
			}
			inputting = 1;
		}
		else if (key == 13) // 小数点
		{
			if (!inputting_decimal) {
				inputting_decimal = 1;
				decimal_pos = 0;
			}
			inputting = 1;
		}
		else if (key == 10) // 确认
		{
			target_voltage = input_integer + input_decimal * 0.1f;
			inputting = 0;
			inputting_decimal = 0;
			decimal_pos = 0;
			input_integer = 0;
			input_decimal = 0;
			startflag=1;
		}
		else if (key == 11) // 清零
		{
			input_integer = 0;
			input_decimal = 0;
			inputting = 0;
			inputting_decimal = 0;
			decimal_pos = 0;
			target_voltage = 0;
		}
	}
}
             



