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
 * verification_functions.h
 *
 *  Created on: 13.11.2014
 *      Author: andreas.meier
 */

#ifndef VERIFICATION_FUNCTIONS_H_
#define VERIFICATION_FUNCTIONS_H_

#include "types.h"

void check_temperature_sensor();
u8* read_waveform_LUT();
void read_and_print_MTP();
void send_drift_test_image();

#endif /* VERIFICATION_FUNCTIONS_H_ */
