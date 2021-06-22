#include "ddl.h"
#include "dmac.h"
#include "adc.h"
#include "gpio.h"
#include "bgr.h"
#include "flash.h"
#include "bt.h"
#include "spi.h"
#include "user_driver_pwm_dma.h"

/******************************************************************************
* Local variable definitions ('static')                                      *
******************************************************************************/
//static uint8_t* io_point = NULL;	// cuixu test
static uint8_t  pwm_led_buff[40][3][8];
boolean_t bSendIrq,bRecvIrq;


static void Spi0CallBack(void)
{
    
    if(Spi_GetState(Spi0)&0x38)
    {
        Spi_ClearStatus(Spi0);
        return;
    }
		if(Spi_GetStatus(Spi0,SpiTxe))
    {
      bSendIrq = 1;
      Spi_FuncEn(Spi0,SpiTxEIe,FALSE);
      Spi_ClearStatus(Spi0);
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
    Gpio_Init(GpioPortA, GpioPin15,&stcGpioCfg);//CS
    Gpio_SetAfMode(GpioPortA, GpioPin15,GpioAf1);
    Gpio_Init(GpioPortB, GpioPin3,&stcGpioCfg);//SCK
    Gpio_SetAfMode(GpioPortB, GpioPin3,GpioAf1);
    stcGpioCfg.enDir = GpioDirIn;
    Gpio_Init(GpioPortA, GpioPin11,&stcGpioCfg);//MISO
    Gpio_SetAfMode(GpioPortA, GpioPin11,GpioAf6);
}


void user_driver_pwm_dma_init(void)
{

	en_flash_waitcycle_t          enFlashWait;
	//stc_dma_config_t           stcDmaCfg;    
  stc_sysctrl_clk_config_t      stcClkConfig;
	stc_gpio_config_t 				stcLEDPortCfg;
	stc_spi_config_t  SPIConfig;		
	stc_dma_config_t           stcDmaCfg;
  //DDL_ZERO_STRUCT(stcDmaCfg);   
  DDL_ZERO_STRUCT(stcClkConfig);
	DDL_ZERO_STRUCT(stcLEDPortCfg);
	DDL_ZERO_STRUCT(pwm_led_buff);
	DDL_ZERO_STRUCT(SPIConfig);
	DDL_ZERO_STRUCT(stcDmaCfg); 



	stcClkConfig.enClkSrc 	 = SysctrlClkXTH; 					//使用外部高速晶振,32M
	stcClkConfig.enHClkDiv	 = SysctrlHclkDiv1; 				// HCLK = SystemClk/1
	stcClkConfig.enPClkDiv	 = SysctrlPclkDiv1; 				// PCLK = HCLK/1
	Sysctrl_SetXTHFreq(SysctrlXthFreq20_32MHz); 				//设置外部高速频率为20~32M
	Sysctrl_ClkInit(&stcClkConfig);	
	
	enFlashWait = FlashWaitCycle1;                      //读等待周期设置为1（当HCLK大于24MHz时必须至少为1）
	Flash_WaitCycle(enFlashWait);                       // Flash 等待1个周期
	
	Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //GPIO 外设时钟使能
	Sysctrl_SetPeripheralGate(SysctrlPeripheralSpi0,TRUE); //SPI0

	{
		uint8_t i,j,k;
		
		for(i=0;i<40;i++)
		{
			for(j=0;j<3;j++)
				for(k=0;k<8;k++)
					{
						pwm_led_buff[i][j][k]=0xaa;
					}
		}

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
	Spi_SetCS(Spi0,TRUE);
	//SPI模块配置
	SPIConfig.bCPHA = Spicphafirst;//模式0
	SPIConfig.bCPOL = Spicpollow;
	SPIConfig.bIrqEn = TRUE;
	SPIConfig.bMasterMode = SpiMaster;
	SPIConfig.u8BaudRate = SpiClkDiv8;
	SPIConfig.pfnSpi0IrqCb = Spi0CallBack;
	if(SPIConfig.bIrqEn)
	{
		EnableNvic(SPI0_IRQn,IrqLevel3,TRUE);
	}
	Spi_Init(Spi0,&SPIConfig);//模块初始化

	Spi_SetCS(Spi0,FALSE);//使能片选信号
	//while(1)


  //使能DMA触发
  Adc_ConfigDmaTrig(DmaSqr);
  
  stcDmaCfg.enMode =  DmaBurst;                   
  stcDmaCfg.u16BlockSize = 0x01u;
  stcDmaCfg.u16TransferCnt = 40*3*8;         //burst模式，一次触发传输数据大小为 3*1,
  stcDmaCfg.enTransferWidth = Dma8Bit;            
  stcDmaCfg.enSrcAddrMode = AddressFix;
  stcDmaCfg.enDstAddrMode = AddressIncrease;
  stcDmaCfg.bDestAddrReloadCtl = FALSE;
  stcDmaCfg.bSrcAddrReloadCtl = FALSE;
  stcDmaCfg.bSrcBcTcReloadCtl = FALSE;
  stcDmaCfg.u32SrcAddress = (uint32_t) &(M0P_SPI0->DATA);
  stcDmaCfg.u32DstAddress = (uint32_t)&pwm_led_buff;
  stcDmaCfg.enRequestNum = SPI0TXTrig;	        //设置为ADCSQR触发
  Dma_InitChannel(DmaCh0,&stcDmaCfg);	
  
  //使能DMA，使能DMA0，启动DMA0
  Dma_Enable();
  Dma_EnableChannel(DmaCh0);
  Dma_Start(DmaCh0);		
  //Adc_SQR_Start();
	{
		Spi_SendData(Spi0,0xAA);//发送数据
	}

  //等待传输完成
  //while(Dma_GetStat(DmaCh0) != DmaTransferComplete);
	while(1)
	{
		Spi_SendData(Spi0,0xAA);//发送数据
	}
}
