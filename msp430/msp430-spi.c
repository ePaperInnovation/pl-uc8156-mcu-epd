
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
 * msp430-spi.c -- MSP430 SPI interface driver
 *
 * Authors: Nick Terry <nick.terry@plasticlogic.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <msp430.h>

#include "types.h"
#include "assert.h"
#include "utils.h"

#include "msp430-defs.h"
#include "msp430-spi.h"
#include "msp430-gpio.h"

/* We only support a single SPI bus and that bus is defined at compile
 * time.
 */
int spi_init(u8 spi_channel, u16 divisor)
{
	if (spi_channel != 0)
		return -EACCES;

	UCxnCTL1 |= UCSWRST;					// Put state machine in reset

	// Define chip select pins for multiple controllers
	gpio_request(SPI_SIMO,	PIN_SPECIAL | PIN_OUTPUT);
	gpio_request(SPI_SOMI, 	PIN_SPECIAL | PIN_INPUT);
	gpio_request(SPI_CLK, 	PIN_SPECIAL | PIN_OUTPUT);
	gpio_request(SPI_CS, PIN_GPIO | PIN_OUTPUT | PIN_INIT_HIGH);
	gpio_request(SPI_CS_SLAVE, PIN_GPIO | PIN_OUTPUT | PIN_INIT_HIGH);

	// SPI setting, MSb first, 8bit, Master Mode, 3 pin SPI, Synch Mode
	UCxnCTL0 |= (UCMST | UCSYNC | UCMSB | UCCKPH);

	UCxnCTL1 |= UCSSEL_2;					// SMCLK is selected
    UCxnBR0 = (divisor & 0x00ff);			// f_UCxCLK = 20MHz/1 = 20MHz
    UCxnBR1 = ((divisor >> 8) & 0x00ff);	//
	UCxnIE = 0x00;							// All interrupts disabled

	UCxnCTL1 &= ~UCSWRST;                  	// Release state machine from reset

	return 0;
}

void spi_close(void)
{
	UCxnCTL1 |= UCSWRST;                      // Put state machine in reset
}

u8 spi_write_read_byte(u8 byte)
{
    while (UCxnSTAT & UCBUSY) ;                     // Wait for all TX/RX to finish
    UCxnTXBUF = byte;                        // Write byte
    while (UCxnSTAT & UCBUSY) ;                     // Wait for all TX/RX to finish

    return UCxnRXBUF;                                      // Dummy read to empty RX buffer
}



u8 spi_write_read_byte_GL0()
{
    while (UCxnSTAT & UCBUSY) ;                     // Wait for all TX/RX to finish
    UCxnTXBUF = 0x00;                        // Write byte
    while (UCxnSTAT & UCBUSY) ;                     // Wait for all TX/RX to finish

    return UCxnRXBUF;                                      // Dummy read to empty RX buffer
}

u8 spi_write_read_byte_GL4()
{
    while (UCxnSTAT & UCBUSY) ;                     // Wait for all TX/RX to finish
    UCxnTXBUF = 0x55;                        // Write byte
    while (UCxnSTAT & UCBUSY) ;                     // Wait for all TX/RX to finish

    return UCxnRXBUF;                                      // Dummy read to empty RX buffer
}

u8 spi_write_read_byte_GL15()
{
    while (UCxnSTAT & UCBUSY) ;                     // Wait for all TX/RX to finish
    UCxnTXBUF = 0xff;                        // Write byte
    while (UCxnSTAT & UCBUSY) ;                     // Wait for all TX/RX to finish

    return UCxnRXBUF;                                      // Dummy read to empty RX buffer
}

u8 spi_write_read_byte_inv(u8 byte)
{
    while (UCxnSTAT & UCBUSY) ;                     // Wait for all TX/RX to finish
    UCxnTXBUF = 255 -  byte;                        // Write byte
    while (UCxnSTAT & UCBUSY) ;                     // Wait for all TX/RX to finish

    return UCxnRXBUF;                                      // Dummy read to empty RX buffer
}



void spi_write_command(u8 command, u8 *params, int count)
{
	int i;

	gpio_set_value_lo(SPI_CS);
	command &= ~0x80;
	spi_write_read_byte(command);
	for (i=0; i<count; i++)
		spi_write_read_byte(*(params+i));
	gpio_set_value_hi(SPI_CS);
}

void spi_write_command_slave(u8 command, u8 *params, int count)
{
	int i;

	gpio_set_value_lo(SPI_CS_SLAVE);
	command &= ~0x80;
	spi_write_read_byte(command);
	for (i=0; i<count; i++)
		spi_write_read_byte(*(params+i));
	gpio_set_value_hi(SPI_CS_SLAVE);
}

void spi_write_command_1param(u8 command, u8 param1)
{
	gpio_set_value_lo(SPI_CS);
	command &= ~0x80;
	spi_write_read_byte(command);
	spi_write_read_byte(param1);
	gpio_set_value_hi(SPI_CS);
}

