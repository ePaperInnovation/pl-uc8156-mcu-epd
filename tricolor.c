/*
 * tricolor.c
 * for Test, same function as slideshow
 *  Created on: 12.02.2021
 *      Author: zhongke.dai
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
void tricolor_run(int mode, u16 delay_ms)
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
