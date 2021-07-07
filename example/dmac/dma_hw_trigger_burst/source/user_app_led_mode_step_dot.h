#ifndef  __USER_APP_LED_STEP_DOT_H__
#define  __USER_APP_LED_STEP_DOT_H__

#include "user_app_led.h"


const uint32_t LED_MODE_STEP_DOT_TAB1[USER_APP_LED_COUNT_MAX]=
{
	//G R B
	0x00e7e732,				//0-39
	0x00b7c300,
	0x009648ad,
	0x009648ad,
	0x009648ad,
	0x009648ad,
	0x009648ad,
	0x009648ad,
	0x009648ad,
	0x009648ad,
	0x009648ad,
	0x009648ad,
	0x009648ad,
	0x009648ad,
	0x009648ad,
	0x0091eedb,
	0x0091eedb,
	0x0091eedb,
	0x008fdbcb,
	0x008fdbcb,
	0x008fdbcb,
	0x008fdbcb,
	0x0000FF00,
	0x0000FF00,
	0x0000FF00,
	0x0000FF00,
	0x0000FF00,
	0x0000FF00,
	0x0000FF00,
	0x0000FF00,
	0x00FFFFFF,
	0x00FFFFFF,
	0x00FFFFFF,
	0x00FFFFFF,
	0x00FFFFFF,
	0x00FFFFFF,
	0x00FFFFFF,
	0x00FFFFFF,
};

const uint32_t* LED_MODE_STEP_DOT_TAB[USER_APP_LED_COLOUR_MAX]=
{
	LED_MODE_STEP_DOT_TAB1,

};

#endif

