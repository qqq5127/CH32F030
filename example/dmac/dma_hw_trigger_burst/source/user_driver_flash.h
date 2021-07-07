#ifndef  __USER_DRIVER_FLASH_H__
#define  __USER_DRIVER_FLASH_H__


#define	USER_DRIVER_SECTOR_ADDR		0xFF00
#define	USER_DRIVER_MODE_VALUE		0xFF00
#define	USER_DRIVER_COLOUR_VALUE		0xFF01

extern void user_driver_flash_init(void);
extern void user_driver_flash_write_mode(uint8_t value);
extern void user_driver_flash_write_colour(uint8_t value);
extern uint8_t user_driver_flash_get_mode(void);
extern uint8_t user_driver_flash_get_colour(void);

#endif

