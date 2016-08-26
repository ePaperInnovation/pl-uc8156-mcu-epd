#include <stdio.h>
#include <string.h>

#include "types.h"
#include "FatFs/ff.h"
#include "UC8156.h"
#include "utils.h"
#include "display_functions.h"
#include "config.h"

/* FatFS only supports 8.3 filenames, and we work from the current directory so
   paths should be short... */
#define MAX_PATH_LEN 64

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
		abort_now("Fatal error in: slideshow.c -> slideshow_run -> f_opendir");

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

		show_image_from_SDcard(full_path, mode);
        mdelay(delay_ms);

	} while (result >= 0);
}

