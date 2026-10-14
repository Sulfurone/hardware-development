#include "flash.h"


#define FLASH_SAVE_ADDR 0x8007000

static FLASH_EraseInitTypeDef EraseInitTypeDef = {
    .TypeErase = FLASH_TYPEERASE_PAGES,
    .PageAddress = FLASH_SAVE_ADDR,
    .NbPages = 1
};
void Flash_Mid_Offset(float offset)
{
    HAL_FLASH_Unlock();
    uint32_t page_error = 0;
    __disable_irq();

    if (HAL_FLASHEx_Erase(&EraseInitTypeDef, &page_error) == HAL_OK)
    {
        Log("Flash Erase OK\r\n");
    }

    __enable_irq();
    uint32_t value=(uint32_t)(offset*1000);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_SAVE_ADDR, (uint32_t)value);

    HAL_FLASH_Unlock();
}

float Get_Mid_Offset(void)
{
    return ((float)  *(__IO uint32_t*)( FLASH_SAVE_ADDR ))/1000; //*(__IO uint16_t *)是读取该地址的参数值,其值为16位数据,一次读取两个字节，*(__IO uint32_t *)就一次读4个字节

}