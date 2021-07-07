#ifndef  __USER_DRIVER_UART_H__
#define  __USER_DRIVER_UART_H__

#define		USER_DEBUGE_MODE	0


#define	USER_UART_CH_ID		UARTCH0
#define	USER_UART_MODE		UartMode1

#define	USER_UART_TX_PORT	GpioPortA
#define	USER_UART_TX_PIN	GpioPin9

#define	USER_UART_BAND		115200

extern void user_driver_uart_init(void);
extern void user_driver_uart_print(const char* string,uint16_t value);

#endif

