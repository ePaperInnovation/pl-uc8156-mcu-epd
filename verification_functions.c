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
 * verification_functions.c
 *
 *  Created on: 13.11.2014
 *      Author: andreas.meier
 */

#include <stdio.h>
#include <stdlib.h>

#include "verification_functions.h"
#include "msp430/msp430-spi.h"
#include "UC8156.h"
#include "UC8156_MTP.h"
#include "waveform.h"
#include "image.h"
#include "pattern.h"
#include "display_functions.h"

extern u16 SOURCE_LINES, GATE_LINES, PIXEL_COUNT;
extern u8 *image_data;

void register_dump()
{
	int i;
	u8 *return_value;

	for(i=0;i<0x20;i++)
	{
		return_value=spi_read_command_4params(i);
		printf("%02x: %02x%02x%02x%02x\n", i, *(return_value+3), *(return_value+2), *(return_value+1), *(return_value+0));
	}
}

void print_register_value(u8 reg, u8 count)
{
	int i;

	u8 *values_p = (u8 *)malloc(count);
	spi_read_command_and_bulk_data(reg, values_p, count);

	printf("Reg0x%02xh: ", reg);
	for(i=0; i<count; i++)
		printf("%02x ", *(values_p+i));
	printf("\n");

	free(values_p);
}

u8* read_waveform_LUT()
{
	static u8 LUT[120];
	spi_read_command_and_bulk_data(0x1c, LUT, 120);

	return(LUT);
}

void read_and_print_LUT()
{
	u8 *LUT = read_waveform_LUT();

	int i=0,j=0;
	printf("\n");
	for(i=0;i<30;i++)
	{
		printf("%2x: ", i*4+j);
		while(j<4)
		{
			printf("%02x ", LUT[i*4+j]);
			j++;
		}
		printf("\n");
		j=0;
	}
}
/*
void read_and_print_MTP()
{
	u8 return_value;

	int i=0,j=0;
	printf("\n");
	for(i=0;i<30;i++)
	{
		printf("%2x: ", i*4+j);
		while(j<4)
		{
			return_value = read_MTP_address(i*4+j);
			printf("%2x ", return_value);
			j++;
		}
		printf("\n");
		j=0;
	}
}
*/

void check_temperature_sensor()
{
	u8 return_value;

	return_value = 	spi_read_command_1param(0x07); //read temperature value register
	printf("R07h = %xh\n", return_value);
	return_value = 	spi_read_command_1param(0x08); //read temperature value register
	printf("temperature (R08h) = %dd\n", return_value);

	spi_write_command_1param(0x07, 0x09); //trigger internal temperature sensor read
	mdelay(170);
 	while (spi_read_command_1param(0x15)!=0); // check status register bit for temperature sensor
	return_value = 	spi_read_command_1param(0x07); //read temperature value register
	printf("R07h = %xh\n", return_value);

	return_value = 	spi_read_command_1param(0x08);
	printf("temperature (R08h) = %dd\n", return_value);
}

void drift_test(u8 *waveform_p)
{
	UC8156_HVs_on();

	send_drift_test_image();
	UC8156_update_display(FULL_UPDATE, NORMAL_4GL);

	send_drift_test_image();
	UC8156_set_Vcom(3600);
	UC8156_send_waveform(waveform_long_null);
	UC8156_update_display(FULL_UPDATE, NORMAL_4GL);

	send_drift_test_image();
	//UC8156_set_Vcom(4000);
	UC8156_send_waveform(waveform_p);
	spi_write_command_4params(0x0c, 0x00, 80, GATE_LINES_MAX-GATE_LINES, GATE_LINES_MAX-1); // Panel resolution setting --> SOURCE_E needs to be SOURCELINE instead of SOURCELINE-1 for 180x100, don't know why
	spi_write_command_3params(0x18, 0x40, 0x02, 0x34); //BPCOM=GND, TPCOM=Hi-Z after update, gate_out=VGH after update
	//spi_write_command_4params(0x0c, 0x00, SOURCE_LINES/2/8*8-1, GATE_LINES_MAX-GATE_LINES, GATE_LINES_MAX-1); // Panel resolution setting --> SOURCE_E needs to be SOURCELINE instead of SOURCELINE-1 for 180x100, don't know why
	UC8156_update_display(FULL_UPDATE, NORMAL_4GL);

	spi_write_command_4params(0x0c, 0x00, SOURCE_LINES, GATE_LINES_MAX-GATE_LINES, GATE_LINES_MAX-1); // Panel resolution setting --> SOURCE_E needs to be SOURCELINE instead of SOURCELINE-1 for 180x100, don't know why

	UC8156_HVs_off();

}

