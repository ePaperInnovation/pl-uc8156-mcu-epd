/*
 * pattern.c
 *
 *  Created on: 11.11.2014
 *      Author: andreas.meier
 */

#include "pattern.h"
#include "UC8156.h"
#include "config.h"
#include "msp430/msp430-spi.h"

void walking_source_line()
{
	u8 image_data[PIXEL_COUNT/4];
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
			UC8156_update_display(FULL_UPDATE);
			UC8156_HVs_off();

			for (gate_p=0; gate_p<GATE_LINES; gate_p++)
				image_data[gate_p*SOURCE_LINES/4+source_p]=0xff;
		}
	}
}

void diagonale()
{
	u8 image_data[PIXEL_COUNT/4];
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

	UC8156_HVs_on();
	UC8156_update_display(FULL_UPDATE);
    UC8156_HVs_off();
}

void alt_source_SOO_0()
{
	u8 image_data[PIXEL_COUNT/4];
	int gate_p=0, source_p=0;

	for (gate_p=0; gate_p<GATE_LINES; gate_p++)
		for (source_p=0; source_p<SOURCE_LINES/2/4; source_p++)
			image_data[gate_p*SOURCE_LINES/4+source_p]=0x00;
	for (gate_p=0; gate_p<GATE_LINES; gate_p++)
		for (source_p=SOURCE_LINES/2/4; source_p<SOURCE_LINES/4; source_p++)
			image_data[gate_p*SOURCE_LINES/4+source_p]=0xff;

	UC8156_send_image_data(image_data);

	UC8156_HVs_on();
	UC8156_update_display(FULL_UPDATE);
    UC8156_HVs_off();
}

void checkerboard_SOO_0()
{
	u8 image_data[PIXEL_COUNT/4];
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

	UC8156_send_image_data(image_data);

	UC8156_HVs_on();
	UC8156_update_display(FULL_UPDATE);
    UC8156_HVs_off();
}

void alt_source_debug(u8 data)
{
	u8 image_data[PIXEL_COUNT/4];
	int gate_p=0, source_p=0;

	for (gate_p=0; gate_p<GATE_LINES; gate_p++)
		for (source_p=0; source_p<SOURCE_LINES/4; source_p++)
			image_data[gate_p*SOURCE_LINES/4+source_p]=data;//0x33 is "real" alt_source

	UC8156_send_image_data(image_data);

	UC8156_HVs_on();
	UC8156_update_display(FULL_UPDATE);
    UC8156_HVs_off();
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
	u8 image_data[PIXEL_COUNT/4];
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

	UC8156_send_image_data(image_data);

	UC8156_HVs_on();
	UC8156_update_display(FULL_UPDATE);
    UC8156_HVs_off();
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

	UC8156_HVs_on();
	UC8156_update_display(FULL_UPDATE);
	UC8156_HVs_off();
}

void black_update()
{
	UC8156_send_repeated_image_data(0x00); // 0xff is white

	UC8156_HVs_on();
	UC8156_update_display(FULL_UPDATE);
	UC8156_HVs_off();
}

void alt_gate()
{
	u8 image_data[PIXEL_COUNT/4];
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

	UC8156_send_image_data(image_data);

	UC8156_HVs_on();
	UC8156_update_display(FULL_UPDATE);
    UC8156_HVs_off();
}

void alt_gate_SOO_0()
{
	u8 image_data[PIXEL_COUNT/4];
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

	UC8156_send_image_data(image_data);

	UC8156_HVs_on();
	UC8156_update_display(FULL_UPDATE);
    UC8156_HVs_off();
}

void alt_source_4er()
{
	u8 image_data[PIXEL_COUNT/4];
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

	UC8156_send_image_data(image_data);

	UC8156_HVs_on();
	UC8156_update_display(FULL_UPDATE);
    UC8156_HVs_off();
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

	show_image("240x160/08_240~1.PGM", FULL_UPDATE);

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
