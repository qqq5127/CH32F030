#ifndef  __USER_APP_LED_H__
#define  __USER_APP_LED_H__

#define	USER_APP_LED_COUNT_MAX		40

#define	USER_APP_ADC_THRESHOLD					400
#define	USER_APP_ADC_STEP_unit					1
#define	USER_APP_ADC_STEP_MAX						40*USER_APP_ADC_STEP_unit
#define	USER_APP_ADC_STEP_MIN						1

typedef enum USER_APP_LED_MODE
{
    USER_APP_LED_MODE_NULL                          = 0u,  ///<
		USER_APP_LED_MODE_STEP_DOT,
		USER_APP_LED_MODE_STEP_DOT2,		
		USER_APP_LED_MODE_STREAM_REVERSE,

		USER_APP_LED_MODE_MAX,
} USER_APP_LED_MODE_T;

typedef enum USER_APP_LED_LEVEL
{
    USER_APP_LED_LEVEL_FULL                          		 = 0xFF,  ///<
		USER_APP_LED_LEVEL_THREE_QUART											 = 0xBD,
		USER_APP_LED_LEVEL_HALF_FULL											 	 = 0x7F,
		USER_APP_LED_LEVEL_ONE_QUART											   = 0x3F,
} USER_APP_LED_LEVEL_T;

typedef enum USER_APP_LED_COLOUR
{
    USER_APP_LED_COLOUR_1                          		 = 0x0,  ///<

		USER_APP_LED_COLOUR_MAX,
} USER_APP_LED_COLOUR_T;

typedef enum USER_APP_LED_SPEED
{
    USER_APP_LED_SPEED_FAST                          		 = 5,  ///<
		USER_APP_LED_SPEED_NORMAL														 = 10,
		USER_APP_LED_SPEED_SOLW															 = 20,	

}USER_APP_LED_SPEED_T;

extern void user_app_led_init(void);
extern void user_app_led_loop(void);
extern void user_app_led_calcu_level(uint16_t adc_value);
extern void user_app_led_mode_change(void);
extern void user_app_led_colour_change(void);

#endif