void send_drift_test_image()
{
	  int i;

	  for(i=0;i<PIXEL_COUNT/4/2;i++)
		  image_data[i]=0x00;
	  for(i=PIXEL_COUNT/4/2;i<PIXEL_COUNT/4;i++)
		  image_data[i]=0xff;

	  UC8156_send_image_data(image_data);
}

void check_GPIOs()
{
//	GPIO output verification
	spi_write_command_1param(0x09, 0xf0); //configure GPIO's for output
	spi_write_command_1param(0x09, 0xf1); //configure GPIO's for output
	spi_write_command_1param(0x09, 0xf2); //configure GPIO's for output
	spi_write_command_1param(0x09, 0xf4); //configure GPIO's for output
	spi_write_command_1param(0x09, 0xf8); //configure GPIO's for output
}


void verify_update_modes()
{
	//show_image("240x160/13_240~1.PGM", FULL_UPDATE);

	show_image_from_SDcard("4GL.PGM", 0x20 | 0x01);

	white_update();

	spi_write_command_4params(0x0d, 0x00, 0xef, 0x10, 0x9f);
	show_image_from_SDcard("4GL.PGM", 0x20 | 0x01);

	spi_write_command_4params(0x0d, 0x10, 0xef, 0x00, 0x9f);
	show_image_from_SDcard("4GL.PGM", 0x20 | 0x01);
}

void verify_ckbd_SOO_0()
{
	alt_source_SOO_0();
	mdelay(2000);
	clear_display(); // initialize display with 2 white updates
	checkerboard_SOO_0();
}

void power_measurement()
{
	//debug power consumption
	spi_write_command_2params(0x04, 0x11, 0x34);
	//spi_read_command_2params(0x04);
	spi_write_command_2params(0x02, 0x25, 0x88); // set Vgate to +17V/-25V
	spi_write_command_2params(0x06, 0xda, 0xaa); // set timing to LAT=105, S2G+G2S=5

	while(1)
	{
		//checkerboard_SOO_0();
		alt_gate_SOO_0();
		alt_gate_SOO_0();
		checkerboard_SOO_0();
		checkerboard_SOO_0();
		show_image_from_SDcard("240x160/13_240~1.PGM", FULL_UPDATE);
		show_image_from_SDcard("240x160/13_240~1.PGM", FULL_UPDATE);
	}
}

