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
 * UC8156_MTP.c
 *
 *  Created on: 09.09.2014
 *      Author: andreas.meier
 */
//#define DEBUG_PRINTOUTS_ON

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "msp430/msp430-spi.h"
#include "UC8156.h"
#include "UC8156_MTP.h"
#include "read-sd.h"
#include "utils.h"
#include "config_display_type.h"

#define WF_LIBRARY_LENGTH 2560
#define WF_1TYPEonly_LENGTH 0x4b9 //=1209d
#define MTP_TYPE2_OFFSET 0x500
#define MTP_VCOM_OFFSET 0x04b9

#define WF_TYPE1 0 // R40h.bit1 (MARS) =0
#define WF_TYPE2 2 // R40h.bit1 (MARS) =1

void write_single_waveform_table_to_MTP(char *filename)
{
	u8 waveform_data[WAVEFORM_LENGTH];

	sdcard_load_waveform(filename, waveform_data, WAVEFORM_LENGTH);
	write_waveform_to_MTP(waveform_data, WAVEFORM_LENGTH, 0x00, WF_TYPE1); // program WS1 only

	//read/verify
	u8 value;
	int addr;
	for (addr=0; addr<WAVEFORM_LENGTH; addr++)
	{
		value = read_MTP_address(addr);
		//printf("addr 0x%x - target=0x%x - read=0x%x\n", addr, waveform_data[addr], value);
		if (value != waveform_data[addr])
			printf("Data miss-match on addr 0x%x - target=0x%x - read=0x%x\n", addr, waveform_data[addr], value);
	}

	// program TS to use always WS1
	one_Byte_MTP_program(0x4B0, 99);
}

void write_waveform_library_for_type1_only_to_MTP(char *filename)
{
	u8 waveform_data[WF_LIBRARY_LENGTH];

	sdcard_load_waveform(filename, waveform_data, WF_1TYPEonly_LENGTH);
	write_waveform_to_MTP(waveform_data, WF_1TYPEonly_LENGTH, 0x0b, WF_TYPE1);

	//read/verify
	u8 value;
	int addr;
	for (addr=0; addr<WF_1TYPEonly_LENGTH; addr++)
	{
		value = read_MTP_address(addr);
		//printf("addr 0x%x - target=0x%x - read=0x%x\n", addr, waveform_data[addr], value);
		if (value != waveform_data[addr])
			printf("Data miss-match on addr 0x%x - target=0x%x - read=0x%x\n", addr, waveform_data[addr], value);
	}
}

void write_complete_waveform_library_to_MTP(u8 *waveform_data)
{
	u8 value;
	int addr;

	// write type1
	write_waveform_to_MTP(waveform_data, WF_1TYPEonly_LENGTH, 0x0b, WF_TYPE1);

	//read/verify type1
	for (addr=0; addr<WF_1TYPEonly_LENGTH; addr++)
	{
		value = read_MTP_address(addr);
		//printf("addr 0x%x - target=0x%x - read=0x%x\n", addr, waveform_data[addr], value);
		if (value != waveform_data[addr])
			printf("Data miss-match on addr 0x%x - target=0x%x - read=0x%x\n", addr, waveform_data[addr], value);
	}

	// write type2
	write_waveform_to_MTP(waveform_data+MTP_TYPE2_OFFSET, WF_1TYPEonly_LENGTH, 0x0b, WF_TYPE2);

	//read/verify type2
	spi_write_command_1param(0x40, 0x02); //set MARS=1
	for (addr=0; addr<WF_1TYPEonly_LENGTH; addr++)
	{
		value = read_MTP_address(addr);
		//printf("addr 0x%x - target=0x%x - read=0x%x\n", addr, waveform_data[addr], value);
		if (value != waveform_data[addr+MTP_TYPE2_OFFSET])
			printf("Data miss-match on addr 0x%x - target=0x%x - read=0x%x\n", addr, waveform_data[addr+MTP_TYPE2_OFFSET], value);
	}

	//read/verify type1 -> double-check
	spi_write_command_1param(0x40, 0x00); //set MARS=0
	for (addr=0; addr<WF_1TYPEonly_LENGTH; addr++)
	{
		value = read_MTP_address(addr);
		//printf("addr 0x%x - target=0x%x - read=0x%x\n", addr, waveform_data[addr], value);
		if (value != waveform_data[addr])
			printf("Data miss-match on addr 0x%x - target=0x%x - read=0x%x\n", addr, waveform_data[addr], value);
	}
}

