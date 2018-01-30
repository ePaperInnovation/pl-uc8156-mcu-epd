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
 * main_debug-flow.c
 *
 *  Created on: 23 Aug 2016
 *      Author: andreas.meier
 */

#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "UC8156.h"
#include "display_functions.h"
#include "read-sd.h"
#include "UC8156_MTP.h"
#include "waveform.h"

extern u8 UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM;
extern char PATH[64]; //global variable

void debug_flow(void)
{
	//set display type manually OR from MTP
//	set_display_type(S011_T1_1); //enum DISPLAY_TYPE {S014_T1_1, S031_T1_1, S011_T1_1}; --> see config_display_type.c/h
//	set_display_type(S014_T1_1); //enum DISPLAY_TYPE {S014_T1_1, S031_T1_1, S011_T1_1}; --> see config_display_type.c/h
//	set_display_type(S021_T1_1); //enum DISPLAY_TYPE {S014_T1_1, S031_T1_1, S011_T1_1}; --> see config_display_type.c/h
//	set_display_type(S031_T1_1); //enum DISPLAY_TYPE {S014_T1_1, S031_T1_1, S011_T1_1}; --> see config_display_type.c/h

#if 1	//set display type from MTP
{
	enum DISPLAY_TYPE display_type;

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
}
#endif

	UC8156_wait_for_BUSY_inactive(); // wait for power-up completed

	mdelay(5);
	UC8156_hardware_reset(); // UC8156 hardware reset
	UC8156_wait_for_BUSY_inactive(); // wait for RESET completed

	UC8156_check_RevID();

	UC8156_init_registers();

	UC8156_check_status_register(0x00);

#if 0	//MTP program - should be used by Plastic Logic only
{
//		write_single_waveform_table_to_MTP("Type31/waveforms/S031_V7D035_2xGL_V1.23.0.uc8156_lut");
		write_complete_waveform_library_to_MTP_from_file("/waveforms_for_MTP_program/S011_V1.0_VJW003_V2.uc8156");
//		write_complete_waveform_library_to_MTP(waveform_MTP_array);

		write_Vcom_to_MTP(4000);

		UC8156_hardware_reset(); // UC8156 hardware reset
		UC8156_wait_for_BUSY_inactive(); // wait for RESET completed
		UC8156_init_registers();
}
#endif

#if 1	//set Vcom from SD card data -> if "/700xxx/display/vcom.txt" exist
{
	int vcom_mv_value;
	if (sdcard_load_vcom(&vcom_mv_value)==true)
		UC8156_set_Vcom(vcom_mv_value);
}
#endif

#if 1
	print_SerialNo_read_from_MTP(); // for debug purposes only
	print_current_VCOM(); // for debug purposes only
	print_WfVersion_read_from_MTP(); // for debug purposes only
	print_Display_Type_read_from_MTP(); // for debug purposes only
	print_MagicWord_read_from_MTP(); // for debug purposes only
#endif

#if 0 	//write waveform from SD card data to LUT -> if "/700xxx/display/waveform.bin" exist
	u8 waveform_from_file[WAVEFORM_LENGTH];
//	if (sdcard_load_waveform("waveform.bin", waveform_from_file, WAVEFORM_LENGTH)==0)
//	if (sdcard_load_waveform("S011_V1.0_VJW003_V1_WfId3_T23.uc8156_lut", waveform_from_file, WAVEFORM_LENGTH)==0)
	if (sdcard_load_waveform("S011_V1.0_VJW003_V2_T23_WfId3.uc8156_lut", waveform_from_file, WAVEFORM_LENGTH)==0)
	{
		UC8156_send_waveform(waveform_from_file);
		UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM =  WAVEFORM_FROM_LUT;
	}
#endif

/*	read_MTP_addresses_and_print(0, 16, 2);
	UC8156_init_registers();
*/

#if 0	//write display type into MTP
	read_display_type_from_MTP();
	program_display_type_into_MTP("S011_T1.1");
	read_display_type_from_MTP();
#endif

#if 0 	//write waveform from header-file
	UC8156_send_waveform(waveform_default);
	UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM =  WAVEFORM_FROM_LUT;
//	UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM =  WAVEFORM_FROM_MTP;
//	UC8156_set_Vcom(4000);
#endif

	clear_display();

#if 0	//measure VCOM
	//print_measured_VCOM();
	UC8156_measure_Vcom_curve();
#endif

#if 1	//steps for measuring current consumption
	clear_display();
	UC8156_HVs_on();	//measure standby HV On
	UC8156_HVs_off();	//measure standby HV Off
	verify_sleep_mode(); //toggle breakpoint in this routine, measure sleep current after breakpoint
	clear_display();
	white_update();	//measure typical current during update
	char path[64];
	sprintf(path, "/%s/%s", PATH, "img_power/1by1Checker_Inv.pgm");
	show_image_from_SDcard(path, FULL_UPDATE);
	sprintf(path, "/%s/%s", PATH, "img_power/1by1Checker.pgm");
	show_image_from_SDcard(path, FULL_UPDATE);	//measure worst case 1 during update
	clear_display();
	sprintf(path, "/%s/%s", PATH, "img_power/AltGates.pgm");
	show_image_from_SDcard(path, FULL_UPDATE);	//measure worst case 2 during update
#endif


#if 0	// Debug ActiveBorder
	spi_write_command_1param(0x1d, 0x37); // enable ActiveBorder update towards white
	black_update();
	spi_write_command_1param(0x1d, 0xc7); // enable ActiveBorder update towards black
	white_update();
	spi_write_command_1param(0x1d, 0x37); // enable ActiveBorder update towards white
	black_update();
	spi_write_command_1param(0x1d, 0x04); // switch ActiveBorder update off
#endif

	show_image_from_SDcard("/S011_T1.1/img/a_PL_148x70pxl_display.pgm", FULL_UPDATE);

#if 0	//scroll text on 1.38
	show_image_from_SDcard("/S014_T1.1/img_Employee-ID/Employee-ID_3.pgm", FULL_UPDATE);
	while(1)
	{
	show_image_from_SDcard_V2("/S014_T1.1/img_Employee-ID/Employee-ID_3-2.pgm", PARTIAL_UPDATE, FAST_2GL);
	show_image_from_SDcard_V2("/S014_T1.1/img_Employee-ID/Employee-ID_3-3.pgm", PARTIAL_UPDATE, FAST_2GL);
	show_image_from_SDcard_V2("/S014_T1.1/img_Employee-ID/Employee-ID_3.pgm", PARTIAL_UPDATE, FAST_2GL);
	}
#endif

	exit(EXIT_SUCCESS);

	//	WF_type2_update_verification();

//	RAM_window_test_180x100();
//	verify_Area_Update_mode();

/*		spi_write_command_1param(0x40, 0x00); // WF mode 1 (4GL/720ms)
		show_image_from_SDcard("/70083x/img/BARCOD~1.PGM", FULL_UPDATE);
		white_update();
		show_image_from_SDcard("/70083x/img/FIRSTD~1.PGM", FULL_UPDATE);
		white_update();

		spi_write_command_1param(0x40, 0x02); // WF mode 2 (2GL/360ms)
		show_image_from_SDcard("/70083x/img/BARCOD~1.PGM", FULL_UPDATE);
		white_update();
		show_image_from_SDcard("/70083x/img/FIRSTD~1.PGM", FULL_UPDATE);
		white_update();

		spi_write_command_1param(0x40, 0x00); // WF mode 1 (4GL/720ms)
		show_image_from_SDcard("/70083x/img/BARCOD~1.PGM", PARTIAL_UPDATE);
		white_update();
		show_image_from_SDcard("/70083x/img/FIRSTD~1.PGM", PARTIAL_UPDATE);
		white_update();

		spi_write_command_1param(0x40, 0x02); // WF mode 2 (2GL/360ms)
		show_image_from_SDcard("/70083x/img/BARCOD~1.PGM", PARTIAL_UPDATE);
		white_update();
		show_image_from_SDcard("/70083x/img/FIRSTD~1.PGM", PARTIAL_UPDATE);
		white_update();
*/

//	UC8156_send_waveform(waveform_S011_V1_0_VJW003_V2_T23_WfId3_uc8156_lut);
//	UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM =  WAVEFORM_FROM_LUT;


//	black_update();

//	solid_update(0x55);

	checkerboard();
//	alt_source();

	solid_update(0xAA);

	spi_write_command_1param (0x1d, 0x30); //switch Active Border
	white_update();

	spi_write_command_1param (0x1d, 0x3f); //switch Active Border
	white_update();

	spi_write_command_1param (0x1d, 0x30); //switch Active Border
	white_update();

	//	checkerboard_20x20();

	while(1)
	{
		slideshow_run(FULL_UPDATE, 1000);
	}

}




