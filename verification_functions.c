/*
 * verification_functions.c
 *
 *  Created on: 13.11.2014
 *      Author: andreas.meier
 */

#include <stdio.h>
#include "verification_functions.h"
#include "msp430/msp430-spi.h"
#include "UC8156.h"
#include "UC8156_MTP.h"
#include "waveform.h"
#include "config.h"

void check_temperature_sensor()
{
	u8 return_value;

	return_value = 	spi_read_command_1param(0x07); //read temperature value register
	fprintf(stderr, "R07h = %xh\n", return_value);
	return_value = 	spi_read_command_1param(0x08); //read temperature value register
	fprintf(stderr, "temperature (R08h) = %dd\n", return_value);

	spi_write_command_1param(0x07, 0x09); //trigger internal temperature sensor read
	mdelay(170);
 	while (spi_read_command_1param(0x15)!=0); // check status register bit for temperature sensor
	return_value = 	spi_read_command_1param(0x07); //read temperature value register
	fprintf(stderr, "R07h = %xh\n", return_value);

	return_value = 	spi_read_command_1param(0x08);
	fprintf(stderr, "temperature (R08h) = %dd\n", return_value);
}

void read_waveform_LUT()
{
	u8 buffer[120];
	spi_read_command_and_bulk_data(0x1c, buffer, 120);
}

void read_and_print_MTP()
{
	u8 return_value;

	int i=0,j=0;
	fprintf(stderr, "\n");
	for(i=0;i<30;i++)
	{
		fprintf(stderr, "%2x: ", i*4+j);
		while(j<4)
		{
			return_value = read_MTP_address(i*4+j);
			fprintf(stderr, "%2x ", return_value);
			j++;
		}
		fprintf(stderr, "\n");
		j=0;
	}
}

void drift_test()
{
	UC8156_HVs_on();
/*
	UC8156_send_repeated_image_data(0xff); // 0xff is white
	UC8156_update_display(FULL);

	UC8156_send_waveform(waveform_long_null);
	UC8156_send_repeated_image_data(0xff); // 0xff is white
	UC8156_update_display(FULL);

	UC8156_send_waveform(waveform_default);
	UC8156_send_repeated_image_data(0x00); // 0xff is white
	UC8156_update_display(FULL);

	UC8156_send_waveform(waveform_long_null);
	UC8156_send_repeated_image_data(0x00); // 0xff is white
	UC8156_update_display(FULL);
*/
	//spi_write_command_1param(0x0f, 0x06); //
	send_drift_test_image();
	UC8156_update_display(FULL_UPDATE);

	UC8156_send_waveform(waveform_long_null);
	UC8156_update_display(FULL_UPDATE);

	UC8156_HVs_off();

}

void send_drift_test_image()
{
	  u8 image_data[PIXEL_COUNT/4];
	  int i;

	  for(i=0;i<PIXEL_COUNT/4/3;i++)
		  image_data[i]=0x00;
	  for(i=PIXEL_COUNT/4/3;i<PIXEL_COUNT/4;i++)
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

	show_image("4GL.PGM", 0x20 | 0x01);

	white_update();

	spi_write_command_4params(0x0d, 0x00, 0xef, 0x10, 0x9f);
	show_image("4GL.PGM", 0x20 | 0x01);

	spi_write_command_4params(0x0d, 0x10, 0xef, 0x00, 0x9f);
	show_image("4GL.PGM", 0x20 | 0x01);
}

void verify_ckbd_SOO_0()
{
	alt_source_SOO_0();
	mdelay(2000);
	clear_display(); // initialize display with 2 white updates
	checkerboard_SOO_0();
}
