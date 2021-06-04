#include "ddl.h"
#include "dmac.h"
#include "adc.h"
#include "gpio.h"
#include "bgr.h"
#include "flash.h"
#include "bt.h"
#include "user_driver_pwm_dma.h"

/******************************************************************************
* Local variable definitions ('static')                                      *
******************************************************************************/
static uint32_t PWM_DMA_Array[] = {4000,6000,4000,6000,4000,6000,4000,6000,4000,6000,4000,6000,4000,6000,4000,6000,4000,6000,4000,6000,4000,6000,4000,6000,4000,6000,4000,6000,4000,6000,4000,6000,4000,6000,4000,6000,4000,6000,4000,6000,4000,6000};


/*******************************************************************************
 * BT2中断服务函数
 ******************************************************************************/
void Tim0Int(void)
{
	if(TRUE == Bt_GetIntFlag(TIM0, BtUevIrq))
	{			
			Bt_ClearIntFlag(TIM0,BtUevIrq);
	}
}


void user_driver_pwm_dma_init(void)
{
	uint16_t											u16ArrValue;
	uint16_t											u16CompareAValue;
	uint16_t											u16CntValue;
	uint8_t 											u8ValidPeriod;


	en_flash_waitcycle_t          enFlashWait;
	stc_dma_config_t           stcDmaCfg;    
  stc_sysctrl_clk_config_t      stcClkConfig;
  stc_gpio_config_t             stcTIM0Port;
	stc_bt_mode23_config_t				stcBtBaseCfg;
	stc_bt_m23_compare_config_t 	stcBtPortCmpCfg;

	
  DDL_ZERO_STRUCT(stcDmaCfg);   
  DDL_ZERO_STRUCT(stcClkConfig);
	DDL_ZERO_STRUCT(stcTIM0Port);
	DDL_ZERO_STRUCT(stcBtBaseCfg);
	DDL_ZERO_STRUCT(stcBtPortCmpCfg);



	enFlashWait = FlashWaitCycle1;                      //读等待周期设置为1（当HCLK大于24MHz时必须至少为1）
	Flash_WaitCycle(enFlashWait);                       // Flash 等待1个周期
  // 

	stcClkConfig.enClkSrc 	 = SysctrlClkXTH; 					//使用外部高速晶振,32M
	stcClkConfig.enHClkDiv	 = SysctrlHclkDiv1; 				// HCLK = SystemClk/1
	stcClkConfig.enPClkDiv	 = SysctrlPclkDiv1; 				// PCLK = HCLK/1
	Sysctrl_SetXTHFreq(SysctrlXthFreq20_32MHz); 				//设置外部高速频率为20~32M
	Sysctrl_ClkInit(&stcClkConfig);
	
  Sysctrl_SetPeripheralGate(SysctrlPeripheralDma,TRUE);
  Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE); 
  Sysctrl_SetPeripheralGate(SysctrlPeripheralBTim, TRUE);   //Base Timer外设时钟使能

	stcTIM0Port.enDir  = GpioDirOut;
	
	Gpio_Init(GpioPortA, GpioPin5, &stcTIM0Port);
	Gpio_SetAfMode(GpioPortA,GpioPin5,GpioAf4); 					 //PA05设置为TIM0_CHA

	stcBtBaseCfg.enWorkMode 	 = BtWorkMode2; 						 //三角波模式
	stcBtBaseCfg.enCT 				 = BtTimer; 								 //定时器功能，计数时钟为内部PCLK
	stcBtBaseCfg.enPRS				 = BtPCLKDiv1;							 //PCLK
	stcBtBaseCfg.enCntDir 	   = BtCntUp; 								 //向上计数，在三角波模式时只读
	stcBtBaseCfg.enPWMTypeSel  = BtIndependentPWM;			 //独立输出PWM
	stcBtBaseCfg.enPWM2sSel 	 = BtSinglePointCmp;				 //单点比较功能
	stcBtBaseCfg.bOneShot 		 = FALSE; 									 //循环计数
	stcBtBaseCfg.bURSSel			 = FALSE; 									 //上下溢更新
	
	stcBtBaseCfg.pfnTim0Cb	= Tim0Int;										 //中断函数入口
	
	Bt_Mode23_Init(TIM0, &stcBtBaseCfg);									 //TIM0 的模式0功能初始化

  u16ArrValue = 0x9000;
  Bt_M23_ARRSet(TIM0, u16ArrValue, TRUE);                //设置重载值,并使能缓存
  
  u16CompareAValue = 0x6000;
  Bt_M23_CCR_Set(TIM0, BtCCR0A, u16CompareAValue);       //设置比较值A,(PWM互补模式下只需要设置比较值A)

	stcBtPortCmpCfg.enCH0ACmpCtrl   = BtPWMMode2;          //OCREFA输出控制OCMA:PWM模式2
	stcBtPortCmpCfg.enCH0APolarity  = BtPortPositive;      //正常输出
	stcBtPortCmpCfg.bCh0ACmpBufEn   = TRUE;                //A通道缓存控制
	stcBtPortCmpCfg.enCh0ACmpIntSel = BtCmpIntNone;        //A通道比较控制:无	

	Bt_M23_PortOutput_Config(TIM0, &stcBtPortCmpCfg); 		 //比较输出端口配置

	u8ValidPeriod = 0;                                     //事件更新周期设置，0表示锯齿波每个周期更新一次，每+1代表延迟1个周期
	Bt_M23_SetValidPeriod(TIM0,u8ValidPeriod);             //间隔周期设置
	u16CntValue = 0;
	Bt_M23_Cnt16Set(TIM0, u16CntValue); 									 //设置计数初值

  
  stcDmaCfg.enMode =  DmaBurst;                   
  stcDmaCfg.u16BlockSize = 0x01u;
  stcDmaCfg.u16TransferCnt = 0x03u;         //
  stcDmaCfg.enTransferWidth = Dma32Bit;            
  stcDmaCfg.enSrcAddrMode = AddressIncrease;
  stcDmaCfg.enDstAddrMode = AddressIncrease;
  stcDmaCfg.bDestAddrReloadCtl = FALSE;
  stcDmaCfg.bSrcAddrReloadCtl = FALSE;
  stcDmaCfg.bSrcBcTcReloadCtl = FALSE;
  stcDmaCfg.u32SrcAddress = (uint32_t) &(M0P_TIM0_MODE23->CCR0A);
  stcDmaCfg.u32DstAddress = (uint32_t)&PWM_DMA_Array[0];
  stcDmaCfg.enRequestNum = TIM0ATrig;	        //
  Dma_InitChannel(DmaCh0,&stcDmaCfg);	
  
  //
  Dma_Enable();
  Dma_EnableChannel(DmaCh0);

	Bt_ClearAllIntFlag(TIM0); 														 //清中断标志
	Bt_Mode23_EnableIrq(TIM0,BtUevIrq); 									 //使能TIM0 UEV更新中断
	EnableNvic(TIM0_IRQn, IrqLevel0, TRUE); 							 //TIM0中断使能 	

	Bt_M23_EnPWM_Output(TIM0, TRUE, FALSE); 							 //TIM0 端口输出使能

	Bt_M23_Run(TIM0); 																		 //TIM0 运行。

  //
  while(Dma_GetStat(DmaCh0) != DmaTransferComplete);
}
