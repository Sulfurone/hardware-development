#include "stm32f10x.h"
// 只保留最基本的头文件

/**
  * @brief  主函数
  */
int main(void)
{
	// 使用硬件寄存器直接访问，确保在任何条件下都能工作
	// 启用GPIOC时钟
	*(volatile uint32_t *)(0x40021018) |= (1 << 4);
	
	// 配置PC13为输出 (CRH寄存器，配置为推挽输出，2MHz)
	*(volatile uint32_t *)(0x40011004) &= ~(0xF << 20);  // 清除配置位
	*(volatile uint32_t *)(0x40011004) |= (0x2 << 20);   // 设置输出模式
	
	// 简单的无限循环
	while(1)
	{
		// LED亮 (写BRR寄存器)
		*(volatile uint32_t *)(0x40011014) = (1 << 13);
		
		// 简单延时
		for(volatile uint32_t i = 0; i < 100000; i++);
		
		// LED灭 (写BSRR寄存器)
		*(volatile uint32_t *)(0x40011010) = (1 << 13);
		
		// 简单延时
		for(volatile uint32_t i = 0; i < 100000; i++);
	}
}
