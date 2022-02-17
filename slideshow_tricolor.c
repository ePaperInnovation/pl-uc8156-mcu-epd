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
   int result = 0;
   do {
        show_image_from_SDcard_inv(file_name, mode, inv_bool);
        result++;
   } while (result < loop_count);
}


void slideshow_tricolor_memory_run(int mode, u16 loop_count, bool inv_bool )     // false for original, true for inverse
{
   int result = 0;
   do {
        show_image_from_flash(mode, inv_bool);
        result++;
   } while (result < loop_count);
}


void slideshow_tricolor_run_GL(int mode, u16 loop_count, int GL_name )
{
    int result = 0;
   do {
        show_image_from_GL(mode, GL_name);
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

