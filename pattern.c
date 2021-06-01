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
 * pattern.c
 *
 *  Created on: 11.11.2014
 *      Author: andreas.meier
 */


#include <UC8156.h>

#include "msp430/msp430-spi.h"

#include "display_functions.h"

extern u16 SOURCE_LINES, GATE_LINES, PIXEL_COUNT;
extern u8 *image_data;

void walking_source_line()
{
	int gate_p=0, source_p=0;
	int data, i;

	for (gate_p=0; gate_p<GATE_LINES; gate_p++)
		for (source_p=0; source_p<SOURCE_LINES/4; source_p++)
			image_data[gate_p*SOURCE_LINES/4+source_p]=0xff;

	for (source_p=20; source_p<SOURCE_LINES/4; source_p++)
	{
		for(i=0; i<4; i++)
		{
			if (i==3)
				data=0xfc;
			else if (i==2)
				data=0xf3;
			else if (i==1)
				data=0xcf;
			else if (i==0)
				data=0x3f;

			for (gate_p=0; gate_p<GATE_LINES; gate_p++)
				image_data[gate_p*SOURCE_LINES/4+source_p]=data;

			UC8156_send_image_data(image_data);

			UC8156_HVs_on();
			UC8156_update_display(FULL_UPDATE, NORMAL_4GL);
			UC8156_HVs_off();

			for (gate_p=0; gate_p<GATE_LINES; gate_p++)
				image_data[gate_p*SOURCE_LINES/4+source_p]=0xff;
		}
	}
}

void diagonale()
{
	int gate_p=0, source_p=0;

	for (gate_p=0; gate_p<GATE_LINES; gate_p++)
		for (source_p=0; source_p<SOURCE_LINES/4; source_p++)
			image_data[gate_p*SOURCE_LINES/4+source_p]=0xff;


/*	image_data[0]=0x00;
	image_data[2]=0x00;
	image_data[8]=0x00;
*/
	int i;
	for (i=0; i<GATE_LINES; i++)
		image_data[i*SOURCE_LINES/4+i]=0x00;

	UC8156_send_image_data(image_data);

//	uc8156_HVs_on();
//	uc8156_update_display(FULL_UPDATE);
//    uc8156_HVs_off();
}

void alt_source_SOO_0()
{
	int gate_p=0, source_p=0;

	for (gate_p=0; gate_p<GATE_LINES; gate_p++)
		for (source_p=0; source_p<SOURCE_LINES/2/4; source_p++)
			image_data[gate_p*SOURCE_LINES/4+source_p]=0x00;
	for (gate_p=0; gate_p<GATE_LINES; gate_p++)
		for (source_p=SOURCE_LINES/2/4; source_p<SOURCE_LINES/4; source_p++)
			image_data[gate_p*SOURCE_LINES/4+source_p]=0xff;

	UC8156_show_image(image_data, FULL_UPDATE, NORMAL_4GL);
}

void checkerboard_SOO_0()
{
	int gate_p=0, source_p=0;

	for (gate_p=0; gate_p<GATE_LINES; gate_p+=2)
		for (source_p=0; source_p<SOURCE_LINES/2/4; source_p++)
		{
			image_data[gate_p*SOURCE_LINES/4+source_p]=0x00;
			image_data[(gate_p+1)*SOURCE_LINES/4+source_p]=0xff;
		}
	for (gate_p=0; gate_p<GATE_LINES; gate_p+=2)
		for (source_p=SOURCE_LINES/2/4; source_p<SOURCE_LINES/4; source_p++)
		{
			image_data[gate_p*SOURCE_LINES/4+source_p]=0xff;
			image_data[(gate_p+1)*SOURCE_LINES/4+source_p]=0x00;
		}

	UC8156_show_image(image_data, FULL_UPDATE, NORMAL_4GL);
}

void alt_source_debug(u8 data)
{
	int gate_p=0, source_p=0;

	for (gate_p=0; gate_p<GATE_LINES; gate_p++)
		for (source_p=0; source_p<SOURCE_LINES/4; source_p++)
			image_data[gate_p*SOURCE_LINES/4+source_p]=data;//0x33 is "real" alt_source

	UC8156_show_image(image_data, FULL_UPDATE, NORMAL_4GL);
}

void alt_source()
{
	alt_source_debug(0x33);
}

