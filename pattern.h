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
 * pattern.h
 *
 *  Created on: 11.11.2014
 *      Author: andreas.meier
 */

#ifndef PATTERN_H_
#define PATTERN_H_

#include "types.h"

void white_update();
void walking_source_line();
void diagonale();
void checkerboard_debug(u8 data, u8 s_start, u8 s_end);
void checkerboard();
void inv_checkerboard();
void alt_source_debug(u8 data);
void alt_source();
void inv_alt_source();
void alt_source_4er();

#endif /* PATTERN_H_ */
