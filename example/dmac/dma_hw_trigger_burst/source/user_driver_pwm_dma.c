#include "ddl.h"
#include "dmac.h"
#include "adc.h"
#include "gpio.h"
#include "bgr.h"
#include "user_driver_pwm_dma.h"

/******************************************************************************
* Local variable definitions ('static')                                      *
******************************************************************************/
static uint32_t ADC_Result_Array[16] = {0};


/*******************************************************************************
 * BT2�жϷ�����
 ******************************************************************************/
void Tim0Int(void)
{
    
}


void user_driver_pwm_dma_init(void)
{
	uint16_t											u16ArrValue;
	uint16_t											u16CompareAValue;
	uint16_t											u16CntValue;


	//en_flash_waitcycle_t          enFlashWait;
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



	//enFlashWait = FlashWaitCycle1;                      //���ȴ���������Ϊ1����HCLK����24MHzʱ��������Ϊ1��
	//Flash_WaitCycle(enFlashWait);                       // Flash �ȴ�1������
  // 

	stcClkConfig.enClkSrc 	 = SysctrlClkXTH; 					//ʹ���ⲿ���پ���,32M
	stcClkConfig.enHClkDiv	 = SysctrlHclkDiv1; 				// HCLK = SystemClk/1
	stcClkConfig.enPClkDiv	 = SysctrlPclkDiv1; 				// PCLK = HCLK/1
	Sysctrl_SetXTHFreq(SysctrlXthFreq20_32MHz); 				//�����ⲿ����Ƶ��Ϊ20~32M
	Sysctrl_ClkInit(&stcClkConfig);
	
  Sysctrl_SetPeripheralGate(SysctrlPeripheralDma,TRUE);
  Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE); 
  Sysctrl_SetPeripheralGate(SysctrlPeripheralBTim, TRUE);   //Base Timer����ʱ��ʹ��

	stcTIM0Port.enDir  = GpioDirOut;
	
	Gpio_Init(GpioPortA, GpioPin5, &stcTIM0Port);
	Gpio_SetAfMode(GpioPortA,GpioPin5,GpioAf4); 					 //PA00����ΪTIM0_CHA

	stcBtBaseCfg.enWorkMode 	 = BtWorkMode3; 						 //���ǲ�ģʽ
	stcBtBaseCfg.enCT 				 = BtTimer; 								 //��ʱ�����ܣ�����ʱ��Ϊ�ڲ�PCLK
	stcBtBaseCfg.enPRS				 = BtPCLKDiv1;							 //PCLK
	//stcBtBaseCfg.enCntDir 	 = BtCntUp; 								 //���ϼ����������ǲ�ģʽʱֻ��
	stcBtBaseCfg.enPWMTypeSel  = BtComplementaryPWM;			 //�������PWM
	stcBtBaseCfg.enPWM2sSel 	 = BtSinglePointCmp;				 //����ȽϹ���
	stcBtBaseCfg.bOneShot 		 = FALSE; 									 //ѭ������
	stcBtBaseCfg.bURSSel			 = FALSE; 									 //���������
	
	stcBtBaseCfg.pfnTim0Cb	= Tim0Int;										 //�жϺ������
	
	Bt_Mode23_Init(TIM0, &stcBtBaseCfg);									 //TIM0 ��ģʽ0���ܳ�ʼ��

  u16ArrValue = 0x9000;
  Bt_M23_ARRSet(TIM0, u16ArrValue, TRUE);                //��������ֵ,��ʹ�ܻ���
  
  u16CompareAValue = 0x6000;
  Bt_M23_CCR_Set(TIM0, BtCCR0A, u16CompareAValue);       //���ñȽ�ֵA,(PWM����ģʽ��ֻ��Ҫ���ñȽ�ֵA)

	stcBtPortCmpCfg.enCH0ACmpCtrl   = BtPWMMode2;          //OCREFA�������OCMA:PWMģʽ2
	stcBtPortCmpCfg.enCH0APolarity  = BtPortPositive;      //�������
	stcBtPortCmpCfg.bCh0ACmpBufEn   = TRUE;                //Aͨ���������
	stcBtPortCmpCfg.enCh0ACmpIntSel = BtCmpIntNone;        //Aͨ���ȽϿ���:��	

	Bt_M23_PortOutput_Config(TIM0, &stcBtPortCmpCfg); 		 //�Ƚ�����˿�����

	u16CntValue = 0;

	Bt_M23_Cnt16Set(TIM0, u16CntValue); 									 //���ü�����ֵ

	Bt_ClearAllIntFlag(TIM0); 														 //���жϱ�־
	Bt_Mode23_EnableIrq(TIM0,BtUevIrq); 									 //ʹ��TIM0 UEV�����ж�
	EnableNvic(TIM0_IRQn, IrqLevel0, TRUE); 							 //TIM0�ж�ʹ�� 	

	Bt_M23_EnPWM_Output(TIM0, TRUE, FALSE); 							 //TIM0 �˿����ʹ��

	Bt_M23_Run(TIM0); 																		 //TIM0 ���С�


  if (Ok != Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE))
  {
    return;
  }	
  
  Gpio_SetAnalogMode(GpioPortA, GpioPin0);        //PA00 (AIN0)
  Gpio_SetAnalogMode(GpioPortA, GpioPin2);        //PA02 (AIN2)
  Gpio_SetAnalogMode(GpioPortA, GpioPin5);        //PA05 (AIN5)

    //
  //Adc_ConfigDmaTrig(DmaSqr);
  
  stcDmaCfg.enMode =  DmaBurst;                   
  stcDmaCfg.u16BlockSize = 0x01u;
  stcDmaCfg.u16TransferCnt = 0x03u;         //
  stcDmaCfg.enTransferWidth = Dma32Bit;            
  stcDmaCfg.enSrcAddrMode = AddressIncrease;
  stcDmaCfg.enDstAddrMode = AddressIncrease;
  stcDmaCfg.bDestAddrReloadCtl = FALSE;
  stcDmaCfg.bSrcAddrReloadCtl = FALSE;
  stcDmaCfg.bSrcBcTcReloadCtl = FALSE;
  stcDmaCfg.u32SrcAddress = (uint32_t) &(M0P_ADC->SQRRESULT0);
  stcDmaCfg.u32DstAddress = (uint32_t)&ADC_Result_Array[0];
  stcDmaCfg.enRequestNum = ADCSQRTrig;	        //
  Dma_InitChannel(DmaCh0,&stcDmaCfg);	
  
  //
  Dma_Enable();
  Dma_EnableChannel(DmaCh0);
  //Dma_Start(DmaCh0);		
  Adc_SQR_Start();

  //
  while(Dma_GetStat(DmaCh0) != DmaTransferComplete);
}
