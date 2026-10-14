#include"bsp_spi.h"



/*SPI1*/
void SPI_Configuration(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOB | RCC_APB2Periph_SPI1, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD, ENABLE);

  GPIO_InitStructure.GPIO_Pin = PCAP_SCK_PIN | PCAP_MOSI_PIN |PCAP_SSN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(PCAP_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PCAP_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(PCAP_PORT, &GPIO_InitStructure);
	
}