void inv_alt_source()
{
	alt_source_debug(0xcc);
}

void checkerboard_debug(u8 data, u8 s_start, u8 s_end)
{
	int gate_p=0, source_p=0;

	for (gate_p=0; gate_p<GATE_LINES; gate_p++)
		for (source_p=0; source_p<SOURCE_LINES/4; source_p++)
			image_data[gate_p*SOURCE_LINES/4+source_p]=0xff;

	for (gate_p=0; gate_p<GATE_LINES; gate_p+=2)
	//for (gate_p=20; gate_p<40; gate_p+=2)
		//for (source_p=0; source_p<SOURCE_LINES/4; source_p++)
		for (source_p=s_start; source_p<s_end; source_p++)
		{

//			image_data[gate_p*SOURCE_LINES/4+source_p]=0xf0;
//			image_data[(gate_p+1)*SOURCE_LINES/4+source_p]=0x0f;
//			image_data[gate_p*SOURCE_LINES/4+source_p]=0x3c;
//			image_data[(gate_p+1)*SOURCE_LINES/4+source_p]=~0x3c;
//			image_data[gate_p*SOURCE_LINES/4+source_p]=0x3f;
//			image_data[(gate_p+1)*SOURCE_LINES/4+source_p]=0xcf;
//			image_data[gate_p*SOURCE_LINES/4+source_p]=0xf3;
//			image_data[(gate_p+1)*SOURCE_LINES/4+source_p]=0xfc;
//			image_data[gate_p*SOURCE_LINES/4+source_p]=0x00;
//			image_data[(gate_p+1)*SOURCE_LINES/4+source_p]=0xff;
			image_data[gate_p*SOURCE_LINES/4+source_p]=data;
			image_data[(gate_p+1)*SOURCE_LINES/4+source_p]=~data;
		}

	UC8156_show_image(image_data, FULL_UPDATE, NORMAL_4GL);
}

void checkerboard()
{
	checkerboard_debug(0x33, 0, SOURCE_LINES/4);
}

void inv_checkerboard()
{
	checkerboard_debug(0xcc, 0, SOURCE_LINES/4);
}

void white_update()
{
	UC8156_send_repeated_image_data(0xff); // 0xff is white

	UC8156_update_display_with_power_on_off(FULL_UPDATE, NORMAL_4GL);
}

void black_update()
{
	UC8156_send_repeated_image_data(0x00); // 0xff is white

	UC8156_update_display_with_power_on_off(FULL_UPDATE, NORMAL_4GL);
}

void solid_update(u8 value)
{
	UC8156_send_repeated_image_data(value);

	UC8156_update_display_with_power_on_off(FULL_UPDATE, NORMAL_4GL);
}

void alt_gate()
{
	int gate_p=0, source_p=0;

	spi_write_command_2params(0x0e, 0x00, 0x00); //set SRAM start location

	for (gate_p=0; gate_p<GATE_LINES; gate_p++)
		for (source_p=0; source_p<SOURCE_LINES/4; source_p++)
			image_data[gate_p*SOURCE_LINES/4+source_p]=0xff;

	for (gate_p=0; gate_p<GATE_LINES; gate_p+=2)
	//for (gate_p=20; gate_p<40; gate_p+=2)
		for (source_p=0; source_p<SOURCE_LINES/4; source_p++)
		//for (source_p=20; source_p<21; source_p++)
		{
			image_data[gate_p*SOURCE_LINES/4+source_p]=0x00;
			image_data[(gate_p+1)*SOURCE_LINES/4+source_p]=0xff;
		}

	UC8156_show_image(image_data, FULL_UPDATE, NORMAL_4GL);
}

void alt_gate_SOO_0()
{
	int gate_p=0, source_p=0;

	for (gate_p=0; gate_p<GATE_LINES; gate_p+=2)
		for (source_p=0; source_p<SOURCE_LINES/2/4; source_p++)
		{
			image_data[gate_p*SOURCE_LINES/4+source_p]=0x00;
			image_data[(gate_p+1)*SOURCE_LINES/4+source_p]=0xff;
		}

	for (gate_p=0; gate_p<GATE_LINES; gate_p+=2)
		for (source_p=SOURCE_LINES/2/4; source_p<SOURCE_LINES/4; source_p++)
		{
			image_data[gate_p*SOURCE_LINES/4+source_p]=0x00;
			image_data[(gate_p+1)*SOURCE_LINES/4+source_p]=0xff;
		}

	UC8156_show_image(image_data, FULL_UPDATE, NORMAL_4GL);
}

