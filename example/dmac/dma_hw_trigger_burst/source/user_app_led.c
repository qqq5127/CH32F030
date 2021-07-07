#include "ddl.h"
#include "dmac.h"
#include "adc.h"
#include "gpio.h"
#include "bgr.h"
#include "flash.h"
#include "bt.h"
#include "spi.h"
#include "uart.h"
#include "user_app_led.h"
#include "user_driver_pwm_dma.h"
#include "user_app_led_mode_null.h"
#include "user_app_led_mode_step_dot.h"
#include "user_driver_uart.h"


static USER_APP_LED_MODE_T led_mode = USER_APP_LED_MODE_STEP_DOT;
static USER_APP_LED_LEVEL_T led_level = USER_APP_LED_LEVEL_FULL;
static USER_APP_LED_COLOUR_T	led_colour = USER_APP_LED_COLOUR_1;
static USER_APP_LED_SPEED_T		led_speed = USER_APP_LED_SPEED_NORMAL;
static uint8_t led_adc_level = USER_APP_LED_COUNT_MAX;
static uint8_t led_adc_real_level = USER_APP_LED_COUNT_MAX;
static uint8_t led_mode_dot_level = USER_APP_LED_COUNT_MAX;

static uint8_t led_mode_null_colour = 0;
static uint16_t led_mode_null_count = 0;


static boolean_t led_mode_stream_start = TRUE;
static uint8_t led_mode_stream_start_cnt = 0;	

static uint8_t led_mode_reverse_level = 0;

static void user_app_led_clean_buff(void)
{
	memset(pwm_led_buff,0xc0,sizeof(pwm_led_buff));
}

/*
填充白色数据
buff 填充的地址
level  填充的等级 0-255
*/
static void user_app_led_fill(uint32_t *buff,uint32_t clour,uint8_t level)
{
	if((uint32_t)buff > (uint32_t )(&pwm_led_buff[39][23]))
	{
		return; //error
	}

	clour = clour*level/0xff;

	for(uint8_t i=0;i<24;i++)
	{
		if(clour & 0x800000)
		{
			buff[i] = 0xFC;
		}
		else
		{
			buff[i] = 0xC0;
		}
		clour = clour << 1;
	}
}


/*
初始化led参数
*/
static void user_app_led_mode_init(void)
{
	led_mode_null_count = 0;
	led_adc_level = USER_APP_LED_COUNT_MAX;
	led_adc_real_level = USER_APP_LED_COUNT_MAX;
	led_mode_dot_level = USER_APP_LED_COUNT_MAX;
	led_mode_stream_start_cnt = 0;
	led_mode_reverse_level = 0;
	user_app_led_clean_buff();
}

void user_app_led_mode_change(void)
{
	led_mode++;
	if(led_mode >= USER_APP_LED_MODE_MAX)
	{
		led_mode = USER_APP_LED_MODE_STEP_DOT;
	}
	user_app_led_mode_init();

}

void user_app_led_colour_change(void)
{
	led_colour++;
	if(led_colour >= USER_APP_LED_COLOUR_MAX)
	{
		led_colour = USER_APP_LED_COLOUR_1;
	}
	user_app_led_mode_init();
}

void user_app_led_init(void)
{
	uint8_t i,j;
	uint16_t level;
	
	for(i = 0;i < 40;i++)
	{
		for(j=0;j<10;j++)
		{
			user_app_led_fill(pwm_led_buff[i+j],0x00ffffff,j*28);
		}
		user_refresh_led();
		delay1ms(10);
	}

	for(level = 0x0;level<0x200;level++)
	{
		for(i=0;i<40;i++)
		{
			if(level < 0x100)
			{
				user_app_led_fill(pwm_led_buff[i],0x00ffffff,level);
			}
			else
			{
				user_app_led_fill(pwm_led_buff[i],0x00ffffff,0x1ff-level);
			}
		}
		user_refresh_led();
		if(level == 0xC0)
		{
			delay1ms(1000);
		}

	}

	for(i=0;i<40;i++)
	{
		user_app_led_fill(pwm_led_buff[i],0x00ffffff,0);
	}
	user_refresh_led();
	user_app_led_mode_init();

}


static void user_app_led_mode_null(void)
{
	static uint32_t mode_null_colour;
	uint8_t i;

	if(led_mode_null_count < 0x200)
	{
		if(led_mode_null_count == 0)
		{
			mode_null_colour = LED_MODE_NULL_COLOUR_TAB[led_mode_null_colour];
		}
		for(i=0;i<40;i++)
		{
			if(led_mode_null_count < 0x100)
			{
				user_app_led_fill(pwm_led_buff[i],mode_null_colour,led_mode_null_count);
			}
			else
			{
				user_app_led_fill(pwm_led_buff[i],mode_null_colour,0x1ff-led_mode_null_count);
			}
		}
		user_refresh_led();
	}

	led_mode_null_count++;

	if(led_mode_null_count >= 0x3ff)
	{
		led_mode_null_count = 0;
		led_mode_null_colour++;
		if(led_mode_null_colour >= LED_MODE_NULL_COLOUR_TAB_MAX)
		{
			led_mode_null_colour = 0;
		}
	}
	
}