void spi_write_command_1param_slave(u8 command, u8 param1)
{
	gpio_set_value_lo(SPI_CS_SLAVE);
	command &= ~0x80;
	spi_write_read_byte(command);
	spi_write_read_byte(param1);
	gpio_set_value_hi(SPI_CS_SLAVE);
}

void spi_write_command_2params(u8 command, u8 param1, u8 param2)
{
	gpio_set_value_lo(SPI_CS);
	command &= ~0x80;
	spi_write_read_byte(command);
	spi_write_read_byte(param1);
	spi_write_read_byte(param2);
	gpio_set_value_hi(SPI_CS);
}

void spi_write_command_2params_slave(u8 command, u8 param1, u8 param2)
{
	gpio_set_value_lo(SPI_CS_SLAVE);
	command &= ~0x80;
	spi_write_read_byte(command);
	spi_write_read_byte(param1);
	spi_write_read_byte(param2);
	gpio_set_value_hi(SPI_CS_SLAVE);
}

void spi_write_command_3params(u8 command, u8 param1, u8 param2, u8 param3)
{
	gpio_set_value_lo(SPI_CS);
	command &= ~0x80;
	spi_write_read_byte(command);
	spi_write_read_byte(param1);
	spi_write_read_byte(param2);
	spi_write_read_byte(param3);
	gpio_set_value_hi(SPI_CS);
}

void spi_write_command_4params(u8 command, u8 param1, u8 param2, u8 param3, u8 param4)
{
	gpio_set_value_lo(SPI_CS);
	command &= ~0x80;
	spi_write_read_byte(command);
	spi_write_read_byte(param1);
	spi_write_read_byte(param2);
	spi_write_read_byte(param3);
	spi_write_read_byte(param4);
	gpio_set_value_hi(SPI_CS);
}

u8 spi_read_command_1param(u8 command)
{
	gpio_set_value_lo(SPI_CS);
	command |= 0x80;
	spi_write_read_byte(command);
	u8 temp = spi_write_read_byte(0x00);
	gpio_set_value_hi(SPI_CS);
	return temp;
}

u8 spi_read_command_1param_slave(u8 command)
{
	gpio_set_value_lo(SPI_CS_SLAVE);
	command |= 0x80;
	spi_write_read_byte(command);
	u8 temp = spi_write_read_byte(0x00);
	gpio_set_value_hi(SPI_CS_SLAVE);
	return temp;
}

u8* spi_read_command_2params(u8 command)
{
	static u8 return_value[2];
	gpio_set_value_lo(SPI_CS);
	command |= 0x80;
	spi_write_read_byte(command);
	return_value[0] = spi_write_read_byte(0x00);
	return_value[1] = spi_write_read_byte(0x00);
	gpio_set_value_hi(SPI_CS);
	return return_value;
}

void spi_read_command_2params1(u8 command, u8 *return_value)
{
	gpio_set_value_lo(SPI_CS);
	command |= 0x80;
	spi_write_read_byte(command);
	*return_value = spi_write_read_byte(0x00);
	*(return_value+1) = spi_write_read_byte(0x00);
	gpio_set_value_hi(SPI_CS);
}

int spi_read_command(u8 command, u8 *read_values_p, int count)
{
	int i;

	gpio_set_value_lo(SPI_CS);
	command |= 0x80;
	spi_write_read_byte(command);
	for (i=0; i<count; i++)
		*(read_values_p+i) = spi_write_read_byte(0x00);
	gpio_set_value_hi(SPI_CS);

	return i;
}

int spi_read_command_slave(u8 command, u8 *read_values_p, int count)
{
	int i;

	gpio_set_value_lo(SPI_CS_SLAVE);
	command |= 0x80;
	spi_write_read_byte(command);
	for (i=0; i<count; i++)
		*(read_values_p+i) = spi_write_read_byte(0x00);
	gpio_set_value_hi(SPI_CS_SLAVE);

	return i;
}

void print_spi_read_command(u8 command, int count)
{
	u8 *read_values_p;
	int i, ret;

	read_values_p = (u8*) malloc(count * sizeof(u8));
	if (read_values_p==0)
		abort_now("Fatal error in msp430-spi.c - print_spi_read_command: malloc not successful", ABORT_MSP430_COMMS_INIT);

	ret = spi_read_command(command, read_values_p, count);

	printf("Reg[%02xh] = ", command);
	for(i=0; i<ret; i++)
		printf("0x%02x ", *(read_values_p+i));
	printf("\n");

	free(read_values_p);
}

void print_spi_read_command_slave(u8 command, int count)
{
	u8 *read_values_p;
	int i, ret;

	read_values_p = (u8*) malloc(count * sizeof(u8));
	if (read_values_p==0)
		abort_now("Fatal error in msp430-spi.c - print_spi_read_command: malloc not successful", ABORT_MSP430_COMMS_INIT);

	ret = spi_read_command_slave(command, read_values_p, count);

	printf("Reg[%02xh] = ", command);
	for(i=0; i<ret; i++)
		printf("0x%02x ", *(read_values_p+i));
	printf("\n");

	free(read_values_p);
}