void alt_source_4er()
{
	int gate_p=0, source_p=0;

	spi_write_command_2params(0x0e, 0x00, 0x00); //set SRAM start location

	for (gate_p=0; gate_p<GATE_LINES; gate_p++)
		for (source_p=0; source_p<SOURCE_LINES/4; source_p++)
			image_data[gate_p*SOURCE_LINES/4+source_p]=0xff;

	for (gate_p=0; gate_p<GATE_LINES; gate_p++)
	//for (gate_p=20; gate_p<40; gate_p+=2)
		for (source_p=0; source_p<SOURCE_LINES/4; source_p+=2)
		//for (source_p=20; source_p<21; source_p++)
		{
			image_data[gate_p*SOURCE_LINES/4+source_p]=0x0f;
			image_data[gate_p*SOURCE_LINES/4+source_p+1]=0xf0;
		}

	UC8156_show_image(image_data, FULL_UPDATE, NORMAL_4GL);
}

void pattern_sequence()
{
	//diagonale();

	//walking_source_line();
	//alt_source_4er();
	checkerboard_debug(0x33, 0, SOURCE_LINES/4);

	checkerboard();
	white_update();
	checkerboard();
	alt_source();

	show_image_from_SDcard("240x160/08_240~1.PGM", FULL_UPDATE);

	diagonale();

	while(1)
	{
		alt_source();
		inv_alt_source();
		white_update();
		alt_source();
		white_update();
		inv_alt_source();
		checkerboard();
		inv_checkerboard();
		white_update();
		checkerboard();
		white_update();
		inv_checkerboard();
	}
}

void pixel_00_update()
{
	//int gate_p=0, source_p=0;
	int i;

//	spi_write_command_2params(0x0e, 0x00, 0x00); //set SRAM start location

/*	for (gate_p=0; gate_p<GATE_LINES; gate_p++)
		for (source_p=0; source_p<SOURCE_LINES/4; source_p++)
			image_data[gate_p*SOURCE_LINES/4+source_p]=0xff;
*/
	for (i=0; i<GATE_LINES*SOURCE_LINES/4; i++)
		image_data[i]=0xff;

	image_data[0]=0x00;
//	image_data[179/4]=0x00;

//	for (i=0; i<200; i++)
//		image_data[i]=0x00;

	//for (source_p=0; source_p<SOURCE_LINES/4; source_p++)
		//image_data[source_p]=0x00;

	UC8156_show_image(image_data, FULL_UPDATE, NORMAL_4GL);
}

void pixel_00_update_windowed()
{
	print_spi_read_command(0x0f, 2);
	print_spi_read_command(0x0e, 2);

	spi_write_command_4params(0x0d, 10, 15, 10, 15); // RAM window setup
	spi_write_command_2params(0x0e, 10, 15); //start Y from 159d/9fh, related to R0fh/DEM setting

	u8 image_data[4];
	image_data[0]=0x00;
	image_data[1]=0x00;
	image_data[2]=0x00;
	image_data[3]=0x00;

	spi_write_command_and_bulk_data(0x10, image_data, 3);
	print_spi_read_command(0x0e, 2);

	//uc8156_send_image_data(image_data);

	UC8156_update_display_with_power_on_off(FULL_UPDATE, NORMAL_4GL);
}

void checkerboard_20x20()
{
	int gate_p=0, source_p=0, x=0, y=0;
	u8 data, data_org=0xff;

	for (gate_p=0; gate_p<GATE_LINES; gate_p++)
		for (source_p=0; source_p<SOURCE_LINES/4; source_p++)
			image_data[gate_p*SOURCE_LINES/4+source_p]=0xff;

	for (gate_p=0; gate_p<GATE_LINES; gate_p+=20)
	{
		for (y=0; y<20; y++)
		{
			data=data_org;
			for (source_p=0; source_p<SOURCE_LINES/4; source_p+=5)
			{
				for(x=0; x<5; x++)
					image_data[(gate_p+y)*SOURCE_LINES/4+source_p+x]=data;
				data=~data;
			}
		}
		data_org=~data_org;
	}

	UC8156_show_image(image_data, FULL_UPDATE, NORMAL_4GL);
}

