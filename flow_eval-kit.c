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

extern u8 UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM;
extern char PATH[64]; //global variable

void eval_kit_flow(void)
{
	enum DISPLAY_TYPE display_type;

	// 1st try to read display-type from MTP
	display_type=read_display_type_from_MTP();
	if (display_type == UNKNOWN)
	{
		// 2nd try to read display-type from SD-Card
		display_type = sdcard_read_display_type("display-type.txt");
		if (display_type == UNKNOWN)
			// finally: set display-type to default (1.38'')
			display_type = S014_T1_1;

	}
	set_display_type(display_type);

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
{
	u8 waveform_from_file[WAVEFORM_LENGTH];
	char path[64];

	sprintf(path, "/%s/%s", PATH, "display/waveform.bin");

	if (sdcard_load_waveform(path, waveform_from_file, WAVEFORM_LENGTH))
	{
		UC8156_send_waveform(waveform_from_file);
		UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM =  WAVEFORM_FROM_LUT;
	}
}

	clear_display();

	while(1)
	{
		slideshow_run(FULL_UPDATE, 2000);
	}
}


