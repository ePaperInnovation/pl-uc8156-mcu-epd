/*
  Plastic Logic EPD project on MSP430

  Copyright (C) 2013 - 2017 Plastic Logic

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
 * display_functions.c
 *
 *  Created on: 24 Aug 2016
 *      Author: andreas.meier
 */

#include "msp430/msp430-spi.h"
#include "msp430/msp430-gpio.h"
#include "UC8156.h"
#include "read-sd.h"
#include "utils.h"
#include "config_display_type.h"

extern u16 PIXEL_COUNT;
extern u8 *image_data;

void clear_display()
{
	u8 reg0fh_value = spi_read_command_1param(0x0f);

	spi_write_command_1param(0x0f, reg0fh_value|0x10); //
	UC8156_send_repeated_image_data(0xff); // 0xff is white
	spi_write_command_1param(0x0f, reg0fh_value&(~0x10)); //
	UC8156_send_repeated_image_data(0xff); // 0xff is white
	if(single_display){
		UC8156_HVs_on();
		UC8156_update_display(FULL_UPDATE, NORMAL_4GL);
		UC8156_update_display(FULL_UPDATE, NORMAL_4GL);
		//UC8156_update_display(INIT_UPDATE);
		UC8156_HVs_off();
	}else{
		spi_write_command_1param_slave(0x0f, reg0fh_value|0x10); //
		UC8156_send_repeated_image_data_slave(0xff); // 0xff is white
		spi_write_command_1param_slave(0x0f, reg0fh_value&(~0x10)); //
		UC8156_send_repeated_image_data_slave(0xff); // 0xff is white

		spi_write_command_1param_slave(0x03, 0xc6); //Enable external HV supply for Slave

		UC8156_HVs_on_dual();

		UC8156_update_display_dual(FULL_UPDATE, NORMAL_4GL);
		UC8156_update_display_dual(FULL_UPDATE, NORMAL_4GL);

		UC8156_HVs_off_dual();
	}
}

// loads image from SD-card and updates it on the display using a 4GL FULL update
void show_image_from_SDcard(char *image, int mode)
{
	sdcard_load_image(image, image_data);

   	UC8156_show_image(image_data, FULL_UPDATE, NORMAL_4GL);
}

// loads image from SD-card and updates it on the display using given update mode and waveform table
void show_image_from_SDcard_V2(char *image, int mode, int waveform_table)
{
	sdcard_load_image(image, image_data);

   	UC8156_show_image(image_data, mode, waveform_table);
}

void show_image_from_SDcard_dual(char *image, int mode)
{
 	sdcard_load_image(image, image_data);
 	UC8156_show_image_dual(image_data, FULL_UPDATE, NORMAL_4GL);
}