void user_app_led_mode_step_dot(void)
{
	const uint32_t* step_dot_table = NULL;
	static uint8_t led_dot_time_cnt = 0;
	static boolean_t led_sleep_flag = FALSE;
	
	led_dot_time_cnt++;
	step_dot_table = LED_MODE_STEP_DOT_TAB[led_colour];
	user_app_led_clean_buff();
	for(uint8_t i=0;i<led_adc_level;i++)
	{
		user_app_led_fill(pwm_led_buff[i],step_dot_table[i],led_level);
	}

	/*dot display*/
	if(led_dot_time_cnt > 10)
	{
		if(led_mode_dot_level > led_adc_level)
		{
			led_mode_dot_level--;
		}
		led_dot_time_cnt = 0;
	}
	if(led_mode_dot_level < led_adc_level)
	{
		led_mode_dot_level = led_adc_level + 1;
		if(led_mode_dot_level > 39)
		{
			led_mode_dot_level = 39;
		}
	}
	if(led_sleep_flag == FALSE)
	{
		user_app_led_fill(pwm_led_buff[led_mode_dot_level],step_dot_table[led_mode_dot_level],led_level);
	}
	if(led_mode_dot_level == 0)
	{
		led_sleep_flag = TRUE;
	}
	else
	{
		led_sleep_flag = FALSE;
	}
	user_refresh_led();
}


void user_app_led_mode_step_dot2(void)
{
	const uint32_t* step_dot_table = NULL;

	step_dot_table = LED_MODE_STEP_DOT_TAB[led_colour];
	user_app_led_clean_buff();
	for(uint8_t i=0;i<led_adc_level;i++)
	{
		user_app_led_fill(pwm_led_buff[i],step_dot_table[i],led_level);
	}

	user_refresh_led();
}

void user_app_led_mode_stream_vererse(void)
{
	uint8_t i,j;

	if(led_mode_stream_start)
	{
	
		user_app_led_clean_buff();
		for(i=39;i>led_mode_stream_start_cnt;i--)
		{
			user_app_led_fill(pwm_led_buff[i],0x00ffffff,led_level);
			if(i==0)
			{
				break;
			}
		}
		led_mode_stream_start_cnt++;
		if(led_mode_stream_start_cnt == 40)
		{
			led_mode_stream_start = 0;
		}
	}
	else
	{
		for(i=39;i!=0;i--)
		{
			for(j=0;j<24;j++)
			{
				pwm_led_buff[i][j] = pwm_led_buff[i-1][j];
			}
		}
		if(led_mode_reverse_level == 0)
		{
			led_mode_reverse_level = led_adc_level;
		}
		else
		{
			if(led_adc_level > led_mode_reverse_level)	// adc change
			{
				user_app_led_fill(pwm_led_buff[0],0x00ffffff,led_level);
			}
			else
			{
				user_app_led_fill(pwm_led_buff[0],0x00000000,led_level);
			}
			led_mode_reverse_level = led_adc_level;
		}
		
	}
	user_refresh_led();
}




void user_app_led_calcu_level(uint16_t adc_value)
{
	static uint16_t current_adc_value = 0,prev_adc_value = 0;
	uint16_t diff_adc_value;
	
	if(current_adc_value == 0 && prev_adc_value == 0)
	{
		current_adc_value = adc_value;
		prev_adc_value = adc_value;
	}
	
	if((adc_value > prev_adc_value) && (adc_value > USER_APP_ADC_THRESHOLD))
	{
		diff_adc_value = adc_value - prev_adc_value;
		user_driver_uart_print("diff ",diff_adc_value);
		user_driver_uart_print("adc_value ",adc_value);
		user_driver_uart_print("prev_adc_value ",prev_adc_value);
		if(diff_adc_value > USER_APP_ADC_STEP_MAX)
		{
			diff_adc_value = USER_APP_ADC_STEP_MAX;
		}
		if(diff_adc_value >= USER_APP_ADC_STEP_MIN)
		{
			led_adc_real_level = led_adc_real_level + diff_adc_value/USER_APP_ADC_STEP_unit;
			if(led_adc_real_level > USER_APP_LED_COUNT_MAX)
			{
				led_adc_real_level = USER_APP_LED_COUNT_MAX;
			}
		}
		else
		{
			if(led_adc_real_level)
			{
				led_adc_real_level--;
			}
		}
	}
	else
	{
		if(led_adc_real_level)
		{
			led_adc_real_level--;
		}
	}

	prev_adc_value = current_adc_value;
	current_adc_value = adc_value;
	
}

void user_app_led_loop(void)
{
	static uint8_t speed_count;
	if(speed_count >= led_speed)
	{
#if 1		//
	if(led_adc_level > led_adc_real_level)
	{
		led_adc_level--;
	}
	else if(led_adc_level < led_adc_real_level)
	{
		led_adc_level++;
	}
#endif
		switch(led_mode)
		{
			case USER_APP_LED_MODE_NULL:
				user_app_led_mode_null();
			break;

			case USER_APP_LED_MODE_STEP_DOT:
				user_app_led_mode_step_dot();

			break;

			case USER_APP_LED_MODE_STEP_DOT2:
				user_app_led_mode_step_dot2();
			break;
			
			case USER_APP_LED_MODE_STREAM_REVERSE:
				user_app_led_mode_stream_vererse();
			break;
				
			default:

			break;
		}

		speed_count = 0;
	}
	speed_count++;
}

