#include "ddl.h"
#include "dmac.h"
#include "adc.h"
#include "gpio.h"
#include "bgr.h"
#include "flash.h"
#include "bt.h"
#include "spi.h"
#include "user_driver_adc.h"

/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/
static stc_adc_irq_t stcAdcIrqFlag;
uint16_t u16AdcRestult0;


/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

void AdcContIrqCallback(void)
{    
    Adc_GetSglResult(&u16AdcRestult0);
  
    stcAdcIrqFlag.bAdcIrq = TRUE;
}


uint16_t user_driver_adc_sgl_start(void)
{
	stcAdcIrqFlag.bAdcIrq = FALSE;
	Adc_SGL_Start();
	while(FALSE == stcAdcIrqFlag.bAdcIrq);
	return u16AdcRestult0;
}

void user_driver_adc_init(void)
{
	//uint32_t									 u32AdcResultAcc;
	stc_adc_cfg_t 						 stcAdcCfg;
	stc_adc_irq_t 						 stcAdcIrq;
	stc_adc_irq_calbakfn_pt_t  stcAdcIrqCalbaks;
	stc_gpio_config_t 				 stcAdcAN0Port;
	stc_gpio_config_t 				 stcAdcAN2Port;
	stc_gpio_config_t 				 stcAdcAN5Port;
	
	
	DDL_ZERO_STRUCT(stcAdcCfg);
	DDL_ZERO_STRUCT(stcAdcIrq);
	DDL_ZERO_STRUCT(stcAdcIrqCalbaks);
	DDL_ZERO_STRUCT(stcAdcIrqFlag);
	DDL_ZERO_STRUCT(stcAdcAN0Port);
	DDL_ZERO_STRUCT(stcAdcAN2Port);
	DDL_ZERO_STRUCT(stcAdcAN5Port);
	
			
	Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
	
	Gpio_SetAnalogMode(GpioPortA, GpioPin2);				//PA02 (AIN0)
	
	if (Ok != Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE))
	{
			return;
	} 	 
	
	//ADC配置
	Adc_Enable();
	M0P_BGR->CR_f.BGR_EN = 0x1u;								 //BGR必须使能
	M0P_BGR->CR_f.TS_EN = 0x0u;
	delay100us(1);
	
	stcAdcCfg.enAdcOpMode = AdcSglMode; 				//单次转换模式
	stcAdcCfg.enAdcClkDiv = AdcClkSysTDiv1;
	stcAdcCfg.enAdcSampTimeSel = AdcSampTime8Clk;
	stcAdcCfg.enAdcRefVolSel	 = RefVolSelAVDD;
	stcAdcCfg.bAdcInBufEn 		 = FALSE;
	
	Adc_Init(&stcAdcCfg);
	
	Adc_ConfigSglMode(&stcAdcCfg);
	
	Adc_ConfigSglChannel(AdcExInputCH2);
	
	EnableNvic(ADC_IRQn, IrqLevel3, TRUE);
	
	Adc_EnableIrq();
	
	stcAdcIrq.bAdcIrq = TRUE;
	stcAdcIrqCalbaks.pfnAdcIrq = AdcContIrqCallback;
	
	Adc_ConfigIrq(&stcAdcIrq, &stcAdcIrqCalbaks);

	Adc_SGL_Start();
}

