#include "ddl.h"
#include "dmac.h"
#include "adc.h"
#include "gpio.h"
#include "bgr.h"
#include "flash.h"
#include "bt.h"
#include "spi.h"
#include "uart.h"
#include "user_driver_flash.h"

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
static volatile uint32_t u32FlashTestFlag   = 0;

/*******************************************************************************
 * FLASH 中断服务函数
 ******************************************************************************/
 
 static void FlashInt(void)
 {
    if (TRUE == Flash_GetIntFlag(FlashPCInt))
    {
        Flash_ClearIntFlag(FlashPCInt);
        u32FlashTestFlag |= 0x01;
        Flash_DisableIrq(FlashPCInt);
    }
    if (TRUE == Flash_GetIntFlag(FlashSlockInt))
    {
        Flash_ClearIntFlag(FlashSlockInt);
        u32FlashTestFlag |= 0x02;
        Flash_DisableIrq(FlashSlockInt);
    }
      
}

static void user_driver_refresh_data(uint8_t mode,uint8_t colour)
{
	 ///< FLASH目标扇区擦除
	 while(Ok != Flash_SectorErase(USER_DRIVER_SECTOR_ADDR))
	 {
			 ;
	 }
	 
	 if (Ok == Flash_WriteByte(USER_DRIVER_COLOUR_VALUE, colour))
	 {
			 while(*((volatile uint8_t*)USER_DRIVER_COLOUR_VALUE) != colour);
	 }
	 if (Ok == Flash_WriteByte(USER_DRIVER_MODE_VALUE, mode))
	 {
			 while(*((volatile uint8_t*)USER_DRIVER_MODE_VALUE) != mode);
	 }

}

void user_driver_flash_write_mode(uint8_t value)
{
	uint8_t mode_temp,colour_temp;

	colour_temp = (*((volatile uint8_t*)USER_DRIVER_COLOUR_VALUE));
	mode_temp = value;
	
	user_driver_refresh_data(mode_temp,colour_temp);

}

void user_driver_flash_write_colour(uint8_t value)
{
	uint8_t mode_temp,colour_temp;

	mode_temp = (*((volatile uint8_t*)USER_DRIVER_MODE_VALUE));
	colour_temp = value;
	
	user_driver_refresh_data(mode_temp,colour_temp);
}

uint8_t user_driver_flash_get_mode(void)
{
	return (*((volatile uint8_t*)USER_DRIVER_MODE_VALUE));
}

uint8_t user_driver_flash_get_colour(void)
{
	return (*((volatile uint8_t*)USER_DRIVER_COLOUR_VALUE));
}


void user_driver_flash_init(void)
{
	 ///< 确保初始化正确执行后方能进行FLASH编程操作，FLASH初始化（中断函数,编程时间,休眠模式配置）
	 while(Ok != Flash_Init(FlashInt, 8, TRUE))	//32M system clk
	 {
			 ;
	 }
}

