#include "ddl.h"
#include "dmac.h"
#include "adc.h"
#include "gpio.h"
#include "bgr.h"
#include "flash.h"
#include "bt.h"
#include "spi.h"
#include "uart.h"
#include "user_driver_uart.h"

#define SLAVEADDR   0xc0
uint8_t u8TxData[2] = {0xaa,0x55};
uint8_t u8RxData[2] = {0x00};
uint8_t u8TxCnt=0,u8RxCnt=0;
boolean_t uart_finish = FALSE;


void TxIntCallback(void)
{
	uart_finish = TRUE;
}
void RxIntCallback(void)
{
    u8RxData[u8RxCnt]=Uart_ReceiveData(USER_UART_CH_ID);
    u8RxCnt++;
    if(u8RxCnt>1)
    {
    	u8RxCnt = 0;
    }
}
void ErrIntCallback(void)
{
  
}
void PErrIntCallBack(void)
{
}
void CtsIntCallBack(void)
{
}

void Uart_PortInit(void)
{
    stc_gpio_config_t stcGpioCfg;
    DDL_ZERO_STRUCT(stcGpioCfg);
    stcGpioCfg.enDir = GpioDirOut;
    Gpio_Init(USER_UART_TX_PORT,USER_UART_TX_PIN,&stcGpioCfg);
    Gpio_SetAfMode(USER_UART_TX_PORT,USER_UART_TX_PIN,GpioAf1);//TX PA9
    stcGpioCfg.enDir = GpioDirIn;
    Gpio_Init(GpioPortA,GpioPin10,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortA,GpioPin10,GpioAf1);//RX PA10
}

void user_driver_uart_print_char(char value)
{
	uart_finish = FALSE;
	M0P_UART0->SBUF = value;
	while(uart_finish == FALSE);
	
}

#if USER_DEBUGE_MODE
void user_driver_uart_print(const char* string,uint16_t value)
{
	uint8_t i;
	char vlaue_buff[16];

	memset(vlaue_buff,0,16);
	for(i=0;string[i] != 0;i++)
	{
		user_driver_uart_print_char(string[i]);
	}
	user_driver_uart_print_char(0x20);
	user_driver_uart_print_char(0x20);
	user_driver_uart_print_char(0x20);
	sprintf(vlaue_buff,"%d",value);
	for(i=0;vlaue_buff[i] != 0;i++)
	{
		user_driver_uart_print_char(vlaue_buff[i]);
	}
	
	user_driver_uart_print_char(0x0D);
	user_driver_uart_print_char(0x0A);
	user_driver_uart_print_char(0x0D);
	user_driver_uart_print_char(0x0A);
}

void user_driver_uart_init(void)
{
	uint16_t u16Scnt = 0;
	stc_uart_config_t  stcConfig;
	stc_uart_irq_cb_t stcUartIrqCb;
	stc_uart_multimode_t stcMulti;
	stc_uart_baud_t stcBaud;

	en_uart_mmdorck_t enTb8;

	DDL_ZERO_STRUCT(stcConfig);
	DDL_ZERO_STRUCT(stcUartIrqCb);
	DDL_ZERO_STRUCT(stcMulti);
	DDL_ZERO_STRUCT(stcBaud);

	//Sysctrl_SetFunc(SysctrlSWDUseIOEn,TRUE);	// sw 做普通io
	Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);//模块时钟使能
	Sysctrl_SetPeripheralGate(SysctrlPeripheralUart0,TRUE);
	Uart_PortInit();//端口初始化

	stcUartIrqCb.pfnRxIrqCb   = RxIntCallback;//中断服务函数指定
	stcUartIrqCb.pfnTxIrqCb   = TxIntCallback;
	stcUartIrqCb.pfnRxFEIrqCb = ErrIntCallback;
	stcUartIrqCb.pfnPEIrqCb   = PErrIntCallBack;
	stcUartIrqCb.pfnCtsIrqCb  = CtsIntCallBack;
	stcConfig.pstcIrqCb = &stcUartIrqCb;
	stcConfig.bTouchNvic = TRUE;
	if(TRUE == stcConfig.bTouchNvic)
	{
		EnableNvic(UART0_IRQn,IrqLevel3,TRUE);
	}
	stcConfig.enRunMode = USER_UART_MODE;//模式3
	stcConfig.enStopBit = Uart1bit;  
	stcMulti.enMulti_mode = UartNormal;//正常工作模式
	Uart_SetMultiMode(USER_UART_CH_ID,&stcMulti);//多主机单独配置

	Uart_Init(USER_UART_CH_ID, &stcConfig);//端口初始化

	Uart_SetClkDiv(USER_UART_CH_ID,Uart8Or16Div);//采样分频
	stcBaud.u32Pclk = Sysctrl_GetPClkFreq();
	stcBaud.enRunMode = USER_UART_MODE;
	stcBaud.u32Baud = USER_UART_BAND*2;
	u16Scnt = Uart_CalScnt(USER_UART_CH_ID,&stcBaud);//波特率计算值
	Uart_SetBaud(USER_UART_CH_ID,u16Scnt);//波特率设置

	Uart_ClrIsr(USER_UART_CH_ID);//清所有请求
	Uart_EnableIrq(USER_UART_CH_ID,UartTxIrq);
	Uart_DisableFunc(USER_UART_CH_ID,UartRx); 
	enTb8 = UartDataOrAddr;//Bit8由软件读写决定
	Uart_SetMMDOrCk(USER_UART_CH_ID,enTb8);
	Uart_ClrStatus(USER_UART_CH_ID,UartTC);
	M0P_UART0->SBUF = 0x55;  //发送地址后，触发中断，后续发送数据        

}
#else
void user_driver_uart_print(const char* string,uint16_t value)
{

}

void user_driver_uart_init(void)
{

}
#endif
