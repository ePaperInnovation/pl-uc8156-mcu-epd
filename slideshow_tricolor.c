/*
 * slideshow_tricolor.c
 *
 *  Created on: 15.02.2021
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
#include "slideshow_tricolor.h"
#define MAX_PATH_LEN 64
extern char PATH[64]; //global variable

/** Slideshow, calling show_image for each pgm-file found in a directory */
void slideshow_tricolor_run(int mode, u16 loop_count, char *file_name, bool inv_bool )     // false for original, true for inverse
{
    /*
    DIR dir;
//    FILINFO fno;
    char path[MAX_PATH_LEN];
    char full_path[MAX_PATH_LEN];


    sprintf(path, "/%s/%s", PATH, "img");


    if (f_opendir(&dir, path) != FR_OK)
        abort_now("Fatal error in: slideshow.c -> slideshow_run -> f_opendir", ABORT_SD_CARD);
*/
    int result = 0;

   do {
        /*

        if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0)
            break;

        // skip directories
        if ((fno.fname[0] == '.') || (fno.fattrib & AM_DIR))
            continue;

        // .. and files without the PGM extension
        if (!strstr(fno.fname, ".PGM") && !strstr(fno.fname, ".pgm"))
            continue;
*/
       // sprintf(full_path, "%s/%s", path, file_name);
        show_image_from_SDcard_inv(file_name, mode, inv_bool);

    //   mdelay(10);
        result++;
   } while (result < loop_count);
}





void slideshow_tricolor_run_GL(int mode, u16 loop_count, char *file_name, int GL_name )
{

    /*
    DIR dir;
//    FILINFO fno;
    char path[MAX_PATH_LEN];
    char full_path[MAX_PATH_LEN];


    sprintf(path, "/%s/%s", PATH, "img");


    if (f_opendir(&dir, path) != FR_OK)
        abort_now("Fatal error in: slideshow.c -> slideshow_run -> f_opendir", ABORT_SD_CARD);
        */
    int result = 0;
   do {
        /*

        if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0)
            break;

        // skip directories
        if ((fno.fname[0] == '.') || (fno.fattrib & AM_DIR))
            continue;

        // .. and files without the PGM extension
        if (!strstr(fno.fname, ".PGM") && !strstr(fno.fname, ".pgm"))
            continue;
*/
       // sprintf(full_path, "%s/%s", path, file_name);
        show_image_from_SDcard_GL(file_name, mode, GL_name);

    //   mdelay(10);
        result++;
   } while (result < loop_count);
}




void slideshow_tricolor_run_path(int mode, int count, char *file_name, char *file_path, bool inv_bool )
{

    DIR dir;

    char path_bg[MAX_PATH_LEN];
    char full_path_bg[MAX_PATH_LEN];


    sprintf(path_bg, "/%s/%s", PATH, file_path);


    if (f_opendir(&dir, path_bg) != FR_OK)
        abort_now("Fatal error in: slideshow.c -> slideshow_run -> f_opendir", ABORT_SD_CARD);

    int count_ini = 0;
    sprintf(full_path_bg, "%s/%s", path_bg, file_name);
   do {

       show_image_from_SDcard_inv(full_path_bg, mode, inv_bool );
        count_ini++;
   } while (count_ini <  count);
}



void slideshow_tricolor_run_all(int mode, int count, char *file_name, char *file_path,   u8 transparency_key_value, u8 transparency_display_enable, u8 display_mode_select  ,bool inv_bool )
{

    DIR dir;

    char path_bg[MAX_PATH_LEN];
    char full_path_bg[MAX_PATH_LEN];


    sprintf(path_bg, "/%s/%s", PATH, file_path);


    if (f_opendir(&dir, path_bg) != FR_OK)
        abort_now("Fatal error in: slideshow.c -> slideshow_run -> f_opendir", ABORT_SD_CARD);

    int count_ini = 0;
    sprintf(full_path_bg, "%s/%s", path_bg, file_name);
   do {


       show_image_from_SDcard_all_set(full_path_bg, mode, transparency_key_value, transparency_display_enable, display_mode_select, inv_bool );    // show_image_from_SDcard_all_set(char *image, int mode,  u8 transparency_key_value, u8 transparency_display_enable, u8 display_mode_select, bool inv_bool);

        count_ini++;
   } while (count_ini <  count);
}

