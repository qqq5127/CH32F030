#include "ddl.h"
#include "dmac.h"
#include "adc.h"
#include "gpio.h"
#include "bgr.h"
#include "flash.h"
#include "bt.h"
#include "spi.h"
#include "user_driver_pwm_dma.h"


#define	SPI_DMA_CONFIG		1
/******************************************************************************
* Local variable definitions ('static')                                      *
******************************************************************************/
//static uint8_t* io_point = NULL;	// cuixu test
static uint32_t  pwm_led_buff[40][24];
boolean_t bSendIrq;


static void Spi0CallBack(void)
{
    
    if(Spi_GetState(Spi0)&0x38)
    {
        Spi_ClearStatus(Spi0);
        return;
    }
    if(Spi_GetStatus(Spi0,SpiRxne))
    {
      Spi_FuncEn(Spi0,SpiRxNeIe,FALSE);
      Spi_ClearStatus(Spi0);
    }
		else if(Spi_GetStatus(Spi0,SpiTxe))
    {
      bSendIrq = 1;
			//Spi_FuncEn(Spi0,SpiTxEIe,FALSE);
			//Spi_ClearStatus(Spi0);
    }
}

/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** This sample
 **
 ******************************************************************************/
void Spi_PortInit(void)
{
    stc_gpio_config_t stcGpioCfg;
    DDL_ZERO_STRUCT(stcGpioCfg);
    
    stcGpioCfg.enDir = GpioDirOut;   
    Gpio_Init(GpioPortB, GpioPin5,&stcGpioCfg);//MOSI
    Gpio_SetAfMode(GpioPortB, GpioPin5,GpioAf1);   
    Gpio_Init(GpioPortA, GpioPin4,&stcGpioCfg);//CS
    Gpio_SetAfMode(GpioPortA, GpioPin4,GpioAf1);
    Gpio_Init(GpioPortB, GpioPin3,&stcGpioCfg);//SCK
    Gpio_SetAfMode(GpioPortB, GpioPin3,GpioAf1);
    stcGpioCfg.enDir = GpioDirIn;
    stcGpioCfg.enPuPd = GpioPd;
    Gpio_Init(GpioPortA, GpioPin11,&stcGpioCfg);//MISO
    Gpio_SetAfMode(GpioPortA, GpioPin11,GpioAf6);
}


void user_driver_pwm_dma_init(void)
{

	en_flash_waitcycle_t          enFlashWait;
  stc_sysctrl_clk_config_t      stcClkConfig;
	stc_gpio_config_t 				stcLEDPortCfg;
	stc_spi_config_t  SPIConfig;		

#if SPI_DMA_CONFIG	
	stc_dma_config_t           stcDmaCfg;
#endif

  DDL_ZERO_STRUCT(stcClkConfig);
	DDL_ZERO_STRUCT(stcLEDPortCfg);
	DDL_ZERO_STRUCT(pwm_led_buff);
	DDL_ZERO_STRUCT(SPIConfig);
#if SPI_DMA_CONFIG	
	DDL_ZERO_STRUCT(stcDmaCfg); 
#endif


	stcClkConfig.enClkSrc 	 = SysctrlClkXTH; 					//使用外部高速晶振,32M
	stcClkConfig.enHClkDiv	 = SysctrlHclkDiv1; 				// HCLK = SystemClk/1
	stcClkConfig.enPClkDiv	 = SysctrlPclkDiv1; 				// PCLK = HCLK/1
	Sysctrl_SetXTHFreq(SysctrlXthFreq20_32MHz); 				//设置外部高速频率为20~32M
	Sysctrl_ClkInit(&stcClkConfig);	
	
	enFlashWait = FlashWaitCycle1;                      //读等待周期设置为1（当HCLK大于24MHz时必须至少为1）
	Flash_WaitCycle(enFlashWait);                       // Flash 等待1个周期
	
	Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //GPIO 外设时钟使能
	Sysctrl_SetPeripheralGate(SysctrlPeripheralSpi0,TRUE); //SPI0
	Sysctrl_SetPeripheralGate(SysctrlPeripheralDma,TRUE); //SPI0

	{
		boolean_t flag = FALSE;
		for(uint8_t i=0;i<40;i++)
		{
			for(uint8_t j=0;j<24;j++)
			{
			#if 0
				if(flag)
				{
					pwm_led_buff[i][j]=0xC0;	//0
				}
				else
				{
					pwm_led_buff[i][j]=0xFC;	//1
				}
				flag = !flag;
			#else
				pwm_led_buff[i][j]=0xFC;	//0
			#endif
			}
		}
		pwm_led_buff[0][7]=0;
		pwm_led_buff[39][23]=0;
	}
#if 0
	stcLEDPortCfg.enDir  = GpioDirOut;
	Gpio_Init(GpioPortB, GpioPin5, &stcLEDPortCfg);
	io_point = (volatile uint8_t *)((uint32_t)&M0P_GPIO->PAOUT + 0x40u);

	while(1)
	{
		*(io_point) = 0x20;
		//Gpio_WriteOutputIO(GpioPortB, GpioPin5, FALSE);
		*(io_point) = 0x00;
	}
#endif

	Spi_PortInit();//模块初始化
	//SPI模块配置
	SPIConfig.bCPHA = Spicphafirst;//模式0
	SPIConfig.bCPOL = Spicpollow;
	SPIConfig.bIrqEn = FALSE;
	SPIConfig.bMasterMode = SpiMaster;
	SPIConfig.u8BaudRate = SpiClkDiv2;
	SPIConfig.pfnSpi0IrqCb = Spi0CallBack;
	Spi_Init(Spi0,&SPIConfig);//模块初始化
	Spi_SetCS(Spi0,FALSE);//使能片选信号
	
	
	Spi_FuncEn(Spi0,SpiDmaTxEn,TRUE);	
	
#if SPI_DMA_CONFIG	
  stcDmaCfg.enMode =  DmaBlock;                   
  stcDmaCfg.u16BlockSize = 0x01u;
  stcDmaCfg.u16TransferCnt = 40*24 + 1;         //burst模式，一次触发传输数据大小
  stcDmaCfg.enTransferWidth = Dma32Bit;            
  stcDmaCfg.enSrcAddrMode = AddressIncrease;
  stcDmaCfg.enDstAddrMode = AddressFix;
  stcDmaCfg.bDestAddrReloadCtl = FALSE;
  stcDmaCfg.bSrcAddrReloadCtl = TRUE;
  stcDmaCfg.bSrcBcTcReloadCtl = TRUE;
  stcDmaCfg.u32SrcAddress = (uint32_t)&pwm_led_buff[0][0];
  stcDmaCfg.u32DstAddress = (uint32_t) &(M0P_SPI0->DATA);
  stcDmaCfg.enRequestNum = SPI0TXTrig;	        //设置为SPI 触发
  Dma_InitChannel(DmaCh0,&stcDmaCfg);	
		
	//M0P_SPI0->DATA = 0xaa;
	//使能DMA，使能DMA0，启动DMA0
  Dma_Enable();
  Dma_EnableChannel(DmaCh0);
	//EnableNvic(SPI0_IRQn,IrqLevel3,TRUE);
	//M0P_SPI0->DATA = 0xaa;
  //等待传输完成
  while(Dma_GetStat(DmaCh0) != DmaTransferComplete);
#endif	
	Spi_SetCS(Spi0,TRUE);//使能片选信号

}
