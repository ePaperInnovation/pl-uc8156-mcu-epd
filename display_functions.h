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
void tri_color_display();
int show_image_from_SDcard(char *image, int mode);
int show_image_from_SDcard_inv(char *image, int mode, bool inv_bool);
int show_image_from_SDcard_V2(char *image, int mode, int waveform_table);
void show_image_from_SDcard_dual(char *image, int mode);

void show_image_from_SDcard_GL(char *image, int mode, int GL_name);
void show_image_from_SDcard_bg(char *image, int mode);
void show_image_from_SDcard_all_set(char *image, int mode,  u8 transparency_key_value, u8 transparency_display_enable, u8 display_mode_select, bool inv_bool);
#endif /* DISPLAY_FUNCTIONS_H_ */
