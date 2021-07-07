#include "ddl.h"
#include "dmac.h"
#include "adc.h"
#include "gpio.h"
#include "bgr.h"
#include "flash.h"
#include "bt.h"
#include "spi.h"
#include "user_driver_timer.h"


#define	GPIO_TEST_TIMER	0

boolean_t timer_10ms_flag;

/*******************************************************************************
 * BT0�жϷ����� 10ms
 ******************************************************************************/
void Tim0Int(void)
{
#if	GPIO_TEST_TIMER
    static uint8_t i;
#endif    
    if(TRUE == Bt_GetIntFlag(TIM0, BtUevIrq))
    {
#if	GPIO_TEST_TIMER
        if(i == 0)
        {
            Gpio_WriteOutputIO(GpioPortB, GpioPin5, TRUE);
            
            i = 1;
        }
        else
        {
            Gpio_WriteOutputIO(GpioPortB, GpioPin5, FALSE);
            
            i = 0;
        }
#endif    
				timer_10ms_flag = 1;
        Bt_ClearIntFlag(TIM0,BtUevIrq);
    }
}


void user_driver_timer_init(void)
{
	uint16_t									u16ArrValue;
	uint16_t									u16CntValue;
	stc_bt_mode0_config_t 		stcBtBaseCfg;
#if	GPIO_TEST_TIMER
	stc_gpio_config_t 				stcLEDPortCfg;

	DDL_ZERO_STRUCT(stcLEDPortCfg);
	
	Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //GPIO ����ʱ��ʹ��

	stcLEDPortCfg.enDir  = GpioDirOut;
	Gpio_Init(GpioPortB, GpioPin5, &stcLEDPortCfg);

#endif

	DDL_ZERO_STRUCT(stcBtBaseCfg);

	Sysctrl_SetPeripheralGate(SysctrlPeripheralBTim, TRUE); //Base Timer����ʱ��ʹ��

	stcBtBaseCfg.enWorkMode = BtWorkMode0;                  //��ʱ��ģʽ
	stcBtBaseCfg.enCT       = BtTimer;                      //��ʱ�����ܣ�����ʱ��Ϊ�ڲ�PCLK
	stcBtBaseCfg.enPRS      = BtPCLKDiv64;                  //PCLK/64
	stcBtBaseCfg.enCntMode  = Bt16bitArrMode;               //�Զ�����16λ������/��ʱ��
	stcBtBaseCfg.bEnTog     = FALSE;
	stcBtBaseCfg.bEnGate    = FALSE;
	stcBtBaseCfg.enGateP    = BtGatePositive;

	stcBtBaseCfg.pfnTim0Cb  = Tim0Int;                      //�жϺ������

	Bt_Mode0_Init(TIM0, &stcBtBaseCfg); 										//TIM0 ��ģʽ0���ܳ�ʼ��
	
	u16ArrValue = 0xFD80;	//10ms
	
	Bt_M0_ARRSet(TIM0, u16ArrValue);												//��������ֵ(���� = 0x10000 - ARR)
	
	u16CntValue = 0xFD80;	//10ms
	
	Bt_M0_Cnt16Set(TIM0, u16CntValue);											//���ü�����ֵ
	
	Bt_ClearIntFlag(TIM0,BtUevIrq); 												//���жϱ�־ 	
	Bt_Mode0_EnableIrq(TIM0); 															//ʹ��TIM0�ж�(ģʽ0ʱֻ��һ���ж�)
	EnableNvic(TIM0_IRQn, IrqLevel3, TRUE); 								//TIM0�ж�ʹ��
	
	Bt_M0_Run(TIM0);																				//TIM0 ���С�
	
}