void measure_vcom()
{
#define MEAS_TIME 5 //in seconds
#define MEAS_RESOLUTION 4 // in ms
#define MEAS_COUNT MEAS_TIME * 1000 / MEAS_RESOLUTION + 10

	u8 meas_value[MEAS_COUNT][2], status_reg[MEAS_COUNT];
	int i;

	u8 status = 	spi_read_command_1param(0x15);
	printf("Status Register = %x\n", status);

	UC8156_HVs_on();

	status = 	spi_read_command_1param(0x15);
	printf("Status Register = %x\n", status);

	printf("Reg[19h]=0x%2x\n\n", ((MEAS_TIME * 50 / 24)<<2) | 0x01);
	spi_write_command_1param(0x19, ((MEAS_TIME * 50 / 24)<<2) | 0x01);

	status = 	spi_read_command_1param(0x15);
	printf("Status Register = %x\n", status);

	for (i=0; i<MEAS_COUNT; i++)
	{
		//spi_read_command_2params1(0x1a, &(meas_value[i][0]));
		//status_reg[i] =	spi_read_command_1param(0x15);
		spi_read_command(0x1a, &meas_value[i][0], 2);
		spi_read_command(0x15, &status_reg[i], 1);

		mdelay(MEAS_RESOLUTION);
	}

	UC8156_HVs_off();

	for (i=0; i<MEAS_COUNT; i++)
	{
	//	if (meas_value[i][0]!=0)
//			printf("Vkb[%dms] = %3d - %f\n", (i+1)*MEAS_RESOLUTION, meas_value[i], meas_value[i]*0.03);
//			printf("Vkb[%dms] = %3d - %3d - %f - %d\n", (i+1)*MEAS_RESOLUTION, meas_value[i][0], meas_value[i][1], (meas_value[i][0]+meas_value[i][1]*256)*0.03, status_reg[i]);
	}
}

void measure_vcom_new(int meas_time)
{
//#define MEAS_TIME 10 //in seconds
#define MEAS_RESOLUTION 4 // in ms
//#define MEAS_COUNT MEAS_TIME * 1000 / MEAS_RESOLUTION + 10
#undef MEAS_COUNT
#define MEAS_COUNT 1

	u8 meas_value[MEAS_COUNT][2], status_reg[MEAS_COUNT];
	int i;

	UC8156_HVs_on();

	printf("Reg[19h]=0x%2x\n\n", ((meas_time * 50 / 24)<<2) | 0x01);
	spi_write_command_1param(0x19, ((meas_time * 50 / 24)<<2) | 0x01);

	UC8156_wait_for_BUSY_inactive(); // wait for power-up completed

	for (i=0; i<MEAS_COUNT; i++)
	{
		spi_read_command(0x1a, &meas_value[i][0], 2);
		spi_read_command(0x15, &status_reg[i], 1);

		mdelay(MEAS_RESOLUTION);
	}

	UC8156_HVs_off();

//	for (i=0; i<MEAS_COUNT; i++)
//		printf("Vkb[%dms] = %d - %f - %d\n", (i+1)*MEAS_RESOLUTION, meas_value[i], meas_value[i]*0.03, status_reg[i]);
//		printf("Vkb[%dms] = %3d - %3d - %f - %d\n", (i+1)*MEAS_RESOLUTION, meas_value[i][0], meas_value[i][1], (meas_value[i][0]+meas_value[i][1]*256)*0.03, status_reg[i]);
}

void WF_type2_update_verification()
{
//spi_write_command_1param(0x07, 0x00);
//spi_write_command_1param(0x07, 20);
spi_write_command_1param(0x07, 0x0a);

	//print_spi_read_command(0x44, 1);
/*	u8 R44h_temp;
	spi_read_command(0x44, &R44h_temp, 1);
	spi_write_command_1param(0x44, R44h_temp | 0x40);
*/
	//while(1)
	{
		spi_write_command_1param(0x40, 0x00);
//		read_MTP_addresses_and_print(0x60, 0x28);
		white_update();

		spi_write_command_1param(0x40, 0x02);
//		read_MTP_addresses_and_print(0x60, 0x28);
		white_update();
	}
}

