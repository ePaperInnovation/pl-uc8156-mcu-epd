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
 * display_functions.h
 *
 *  Created on: 24 Aug 2016
 *      Author: andreas.meier
 */

#ifndef DISPLAY_FUNCTIONS_H_
#define DISPLAY_FUNCTIONS_H_

void clear_display();
int show_image_from_SDcard(const char *image, int mode);
int show_image_from_SDcard_V2(const char *image, int mode, int waveform_table);


#endif /* DISPLAY_FUNCTIONS_H_ */
