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
 * main_basic-flow.c
 *
 *  Created on: 23 Aug 2016
 *      Author: andreas.meier
 */

#include <stdlib.h>
#include "types.h"
#include "UC8156.h"
#include "image.h"
#include "display_functions.h"

void basic_flow(void)
{
	UC8156_wait_for_BUSY_inactive(); // wait for power-up completed

	// optional: additional hardware reset after 5ms delay
	mdelay(5);
	UC8156_hardware_reset(); // UC8156 hardware reset
	UC8156_wait_for_BUSY_inactive(); // wait for RESET completed

	// optional -> verifies successful power-up
	UC8156_check_RevID();

	UC8156_init_registers(); // over-writes some register values with display-specific values

	// optional -> check for possible problems
	UC8156_check_status_register(0x00);

	clear_display();

//	UC8156_show_image(visa_180x100, FULL_UPDATE, NORMAL_4GL);
}


