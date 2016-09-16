/*
 * display_functions.c
 *
 *  Created on: 24 Aug 2016
 *      Author: andreas.meier
 */

#include "msp430/msp430-spi.h"
#include "UC8156.h"
#include "read-sd.h"
#include "utils.h"

extern u16 PIXEL_COUNT;
extern u8 *image_data;

void clear_display()
{
	u8 reg0fh_value = spi_read_command_1param(0x0f);

	spi_write_command_1param(0x0f, reg0fh_value|0x10); //
	UC8156_send_repeated_image_data(0xff); // 0xff is white
	spi_write_command_1param(0x0f, reg0fh_value&(~0x10)); //
	UC8156_send_repeated_image_data(0xff); // 0xff is white

	UC8156_HVs_on();
	UC8156_update_display(FULL_UPDATE, NORMAL_4GL);
	UC8156_update_display(FULL_UPDATE, NORMAL_4GL);
	//UC8156_update_display(INIT_UPDATE);
	UC8156_HVs_off();
}

// loads image form SD-card and updates it on the display using a 4GL FULL update
void show_image_from_SDcard(char *image, int mode)
{
	sdcard_load_image(image, image_data);

   	UC8156_show_image(image_data, FULL_UPDATE, NORMAL_4GL);
}
