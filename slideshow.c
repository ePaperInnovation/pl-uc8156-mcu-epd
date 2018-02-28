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


#include <stdio.h>
#include <string.h>

#include "types.h"
#include "FatFs/ff.h"
#include "UC8156.h"
#include "utils.h"
#include "display_functions.h"
#include "config_display_type.h"

/* FatFS only supports 8.3 filenames, and we work from the current directory so
   paths should be short... */
#define MAX_PATH_LEN 64
extern char PATH[64]; //global variable

/** Slideshow, calling show_image for each pgm-file found in a directory */
void slideshow_run(int mode, u16 delay_ms)
{
	DIR dir;
	FILINFO fno;
	char path[MAX_PATH_LEN];
	char full_path[MAX_PATH_LEN];
	int result = 0;

	sprintf(path, "/%s/%s", PATH, "img");


	if (f_opendir(&dir, path) != FR_OK)
		abort_now("Fatal error in: slideshow.c -> slideshow_run -> f_opendir", ABORT_SD_CARD);

	do {
		if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0)
			break;

		/* skip directories */
		if ((fno.fname[0] == '.') || (fno.fattrib & AM_DIR))
			continue;

		/* .. and files without the PGM extension */
		if (!strstr(fno.fname, ".PGM") && !strstr(fno.fname, ".pgm"))
			continue;

		sprintf(full_path, "%s/%s", path, fno.fname);
		if(single_display){
			show_image_from_SDcard(full_path, mode);
		}else{
			show_image_from_SDcard_dual(full_path, mode);
		}
		mdelay(delay_ms);

	} while (result >= 0);
}

