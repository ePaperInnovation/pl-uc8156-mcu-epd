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

#ifndef READ_SD_H_
#define READ_SD_H_

#include "FatFs/ff.h"

int sdcard_init(void);
void sdcard_load_image(char *image_name, u8 *image_data);
bool sdcard_load_vcom(int *value);
bool sdcard_load_waveform(char *waveform_file_name, u8 *waveform_data, UINT length);
enum DISPLAY_TYPE sdcard_read_display_type(const char *config_file_name);

int read_image_data_from_file_default(FIL *f, u8 *image_buffer);
int read_image_data_from_file_S031_T1(FIL *f, u8 *image_buffer);
int read_image_data_from_file_S021_T1(FIL *f, u8 *image_buffer);
extern int (* read_image_data_from_file) (FIL *f, u8 *image_buffer);

#endif /*READ_SD_H_*/
