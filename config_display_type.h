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
 * config_display_type.h
 *
 *  Created on: 9 Sep 2016
 *      Author: andreas.meier
 */

#ifndef CONFIG_DISPLAY_TYPE_H_
#define CONFIG_DISPLAY_TYPE_H_

enum DISPLAY_TYPE { S011_T1_1, S011_T1_2, S014_T1_1, S014_T1_2, S021_T1_1, S025_T1_1, S031_T1_1, D011_T1_1, UC8179_800_600 , UNKNOWN};

void set_display_type(int display_type);
extern volatile int single_display;

#endif /* CONFIG_DISPLAY_TYPE_H_ */
