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
 * UC8156_MTP.h
 *
 *  Created on: 15.09.2014
 *      Author: andreas.meier
 */

#ifndef UC8156_MTP_H_
#define UC8156_MTP_H_

#include "types.h"
#include "config_display_type.h"

void write_Vcom_to_MTP(u16 value);
u8 read_MTP_address(const u16 address);

void one_Byte_MTP_program(u16 address, u8 data);
void complex_MTP_program();
void write_waveform_to_MTP(u8 *waveform_data, int waveform_data_length, int mtp_offset_pgrs, int wf_type);
void write_complete_waveform_library_to_MTP_from_file(char *filename);

void print_SerialNo_read_from_MTP();
void print_WfVersion_read_from_MTP();
void print_Display_Type_read_from_MTP();
void print_MagicWord_read_from_MTP();



enum DISPLAY_TYPE read_display_type_from_MTP();

enum DISPLAY_TYPE convert_string_to_DISPLAY_TYPE(const char *display_type_string);

#endif /* UC8156_MTP_H_ */
