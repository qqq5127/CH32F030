#include "ddl.h"
#include "dmac.h"
#include "adc.h"
#include "gpio.h"
#include "bgr.h"
#include "flash.h"
#include "bt.h"
#include "spi.h"
#include "uart.h"
#include "user_driver_key.h"
#include "user_app_led.h"

void user_driver_key_init(void)
{
	stc_gpio_config_t stcGpioCfg;
	
	DDL_ZERO_STRUCT(stcGpioCfg);

	Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);//模块时钟使能
	stcGpioCfg.enDir = GpioDirIn;
	stcGpioCfg.enPuPd = GpioPu;
	Gpio_Init(GpioPortA,GpioPin9,&stcGpioCfg);
	stcGpioCfg.enDir = GpioDirIn;
	stcGpioCfg.enPuPd = GpioPu;
	Gpio_Init(GpioPortA,GpioPin10,&stcGpioCfg);
}


void user_driver_key_sacn(void)
{
	static uint8_t porta_9_press_cnt = 0,porta_10_press_cnt = 0;
	
	if(Gpio_GetInputIO(GpioPortA,GpioPin9) == FALSE)
	{
		porta_9_press_cnt++;
	}
	else
	{
		if(porta_9_press_cnt >= USER_KEY_DRIVER_SHOT_CNT)
		{
			user_app_led_colour_change();
		}
		porta_9_press_cnt = 0; 
	}

	if(Gpio_GetInputIO(GpioPortA,GpioPin10) == FALSE)
	{
		porta_10_press_cnt++;
	}
	else
	{
		if(porta_10_press_cnt >= USER_KEY_DRIVER_SHOT_CNT)
		{
			user_app_led_mode_change();
		}
		porta_10_press_cnt = 0; 
	}
}