void RAM_window_test_180x100()
{
	u8 image_data[30*30/4];

	u8 x_s=20;
	u8 y_s=10;
	u8 x_e=50;
	u8 y_e=40;

	spi_write_command_4params(0x0d, x_s, x_e-1, GATE_LINES_MAX-y_e, GATE_LINES_MAX-y_s-1); // RAM window setup

	UC8156_send_image_data(image_data);

	UC8156_update_display_with_power_on_off(FULL_UPDATE, NORMAL_4GL);

	x_s=50;
	y_s=50;
	x_e=65;
	y_e=65;

	  spi_write_command_4params(0x0d, x_s, x_e-1, GATE_LINES_MAX-y_e, GATE_LINES_MAX-y_s-1); // RAM window setup

	  UC8156_send_image_data(image_data);

	  UC8156_update_display_with_power_on_off(FULL_UPDATE, NORMAL_4GL);

		x_s=90;
		y_s=70;
		x_e=120;
		y_e=90;

		  spi_write_command_4params(0x0d, x_s, x_e-1, GATE_LINES_MAX-y_e, GATE_LINES_MAX-y_s-1); // RAM window setup

		  UC8156_send_image_data(image_data);

		  UC8156_HVs_on();
		  UC8156_update_display_full();
		  UC8156_HVs_off();
}
#if 0
void RAM_window_test_312x74()
{
	int col_start, col_size, row_start, row_size;

	col_start = 0, col_size = 312;
	row_start = 0, row_size = 74;
	spi_write_command_4params(0x0d, row_start*2, row_start*2+row_size*2-1, col_start/2, col_start/2+col_size/2-1);
	spi_write_command_2params(0x0e, row_start*2, col_size/2-1);
	spi_write_command_1param(0x0f, 2);
	show_image_area(image_74x312_1card, col_size*row_size, FULL_UPDATE);

	col_start = 70, col_size = 8;
	row_start = 10, row_size = 8;
	spi_write_command_4params(0x0d, row_start*2, row_start*2+row_size*2-1, col_start/2, col_start/2+col_size/2-1);
	spi_write_command_2params(0x0e, row_start*2, col_start/2);
	spi_write_command_1param(0x0f, 4);
	show_image_area(image_8x8_scrambled_A, col_size*row_size, PARTIAL_UPDATE);

	//show_image(image_74x312_1card, FULL_UPDATE);
}
#endif
void print_characters_312x74()
{
	int col_start, col_size, row_start, row_size;

	spi_write_command_1param(0x0f, 4);

	col_start = 70, col_size = 8;
	row_start = 10, row_size = 8;
	UC8156_show_image_area(image_8x8_scrambled_A, col_start, col_size, row_start, row_size, PARTIAL_UPDATE, NORMAL_4GL);
	UC8156_send_image_data_area(image_8x8_scrambled_A, col_start, col_size, row_start, row_size);

	col_start = 80, col_size = 8;
	row_start = 10, row_size = 8;
	UC8156_show_image_area(image_8x8_scrambled_A, col_start, col_size, row_start, row_size, PARTIAL_UPDATE, NORMAL_4GL);
	UC8156_send_image_data_area(image_8x8_scrambled_A, col_start, col_size, row_start, row_size);

	col_start = 90, col_size = 8;
	row_start = 10, row_size = 8;
	UC8156_show_image_area(image_8x8_scrambled_A, col_start, col_size, row_start, row_size, PARTIAL_UPDATE, NORMAL_4GL);
}

void drive_active_border_black()
{
	spi_write_command_1param(0x1d, 0x47);

	UC8156_HVs_on();
	UC8156_update_display(FULL_UPDATE, NORMAL_4GL);
	UC8156_HVs_off();

	spi_write_command_1param(0x1d, 0x04);
}

void verify_Area_Update_mode()
{
	diagonale();

//    spi_write_command_4params(0x0d, 0, 10, 00, 20);
//	spi_write_command_4params(0x0d, 0, 180-20, 160-100, 160-10);
//	spi_write_command_4params(0x0d, 0, 63, 160-100, 160-80);

	UC8156_HVs_on();
//	UC8156_update_display(FULL_UPDATE | 0x20);
	UC8156_update_display(FULL_UPDATE, NORMAL_4GL);
    UC8156_HVs_off();

}

void verify_sleep_mode()
{
	spi_write_command_1param(0x21, 0x00);

	UC8156_hardware_reset(); // UC8156 hardware reset
	UC8156_wait_for_BUSY_inactive(); // wait for RESET completed

	UC8156_init_registers();
}

void read_and_print_temperature_sensor()
{
	printf("temperature (R08h) = %dd\n", spi_read_command_1param(0x08));
}
