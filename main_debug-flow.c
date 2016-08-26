/*
 * main_debug-flow.c
 *
 *  Created on: 23 Aug 2016
 *      Author: andreas.meier
 */

#include <stdlib.h>
#include "types.h"
#include "UC8156.h"
#include "display_functions.h"
#include "read-sd.h"
#include "UC8156_MTP.h"

extern u8 UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM;

void main_debug_flow(void)
{
	sdcard_init(); // initialize SD-card using FatFS

	UC8156_wait_for_BUSY_inactive(); // wait for power-up completed

	mdelay(5);
	UC8156_hardware_reset(); // UC8156 hardware reset
	UC8156_wait_for_BUSY_inactive(); // wait for RESET completed

	if (UC8156_check_RevID() == false)
			exit(EXIT_FAILURE);

	UC8156_init_registers();

	if (UC8156_check_status_register(0x00) == false)
			exit(EXIT_FAILURE);

#if 1	//MTP program - should be used by Plastic Logic only
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

#if 0	//set Vcom from SD card data -> if "/700xxx/display/vcom.txt" exist
{
	int vcom_mv_value;
	if (sdcard_load_vcom(&vcom_mv_value)==0)
		UC8156_set_Vcom(vcom_mv_value);
}
#endif

#if 1
	print_SerialNo_read_from_MTP(); // for debug purposes only
	print_current_VCOM(); // for debug purposes only
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

//	UC8156_send_waveform(waveform_default);
//	UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM =  WAVEFORM_FROM_LUT;

	clear_display();

//	playground();

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

	solid_update(0xAA);

	white_update();

//	checkerboard_20x20();

	while(1)
	{
		slideshow_run(FULL_UPDATE, 1000);
	}

}




