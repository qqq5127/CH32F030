#ifndef  __USER_DRIVER_PWM_DMA_H__
#define  __USER_DRIVER_PWM_DMA_H__


extern void user_driver_pwm_dma_init(void);
extern void user_refresh_led(void);
extern uint32_t  pwm_led_buff[40][24];

#endif

