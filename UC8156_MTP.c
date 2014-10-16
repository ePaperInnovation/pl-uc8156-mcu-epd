/*
 * UC8156_MTP.c
 *
 *  Created on: 09.09.2014
 *      Author: andreas.meier
 */
#include <stdio.h>

#include "msp430/msp430-spi.h"
#include "UC8156.h"

void write_Vcom_to_MTP(u16 value)
{
	spi_write_command_2params(0x41, 0xB9, 0x04); // set address for Vcom inside MTP

	spi_write_command_1param(0x42, (value>>8)&0xFF);
	UC8156_wait_for_BUSY_inactive();

	spi_write_command_1param(0x42, value&0xFF);
	UC8156_wait_for_BUSY_inactive();
}

u8 read_MTP_address(u16 address)
{
	spi_write_command_2params(0x41, address&0xFF, (address>>8)&0x07); // set MTP address
	//spi_read_command_2params(0x43);
	return spi_read_command_1param_1dummy(0x43);
}

void complex_MTP_program()
{
	u8 return_value;

	return_value = read_MTP_address(0x0000);
	fprintf(stderr, "return_value = %x\n", return_value);
	return_value = read_MTP_address(0x0010);
	fprintf(stderr, "return_value = %x\n", return_value);
	return_value = read_MTP_address(0x0033);
	fprintf(stderr, "return_value = %x\n", return_value);
	return_value = read_MTP_address(0x0066);
	fprintf(stderr, "return_value = %x\n", return_value);
	return_value = read_MTP_address(0x0077);
	fprintf(stderr, "return_value = %x\n", return_value);

	//set VSH to 9.5V to be used as VPP
	//spi_write_command_2params(0x02, 0x50, 0xFF); //set Vgate+Vsource
	static u8 v1[2];
	spi_read_command_2params1(0x02, v1);
	spi_write_command_2params(0x02, v1[0], 0x33);

	//power-up default is [0x20, 0xE1]
	spi_write_command_2params(0x44, 0x20, 0xEC); //MTP option setting --> Vs=2.4V (new default)

	//write to be programmed data into SRAM
	UC8156_send_repeated_image_data(0x67);

	UC8156_HVs_on();

	//spi_write_command_4params(0x0D, 0x00, 0xEF, 0x00, 0x9F); //set window to full size
	//spi_write_command_2params(0x0E, 0x00, 0x00); //set WS start location to (0,0)
	//spi_write_command_1param(0x0F, 0x00); //set data entry mode

	//switch on VPP manually

	//switch internal VPP on (VSH)
	return_value = spi_read_command_1param(0x03);
	spi_write_command_1param(0x03, return_value|0x08);
	mdelay(100);

	//start MTP program
	spi_write_command_1param(0x40, 0x01);
	UC8156_wait_for_BUSY_inactive();

	//switch VPP off
	return_value = spi_read_command_1param(0x03);
	spi_write_command_1param(0x03, return_value&(~0x08));
	mdelay(100);

	UC8156_HVs_off();

	return_value = read_MTP_address(0x0000);
	fprintf(stderr, "return_value = %x\n", return_value);
	return_value = read_MTP_address(0x0010);
	fprintf(stderr, "return_value = %x\n", return_value);
	return_value = read_MTP_address(0x0033);
	fprintf(stderr, "return_value = %x\n", return_value);
	return_value = read_MTP_address(0x0066);
	fprintf(stderr, "return_value = %x\n", return_value);
	return_value = read_MTP_address(0x0077);
	fprintf(stderr, "return_value = %x\n", return_value);
}

void one_Byte_MTP_program()
{
	u8 return_value;

	static u8 v1[2];
	spi_read_command_2params1(0x02, v1);
	spi_write_command_2params(0x02, v1[0], 0x33);

	return_value = read_MTP_address(0x0000);
	fprintf(stderr, "return_value = %x\n", return_value);

	UC8156_HVs_on();

	//switch internal VPP on (VSH)
	return_value = spi_read_command_1param(0x03);
	spi_write_command_1param(0x03, return_value|0x08);
	mdelay(100);
	//return_value = spi_read_command_1param(0x03);
	//fprintf(stderr, "return_value = %x\n", return_value);

	//setup and start MTP program
	spi_write_command_2params(0x41, 0x00, 0x00); // set address for Vcom inside MTP
	spi_write_command_1param(0x42, 0x35);
	UC8156_wait_for_BUSY_inactive();

	//switch internal VPP off
	return_value = spi_read_command_1param(0x03);
	spi_write_command_1param(0x03, return_value&(~0x08));
	mdelay(100);

	UC8156_HVs_off();

	return_value = read_MTP_address(0x0000);
	fprintf(stderr, "return_value = %x\n", return_value);
}
