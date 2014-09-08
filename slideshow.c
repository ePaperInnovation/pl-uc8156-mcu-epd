#include <stdio.h>
#include <string.h>

#include "FatFs/ff.h"
#include "UC8156.h"
#include "read-sd.h"

/* FatFS only supports 8.3 filenames, and we work from the current directory so
   paths should be short... */
#define MAX_PATH_LEN 64

// loads image form SD-card and updates it on the display using REFRESH waveform
int show_image(const char *image)
{
	  //u8 image_data[240*160/4];
	  u8 image_data[128*180/4];

	  if (sdcard_load_image(image, image_data))
		return -1;

	  UC8156_send_image_data(image_data);
	  UC8156_update_display();

	return 0;
}

/** Slideshow callback function, called on each file found in a directory */
int slideshow_run(const char *path, slideshow_cb_t callback)
{
	DIR dir;
	FILINFO fno;
	char full_path[MAX_PATH_LEN];
	int result = 0;

	if (f_opendir(&dir, path) != FR_OK)
		return -1;

	do {
		if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0)
			break;

		/* skip directories */
		if ((fno.fname[0] == '.') || (fno.fattrib & AM_DIR))
			continue;

		/* .. and files without the PGM extension */
		if (!strstr(fno.fname, ".PGM"))
			continue;

		sprintf(full_path, "%s/%s", path, fno.fname);

		result = callback(full_path);

	} while (result >= 0);

	return result;
}
