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
 * msp430-spi.h -- MSP430 SPI interface driver
 *
 * Authors: Nick Terry <nick.terry@plasticlogic.com>
 *
 */

#ifndef MSP430_SPI_H_
#define MSP430_SPI_H_

#include <stddef.h>
#include <types.h>
#include <stdint.h>

#define USCI_UNIT	A
#define USCI_CHAN	0
// Pin definitions for this unit
#define	SPI_SIMO		GPIO(3,4)
#define	SPI_SOMI		GPIO(3,5)
#define	SPI_CLK			GPIO(3,0)
#define SPI_CS GPIO(3,6)
#define SPI_CS_SLAVE	GPIO(1,6)

#define SPI_MODE_0 (UCCKPH)                            /* CPOL=0 CPHA=0 */
#define SPI_MODE_1 (0)                         /* CPOL=0 CPHA=1 */
#define SPI_MODE_2 (UCCKPL | UCCKPH)    /* CPOL=1 CPHA=0 */
#define SPI_MODE_3 (UCCKPL)                            /* CPOL=1 CPHA=1 */
#define SPI_MODE_MASK (UCCKPL | UCCKPH)
// function prototypes
int spi_init(u8 spi_channel, u16 divisor);

void spi_write_command_1param(u8 command, u8 param);
void spi_write_command_2params(u8 command, u8 param1, u8 param2);
void spi_write_command_3params(u8 command, u8 param1, u8 param2, u8 param3);
void spi_write_command_4params(u8 command, u8 param1, u8 param2, u8 param3, u8 param4);
void spi_write_command(u8 command, u8 *params, int count);

u8 spi_read_command_1param(u8 command);
void spi_read_command_2params1(u8 command, u8 *);
u8* spi_read_command_2params(u8 command);
u8* spi_read_command_3params(u8 command);
u8* spi_read_command_4params(u8 command);
int spi_read_command(u8 command, u8 *read_values_p, int count);

u8 spi_read_command_1param_1dummy(u8 command);

void spi_write_command_and_bulk_data(u8 command, u8 *buffer, size_t size);
void spi_read_command_and_bulk_data(u8 command, u8 *buffer, size_t size);

void spi_write_command_byte_repeat(u8 command, u8 value, size_t size);
void print_spi_read_command(u8 command, int count);

// Function prototypes for dual controller displays - second display
void spi_write_command_1param_slave(u8 command, u8 param1);
void spi_write_command_2params_slave(u8 command, u8 param1, u8 param2);
void spi_write_command_slave(u8 command, u8 *params, int count);
u8 spi_read_command_1param_slave(u8 command);
void print_spi_read_command(u8 command, int count);
void print_spi_read_command_slave(u8 command, int count);
void spi_write_command_and_bulk_data_slave(u8 command, u8 *buffer, size_t size);
void spi_read_command_and_bulk_data_slave(u8 command, u8 *buffer, size_t size);
void spi_write_command_byte_repeat_slave(u8 command, u8 data, size_t size);
u8 spi_write_read_byte_GL0();
u8 spi_write_read_byte_GL4();
u8 spi_write_read_byte_GL15();
u8 spi_write_read_byte(u8 byte);
u8 spi_write_read_byte_inv(u8 byte);
void spi_write_command_and_bulk_data_inv(u8 command, u8 *buffer, size_t size);

void spi_write_command_and_bulk_data_GL0(u8 command, u8 *buffer, size_t size);
void spi_write_command_and_bulk_data_GL4(u8 command, u8 *buffer, size_t size);
void spi_write_command_and_bulk_data_GL15(u8 command, u8 *buffer, size_t size);

#endif /* MSP430_SPI_H_ */