void write_complete_waveform_library_to_MTP_from_file(char *filename)
{
	u8 waveform_data[WF_LIBRARY_LENGTH];

	// load complete waveform library (2 types, size=0xA00) from SD-card
	sdcard_load_waveform(filename, waveform_data, WF_LIBRARY_LENGTH);

	write_complete_waveform_library_to_MTP(waveform_data);
}

void write_Vcom_to_MTP(u16 Vcom_mv_value)
{
	u16 Vcom_register_value = (float)Vcom_mv_value/(float)30.0;

	one_Byte_MTP_program(MTP_VCOM_OFFSET, Vcom_register_value);

	u8 read_value = read_MTP_address(MTP_VCOM_OFFSET);
#ifdef DEBUG_PRINTOUTS_ON
	printf("written value: 0x%x\n", Vcom_register_value);
	printf("read value   : 0x%x\n", read_value);
#endif

	if (read_value != Vcom_register_value)
		abort_now("Fatal error in: UC8156_MTP.c -> write_Vcom_to_MTP", ABORT_UC8156_MTP);
}

void write_waveform_to_MTP(u8 *waveform_data, int waveform_data_length, int mtp_offset_pgrs, int wf_type)
{
	//set VSH to 9.5V to be used as VPP
	//spi_write_command_2params(0x02, 0x50, 0xFF); //set Vgate+Vsource
	static u8 reg02h_backup[2];
	spi_read_command_2params1(0x02, reg02h_backup);
	//spi_write_command_2params(0x02, reg02h_backup[0], 0x32);
	spi_write_command_2params(0x02, reg02h_backup[0], 0x44);

	//set SRAM access parameter
	spi_write_command_4params(0x0D, 0x00, 0xEF, 0x00, 0x9F); //set window to full size
	spi_write_command_2params(0x0E, 0x00, 0x00); //set WS start location to (0,0)
	u8 reg0fh_backup = spi_read_command_1param(0x0f);
	spi_write_command_1param(0x0f, 0x00); //set data entry mode

	//power-up default is [0x20, 0xE1]
	//spi_write_command_2params(0x44, 0x20, 0xEC); //MTP option setting --> Vs=2.4V (new default)
	//spi_write_command_2params(0x44, 0x30, 0xfc); //MTP option setting --> Vs=2.4V (new default)
	spi_write_command_2params(0x44, 0x30, 0xf1); //MTP option setting --> Vs=2.4V (new default)

	spi_write_command_2params(0x0e, 0x00, 0x00);
	//write to be programmed data into SRAM
	UC8156_send_data_to_image_RAM_for_MTP_program(waveform_data, waveform_data_length);

	UC8156_HVs_on();

	//switch internal VPP on (VSH)
	u8 return_value = spi_read_command_1param(0x03);
	spi_write_command_1param(0x03, return_value|0x08);
	mdelay(100);

	//start MTP program
	spi_write_command_1param(0x40, (mtp_offset_pgrs<<4)&0xf0 | wf_type | 0x01);
    UC8156_wait_for_BUSY_inactive();
 	while (spi_read_command_1param(0x15)&0x08); // BUSY loop
#ifdef DEBUG_PRINTOUTS_ON
 	printf("Reg[15h]=0x%02x\n", spi_read_command_1param(0x15));
#endif
	//switch VPP off
	return_value = spi_read_command_1param(0x03);
	spi_write_command_1param(0x03, return_value&(~0x08));
	mdelay(100);

	UC8156_HVs_off();

	//reset register to previous values
	spi_write_command_2params(0x02, reg02h_backup[0], reg02h_backup[1]);
	spi_write_command_1param(0x0f, reg0fh_backup);
}

