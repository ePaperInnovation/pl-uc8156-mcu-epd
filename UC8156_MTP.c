/*
 * UC8156_MTP.c
 *
 *  Created on: 09.09.2014
 *      Author: andreas.meier
 */

#include "msp430-spi.h"
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
	spi_write_command_2params(0x41, address&0xFF, (address>>8)&0xFF); // set MTP address
	return spi_read_command_1param(0x43);
}
