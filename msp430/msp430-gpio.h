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
 * msp430-gpio.h -- MSP430 gpio pin management functions
 *
 * Authors: Nick Terry <nick.terry@plasticlogic.com>
 *
 */

#ifndef MSP430_GPIO_H_
#define MSP430_GPIO_H_

#include <msp430.h>

enum PinDefs {
	PIN_GPIO = BIT0,			// pin is GPIO pin
	PIN_SPECIAL = BIT1,			// pin is special purpose (eg SPI)
	PIN_OUTPUT = BIT2,			// pin is output
	PIN_FULL_DRIVE = BIT3,		// pin (output) full drive strength
	PIN_REDUCED_DRIVE = BIT4,	// pin (output) reduced drive strength
	PIN_INIT_HIGH = BITB,		// pin (output) initialise high
	PIN_INIT_LOW = BITC,		// pin (output) initialise low
	PIN_INPUT = BIT5,			// pin is input
	PIN_PULL_UP = BIT6,			// pin (input) is pulled up
	PIN_PULL_DOWN = BIT7,		// pin (input) is pulled down
	PIN_FALLING_EDGE = BIT8,	// pin (input) interrupt on falling edge
	PIN_RISING_EDGE = BIT9,		// pin (input) interrupt on rising edge
	PIN_INTERRUPT = BITA,		// pin (input) enable interrupt
	PIN_REDEFINE = BITD		// redefine existing pin
};

#define GPIO(port, pin)	(((port)-1) << 8 | (1 << (pin)))

#define PIN_RESET		GPIO(5,0)	// RESET# pin
#define PIN_BUSY 		GPIO(2,6) 	// BUSY_N pin
#define PIN_BUSY_SLAVE	GPIO(2,5) 	// BUSY_N_2 pin
#define PIN_3V3_ENABLE 	GPIO(1,7) 	// 3V3_Enable pin

#define PIN_RUDDOCK_5V_ENABLE 	GPIO(5,1) 	// if Parrot board is used on Ruddock board BB_SHUTDOWN needs to be high in order to enable the 5V Vmain
#define	PIN_ERROR_LED  GPIO(7,7) // Parrot board "User" LED1


void gpio_init(void);
void gpio_mark_busy(int gpio);
int gpio_request(int gpio, u16 flags);
void gpio_set_value(int gpio, int state);
int gpio_get_value(int gpio);
void gpio_set_value_hi(int gpio);
void gpio_set_value_lo(int gpio);

#endif /* MSP430_GPIO_H_ */