u8* spi_read_command_3params(u8 command)
{
	static u8 return_value[3];
	gpio_set_value_lo(SPI_CS);
	command |= 0x80;
	spi_write_read_byte(command);
	return_value[0] = spi_write_read_byte(0x00);
	return_value[1] = spi_write_read_byte(0x00);
	return_value[2] = spi_write_read_byte(0x00);
	gpio_set_value_hi(SPI_CS);
	return return_value;
}

u8* spi_read_command_4params(u8 command)
{
	static u8 return_value[4];
	gpio_set_value_lo(SPI_CS);
	command |= 0x80;
	spi_write_read_byte(command);
	return_value[0] = spi_write_read_byte(0x00);
	return_value[1] = spi_write_read_byte(0x00);
	return_value[2] = spi_write_read_byte(0x00);
	return_value[3] = spi_write_read_byte(0x00);
	gpio_set_value_hi(SPI_CS);
	return return_value;
}

u8 spi_read_command_1param_1dummy(u8 command)
{
	gpio_set_value_lo(SPI_CS);
	command |= 0x80;
	spi_write_read_byte(command);
	spi_write_read_byte(command); //dummy
	u8 temp = spi_write_read_byte(0x00);
	gpio_set_value_hi(SPI_CS);
	return temp;
}

void spi_write_command_and_bulk_data(u8 command, u8 *buffer, size_t size)
{
	gpio_set_value_lo(SPI_CS);
	command &= ~0x80;
	spi_write_read_byte(command);

    while (size--) {
    	spi_write_read_byte(*buffer);
    	buffer++;
    }
	gpio_set_value_hi(SPI_CS);
}


void spi_write_command_and_bulk_data_GL0(u8 command, u8 *buffer, size_t size)
{
    gpio_set_value_lo(SPI_CS);
    command &= ~0x80;
    spi_write_read_byte(command);

    while (size--) {
        spi_write_read_byte_GL0();
        buffer++;
    }
    gpio_set_value_hi(SPI_CS);
}


void spi_write_command_and_bulk_data_GL4(u8 command, u8 *buffer, size_t size)
{
    gpio_set_value_lo(SPI_CS);
    command &= ~0x80;
    spi_write_read_byte(command);

    while (size--) {
        spi_write_read_byte_GL4();
        buffer++;
    }
    gpio_set_value_hi(SPI_CS);
}



void spi_write_command_and_bulk_data_GL15(u8 command, u8 *buffer, size_t size)
{
    gpio_set_value_lo(SPI_CS);
    command &= ~0x80;
    spi_write_read_byte(command);

    while (size--) {
        spi_write_read_byte_GL15();
        buffer++;
    }
    gpio_set_value_hi(SPI_CS);
}




void spi_write_command_and_bulk_data_inv(u8 command, u8 *buffer, size_t size)
{
    gpio_set_value_lo(SPI_CS);
    command &= ~0x80;
    spi_write_read_byte(command);

    while (size--) {
        spi_write_read_byte_inv(*buffer);
        buffer++;
    }
    gpio_set_value_hi(SPI_CS);
}




void spi_write_command_and_bulk_data_slave(u8 command, u8 *buffer, size_t size)
{
	gpio_set_value_lo(SPI_CS_SLAVE);
	command &= ~0x80;
	spi_write_read_byte(command);

    while (size--) {
    	spi_write_read_byte(*buffer);
    	buffer++;
    }
	gpio_set_value_hi(SPI_CS_SLAVE);
}

void spi_read_command_and_bulk_data(u8 command, u8 *buffer, size_t size)
{
	gpio_set_value_lo(SPI_CS);
	command |= 0x80;
	spi_write_read_byte(command);

    while (size--)
    {
    	*buffer = spi_write_read_byte(0x00); // 0x00 is dummy data
    	buffer++;
    }
	gpio_set_value_hi(SPI_CS);
}

void spi_read_command_and_bulk_data_slave(u8 command, u8 *buffer, size_t size)
{
	gpio_set_value_lo(SPI_CS_SLAVE);
	command |= 0x80;
	spi_write_read_byte(command);

    while (size--)
    {
    	*buffer = spi_write_read_byte(0x00); // 0x00 is dummy data
    	buffer++;
    }
	gpio_set_value_hi(SPI_CS_SLAVE);
}
/*
void spi_write_bulk_data(u8 *buffer, size_t size)
{
    while (size--) {
    	spi_write_byte(*buffer++, 1);
    }
}
*/

void spi_write_command_byte_repeat(u8 command, u8 data, size_t size)
{
	gpio_set_value_lo(SPI_CS);
	command &= ~0x80;
	spi_write_read_byte(command);

	while (size--) {
    	spi_write_read_byte(data);
    }
	gpio_set_value_hi(SPI_CS);
}

void spi_write_command_byte_repeat_slave(u8 command, u8 data, size_t size)
{
	gpio_set_value_lo(SPI_CS_SLAVE);
	command &= ~0x80;
	spi_write_read_byte(command);

	while (size--) {
    	spi_write_read_byte(data);
    }
	gpio_set_value_hi(SPI_CS_SLAVE);
}