u8 read_MTP_address(const u16 address)
{
	spi_write_command_2params(0x41, address&0xFF, (address>>8)&0x07); // set MTP address
	return spi_read_command_1param_1dummy(0x43);
}

u8 read_MTP_address_and_print(const u16 address)
{
	u8 return_value = read_MTP_address(address);
#ifdef	DEBUG_PRINTOUTS_ON
	printf("MTP addr %x = %x\n", address, return_value);
#endif
	return return_value;
}

void read_MTP_addresses_and_print(u16 start_address, int count, int type)
{
	u8 value;
	int i;
	u16 address = start_address;

	switch (type)
	{
		case 1:
			spi_write_command_1param(0x40, 0x00); // WF mode 1 (4GL/720ms)
			break;
		case 2:
			spi_write_command_1param(0x40, 0x02); // WF mode 2 (2GL/360ms)
			break;
		default:
			printf("Wrong type parameter in read_MTP_addresses_and_print\n");
			exit(EXIT_FAILURE);
	}

	while (address < (start_address + count))
	{
		printf("0x%02x =", address);
		for (i=0;i<16;i++)
		{
			 value = read_MTP_address(address);
			 address++;
			 printf(" 0x%02x,", value);
		}
		printf("\n");
	}
}

void one_Byte_MTP_program(u16 address, u8 data)
{
	u8 return_value;

	static u8 v1[2];
	spi_read_command_2params1(0x02, v1);
	spi_write_command_2params(0x02, v1[0], 0x33);

#ifdef DEBUG_PRINTOUTS_ON
	return_value = read_MTP_address(address);
	printf("value before program = 0x%x\n", return_value);
#endif

	UC8156_HVs_on();

	//switch internal VPP on (VSH)
	return_value = spi_read_command_1param(0x03);
	spi_write_command_1param(0x03, return_value|0x08);
	mdelay(100);

	//setup and start MTP program
	spi_write_command_2params(0x44, 0x30, 0xf1); //MTP option setting --> Vs=2.4V (new default)
	spi_write_command_2params(0x41, address&0xFF, (address>>8)&0xFF);
	spi_write_command_1param(0x42, data);
	UC8156_wait_for_BUSY_inactive();

	//switch internal VPP off
	return_value = spi_read_command_1param(0x03);
	spi_write_command_1param(0x03, return_value&(~0x08));
	mdelay(100);

	UC8156_HVs_off();

	spi_write_command_2params(0x02, v1[0], v1[1]);

#ifdef DEBUG_PRINTOUTS_ON
	return_value = read_MTP_address(address);
	printf("value to be programed = 0x%x\n", data);
	printf("value after program = 0x%x\n", return_value);
#endif
}

void print_SerialNo_read_from_MTP()
{
	char display_serial_no[19];
	u16 start_address = 0x4bc;
	int i;

	u8 backup_reg40h = spi_read_command_1param(0x40);

	// switch to "type2" MTP area
	spi_write_command_1param(0x40, spi_read_command_1param(0x40) | 0x02);

	for (i=0; i<18; i++)
		display_serial_no[i] = read_MTP_address(start_address + i);
	display_serial_no[18]='\0';

	printf("Display Serial No: %s\n", display_serial_no);

	// restore Reg[40h] value
	spi_write_command_1param(0x40, backup_reg40h);
}

void print_WfVersion_read_from_MTP()
{
	char display_serial_no[32];
	u16 start_address = 0x4d0;
	int i;

	u8 backup_reg40h = spi_read_command_1param(0x40);

	// switch to "type2" MTP area
	spi_write_command_1param(0x40, spi_read_command_1param(0x40) | 0x02);

	for (i=0; i<32; i++)
		display_serial_no[i] = read_MTP_address(start_address + i);
	display_serial_no[31]='\0';

	printf("Wf Version: %s\n", display_serial_no);

	// restore Reg[40h] value
	spi_write_command_1param(0x40, backup_reg40h);
}

