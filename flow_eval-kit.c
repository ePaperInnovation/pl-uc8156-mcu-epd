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
 * main_eval-kit.c
 *
 *  Created on: 23 Aug 2016
 *      Author: andreas.meier
 */

#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "UC8156.h"
#include "FatFs/ff.h"
#include "display_functions.h"
#include "read-sd.h"
#include "slideshow.h"
#include "config_display_type.h"
#include "UC8156_MTP.h"


extern u8 UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM;
extern char PATH[64]; //global variable

void eval_kit_flow(void)
{
	enum DISPLAY_TYPE display_type;
	u8 waveform_from_file[WAVEFORM_LENGTH];
	char path[64];
	// 1st try to read display-type from MTP
	display_type = read_display_type_from_MTP();
	if (display_type == UNKNOWN)
	{
		// 2nd try to read display-type from SD-Card
		display_type = sdcard_read_display_type("display-type.txt");
		if (display_type == UNKNOWN)
			// finally: set display-type to default (1.38'')
			display_type = S014_T1_1;

	}
	set_display_type(display_type);
	if(single_display){

		UC8156_wait_for_BUSY_inactive(); // wait for power-up completed

		// optional: additional hardware reset after 5ms delay
		mdelay(5);
		UC8156_hardware_reset(); // UC8156 hardware reset
		UC8156_wait_for_BUSY_inactive(); // wait for RESET completed

		// optional -> verifies successful power-up
		UC8156_check_RevID();

		UC8156_init_registers(); // over-writes some register values with display-specific values

		// optional -> check for possible problems
		UC8156_check_status_register(0x00);

		//set Vcom from SD card data -> if "/[display_type]/display/vcom.txt" exist
		int vcom_mv_value;
		if (sdcard_load_vcom(&vcom_mv_value))
			UC8156_set_Vcom(vcom_mv_value);

		//write waveform from SD card data to LUT -> if "/[display_type]/display/waveform.bin" exist

		sprintf(path, "/%s/%s", PATH, "display/S021_T1.1_SPP0B9_V0.uc8156_type1");

		if (sdcard_load_waveform(path, waveform_from_file, WAVEFORM_LENGTH))
		{
			UC8156_send_waveform(waveform_from_file);
			UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM =  WAVEFORM_FROM_LUT;
		}

	}else{
		UC8156_wait_for_BUSY_inactive(); // wait for power-up completed
		UC8156_wait_for_BUSY_inactive_slave(); // wait for power-up completed

		// over-writes some register values with display-specific values
		UC8156_init_registers();
		UC8156_init_registers_slave();

		// optional -> verifies successful power-up
	 	UC8156_check_RevID();
		UC8156_check_RevID_slave();

		// optional -> check for possible problems
		UC8156_check_status_register(0x00);
			//default settings
		sprintf(path, "/%s/%s", PATH, "display/waveform.bin");

			if (sdcard_load_waveform(path, waveform_from_file, WAVEFORM_LENGTH))
			{
				UC8156_send_waveform(waveform_from_file);
				UC8156_send_waveform_slave(waveform_from_file);
				UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM =  WAVEFORM_FROM_LUT;
			}

		UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM =  WAVEFORM_FROM_LUT;
		UC8156_set_Vcom(4000);
		UC8156_set_Vcom_slave(4000);

		//set Vcom from SD card data -> if "/[display_type]/display/vcom.txt" exist
		int vcom_mv_value;
		if (sdcard_load_vcom(&vcom_mv_value))
		{
			UC8156_set_Vcom(vcom_mv_value);
			UC8156_set_Vcom_slave(vcom_mv_value);
		}

		 //write waveform from SD card data to LUT -> if "/[display_type]/display/waveform.bin" exist

		sprintf(path, "/%s/%s", PATH, "display/waveform.bin");


		if (sdcard_load_waveform(path, waveform_from_file, WAVEFORM_LENGTH))
		{
			UC8156_send_waveform(waveform_from_file);
			UC8156_send_waveform_slave(waveform_from_file);
			UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM =  WAVEFORM_FROM_LUT;
		}

	}

	clear_display();

	while(1)
	{
		slideshow_run(FULL_UPDATE, 2000);
	}
}


