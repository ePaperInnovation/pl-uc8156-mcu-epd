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
	if (UC8156_check_RevID() == false)
			exit(EXIT_FAILURE);

	UC8156_init_registers(); // over-writes some register values with display-specific values

	// optional -> check for possible problems
	if (UC8156_check_status_register(0x00) == false)
			exit(EXIT_FAILURE);

	clear_display();

	UC8156_show_image(visa_180x100, FULL_UPDATE, NORMAL_4GL);
}


