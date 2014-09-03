/*
  Plastic Logic EPD project on MSP430

  Copyright (C) 2013 Plastic Logic Limited

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

// function prototypes
int spi_init(u8 spi_channel, u16 divisor);

void spi_write_command_1param(u8 command, u8 param);
void spi_write_command_2params(u8 command, u8 param1, u8 param2);
void spi_write_command_4params(u8 command, u8 param1, u8 param2, u8 param3, u8 param4);

u8 spi_read_command_1param(u8 command);
u8* spi_read_command_2params(u8 command);
u8* spi_read_command_4params(u8 command);

void spi_write_command_and_bulk_data(u8 command, u8 *buffer, size_t size);
void spi_write_bulk_data(u8 *buffer, size_t size);
void spi_write_byte_repeat(u8 byte, size_t size);

#endif /* MSP430_SPI_H_ */