void print_Display_Type_read_from_MTP()
{
	char display_type_string[16];
	u16 start_address = 0x4f0;
	int i;

	u8 backup_reg40h = spi_read_command_1param(0x40);

	// switch to "type2" MTP area
	spi_write_command_1param(0x40, spi_read_command_1param(0x40) | 0x02);

	for (i=0; i<16; i++)
		display_type_string[i] = read_MTP_address(start_address + i);
	display_type_string[15]='\0';

	printf("Display Type: %s\n", display_type_string);

	// restore Reg[40h] value
	spi_write_command_1param(0x40, backup_reg40h);
}

void print_MagicWord_read_from_MTP()
{
	char display_type_string[3];
	u16 start_address = 0x4b9;
	int i;

	u8 backup_reg40h = spi_read_command_1param(0x40);

	// switch to "type2" MTP area
	spi_write_command_1param(0x40, spi_read_command_1param(0x40) | 0x02);

	for (i=0; i<3; i++)
		display_type_string[i] = read_MTP_address(start_address + i);
	display_type_string[2]='\0';

	printf("Magic Word: %s\n", display_type_string);

	// restore Reg[40h] value
	spi_write_command_1param(0x40, backup_reg40h);
}

enum DISPLAY_TYPE read_display_type_from_MTP()
{
	char display_type_string[10];
	enum DISPLAY_TYPE display_type_enum = UNKNOWN;
	u16 start_address = 0x4f0;
	int i;

	u8 backup_reg40h = spi_read_command_1param(0x40);

	// switch to "type2" MTP area
	spi_write_command_1param(0x40, spi_read_command_1param(0x40) | 0x02);

	//first try to read display type directly
	for (i=0; i<9; i++)
			display_type_string[i] = read_MTP_address(start_address + i);
		display_type_string[9]='\0';
	display_type_enum = convert_string_to_DISPLAY_TYPE(display_type_string);

	//if not successful, read from MTP using waveform filename
	if (display_type_enum == UNKNOWN)
	{
		start_address = 0x4d0;
		for (i=0; i<9; i++)
			display_type_string[i] = read_MTP_address(start_address + i);
		display_type_string[9]='\0';
		display_type_enum = convert_string_to_DISPLAY_TYPE(display_type_string);
	}

	printf("Display Type: %s\n", display_type_string);

	// restore Reg[40h] value
	spi_write_command_1param(0x40, backup_reg40h);

	return display_type_enum;
}

enum DISPLAY_TYPE convert_string_to_DISPLAY_TYPE(const char *display_type_string)
{
	int u;
	u = 1;
	u = u+1;

	if (strcmp(display_type_string, "S011_T1.1") == 0)
		{return S011_T1_1;}
	if (strcmp(display_type_string, "S011_T1.2") == 0)
		{return S011_T1_2;}
	if (strcmp(display_type_string, "S014_T1.1") == 0)
		{return S014_T1_1;}
	if (strcmp(display_type_string, "S014_T1.2") == 0)
		{return S014_T1_2;}
	if (strcmp(display_type_string, "S021_T1.1") == 0)
		{return S021_T1_1;}
	if (strcmp(display_type_string, "S025_T1.1") == 0)
	        {return S025_T1_1;}
	if (strcmp(display_type_string, "S031_T1.1") == 0)
		{return S031_T1_1;}
	if (strcmp(display_type_string, "D011_T1.1") == 0)
		{return D011_T1_1;}

	return UNKNOWN;
}

void program_display_type_into_MTP(const char *display_type)
{
	u16 start_address = 0x4f0;
	int i;

	u8 backup_reg40h = spi_read_command_1param(0x40);

	// switch to "type2" MTP area
	spi_write_command_1param(0x40, spi_read_command_1param(0x40) | 0x02);

	for (i=0; i<9; i++)
		one_Byte_MTP_program(start_address + i, display_type[i]);

	// restore Reg[40h] value
	spi_write_command_1param(0x40, backup_reg40h);
}
