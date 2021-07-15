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
 * main_basic-flow.c
 *
 *  Created on: 19 Feb 2021
 *      Author: Zhongke Dai
 */

#include <stdlib.h>
#include <stdio.h>
#include <msp430.h>
#include "types.h"
#include "UC8156.h"
#include "FatFs/ff.h"
#include "display_functions.h"
#include "read-sd.h"
#include "slideshow.h"
#include "config_display_type.h"
#include "UC8156_MTP.h"
#include "slideshow_tricolor.h"
#include "flow_basis.h"
#include "image.h"
#include "image_acep.h"
#include "msp430/msp430-spi.h"

u8 waveform_from_file1[WAVEFORM_LENGTH];
u8 waveform_from_file2[WAVEFORM_LENGTH];
u8 waveform_from_file_4GL[WAVEFORM_LENGTH];
int current_vcom = 0;
bool sd_exist = true;
extern u8 UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM;
extern char PATH[64]; //global variable
enum DISPLAY_TYPE display_type;
char path1[64];
char path2[64];
char path_GL[64];
char path[64];

#define MAX_PATH_LEN 64
char full_path[MAX_PATH_LEN];
char *all_file_path[MAX_PATH_LEN];

enum image_acep_type {LEGIO_BW=0x00, LEGIO_Y=0x01, LEGIO_G=0x02, LEGIO_R=0x03, LEGIO_B=0x04 };
enum imgae_invert_type { NORMAL = false, INVERT = true};
enum image_clear{EPD_LGRAY = 0x00, EPD_WHITE = 0x01, EPD_BLACK = 0x02 , EPD_HGRAY = 0x03 };
enum waveform_place{WF_SD =0x00, WF_FLASH= 0x01 };

void basic_flow(int display_color)
{
    display_type = read_display_type_from_MTP(); // find the type of the display
    if (display_type == UNKNOWN)
        {
            // 2nd try to read display-type from SD-Card
            display_type = sdcard_read_display_type("display-type.txt");
            if (display_type == UNKNOWN)
                // finally: set display-type to default (1.38'')
                display_type = S014_T1_1;

        }


    set_display_type(display_type);             // display tp by tricolor is 2.1

    UC8156_wait_for_BUSY_inactive(); // wait for power-up completed

    // optional: additional hardware reset after 5ms delay
    mdelay(5);
    UC8156_hardware_reset(); // UC8156 hardware reset
    UC8156_wait_for_BUSY_inactive(); // wait for RESET completed

    // optional -> verifies successful power-up
    UC8156_check_RevID();

    UC8156_init_registers(); // over-writes some register values with display-specific values

    // optional -> check for possible problems
    UC8156_check_status_register(0x00);
    sprintf(path2, "/%s/%s", PATH, "display/S021_T1.1_SPP0B9_V0.uc8156_type2");
    sprintf(path1, "/%s/%s", PATH, "display/S021_T1.1_SPP0B9_V0.uc8156_type1");
    sprintf(path_GL, "/%s/%s", PATH, "display/waveform.bin");


    u8 current_vcom_u8 = print_current_VCOM();   // get the Vcom from display
    current_vcom = current_vcom_u8 * 30;        // set the Vcom, unit: mV

    register_vcom_set();

    path_find();
if( sd_exist)
{
    UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM = WAVEFORM_FROM_LUT; // waveform read from sd-card, 2 waveforms for tricolor

    switch(display_color)
    {
    case 0:
        tricolor_yellow(WF_FLASH);                      // for tricolor yellow
        break;
    case 1:
        tricolor_red(FULL_UPDATE, WF_FLASH);         // for tricolor red
        break;
    case 2:
        tricolor_binary();
        break;
    case 3:
        //HTS_test();
        AO_Test();
        break;
    case 4:
        tricolor_display_with_4GL_waveform();
        break;


    }




}
else
{
    image_ini_test();
}
}

void waveform_type1_function(void)
{
    if (sdcard_load_waveform(path1, waveform_from_file1, WAVEFORM_LENGTH)) // BBepdcULD -set_waveform /mnt/data/PL_TEST_SOFTWARE/lib/UC8156/S021_T1.1_SPP0B9_V0.uc8156_type1
    {
        UC8156_send_waveform(waveform_from_file1);
        //       UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM =  WAVEFORM_FROM_LUT;
    }
}

void waveform_type2_function(void)
{
    if (sdcard_load_waveform(path2, waveform_from_file2, WAVEFORM_LENGTH))
    {
        UC8156_send_waveform(waveform_from_file2);

    }
}




void register_vcom_set(void)
{
    drive_voltage_setting(0x25, 0xff);
    tcom_timing_setting(0x67, 0x55);
    UC8156_set_Vcom(current_vcom);

}

void path_find(void)
{
    DIR dir;
    FILINFO fno;

    sprintf(path, "/%s/%s", PATH, "img");
    bool pgm_find = false;
    if (f_opendir(&dir, path) != FR_OK)
    {

        sd_exist = false;
    }

        //        abort_now("Fatal error in: slideshow.c -> slideshow_run -> f_opendir",
//                  ABORT_SD_CARD);
if( sd_exist)
{  do
    {
        if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0)
            break;

        /* skip directories */
        if ((fno.fname[0] == '.') || (fno.fattrib & AM_DIR))
            continue;

        /* .. and files without the PGM extension */
        if (!strstr(fno.fname, ".PGM") && !strstr(fno.fname, ".pgm"))
            continue;
        else
        {
            pgm_find = true;
        }

        sprintf(full_path, "%s/%s", path, fno.fname);
    }
    while (!pgm_find);
}

}

void tricolor_yellow(u8 waveform_place)
{
    unsigned long counter = 0;
    const int vcom_yellow = 0;


    clear_display();
    // slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL15.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL15.png 0 0 4

    waveformType1_choose(waveform_place);
    slideshow_tricolor_run_GL(FULL_UPDATE, 4, full_path, 15);    // 15 for GL15

    waveformType2_choose(waveform_place);
    // slideshow_tricolor_run(FULL_UPDATE, 2 ,"GL15.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL15.png 0 0 2
    slideshow_tricolor_run_GL(FULL_UPDATE, 2, full_path, 15);

    //waveform_type1_function();
    waveformType1_choose(waveform_place);
    //slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
    slideshow_tricolor_run_GL(FULL_UPDATE, 4, full_path, 0);

    // # Load Waveform to do the shaky thing and load normal waveform aftwerwards
   // waveform_type1_function();
    waveformType1_choose(waveform_place);
    // slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
    slideshow_tricolor_run_GL(FULL_UPDATE, 4, full_path, 0);

  //  waveform_type2_function();
    waveformType2_choose(waveform_place);
    //  slideshow_tricolor_run(FULL_UPDATE, 2 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 2
    slideshow_tricolor_run_GL(FULL_UPDATE, 2, full_path, 0);

    UC8156_set_Vcom(current_vcom);

    //waveform_type1_function();
    waveformType1_choose(waveform_place);
    //   slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
    slideshow_tricolor_run_GL(FULL_UPDATE, 8, full_path, 0);

    drive_voltage_setting(0x25, 0x66);

    //# Update Null Frames
    while (counter < 5)
    {
        UC8156_set_Vcom(vcom_yellow);

        //waveform_type2_function();
        waveformType2_choose(waveform_place);
        //  slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL4.pgm", false);
        slideshow_tricolor_run_GL(FULL_UPDATE, 8, full_path, 4);

        UC8156_set_Vcom(current_vcom);
       // waveform_type1_function();
        waveformType1_choose(waveform_place);
        //  slideshow_tricolor_run(FULL_UPDATE, 1,"GL0.pgm", false);
        slideshow_tricolor_run_GL(FULL_UPDATE, 1, full_path, 0);

        counter++;
    }

    // # Set Vcom to 0v and Waveform to drive Null Frames
    UC8156_set_Vcom(vcom_yellow);
    //waveform_type2_function();
    waveformType2_choose(waveform_place);
    //    slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL4.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL4.png 0 0 8
    slideshow_tricolor_run_GL(FULL_UPDATE, 8, full_path, 4);

    // #Begin Black and White Update

    //drive_voltage_setting(0x25, 0xff);
    tcom_timing_setting(0x67, 0x55);
    UC8156_set_Vcom(current_vcom);

    //waveform_type1_function();
    waveformType1_choose(waveform_place);
    slideshow_tricolor_run(FULL_UPDATE, 10, full_path, false); // false for original, true for inverse
    //waveform_type2_function();
    waveformType2_choose(waveform_place);
    slideshow_tricolor_run(FULL_UPDATE, 3, full_path, false); // false for original, true for inverse
    //waveform_type1_function();
    waveformType1_choose(waveform_place);
    slideshow_tricolor_run(FULL_UPDATE, 40, full_path, true); // false for original, true for inverse
}

void tricolor_red(int mode, u8 waveform_place)
{
    static const int vcom_int_red = 5800;
    unsigned long counter = 0;
    // # Load Waveform to do the shaky thing and load normal waveform aftwerwards
    //slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
 //   waveform_type1_function();
    waveformType1_choose(waveform_place);
    clear_display();
    slideshow_tricolor_run_GL(mode, 4, full_path, 0);

   // waveform_type2_function();
    waveformType2_choose(waveform_place);
    //slideshow_tricolor_run(FULL_UPDATE, 2 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 2
    slideshow_tricolor_run_GL(mode, 2, full_path, 0);

    //waveform_type1_function();
    waveformType1_choose(waveform_place);
    slideshow_tricolor_run_GL(mode, 4, full_path, 0); // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
    // slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4

    //     # Update Null Frames

    while (counter < 5)
    {
        UC8156_set_Vcom(vcom_int_red);
        drive_voltage_setting(0x25, 0x00);

        //slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL15.pgm", false);
        slideshow_tricolor_run_GL(mode, 8, full_path, 15);

        UC8156_set_Vcom(current_vcom);
        drive_voltage_setting(0x25, 0xff);

        //slideshow_tricolor_run(FULL_UPDATE, 1,"GL0.pgm", false);
        slideshow_tricolor_run_GL(mode, 1, full_path, 0);

        counter++;
    }

    // # Set Vcom to 0v and Waveform to drive Null Frames
    drive_voltage_setting(0x25, 0x00);
    UC8156_set_Vcom(vcom_int_red);

    //waveform_type1_function();
    waveformType1_choose(waveform_place);
    //slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL15.pgm", false);
    slideshow_tricolor_run_GL(mode, 8, full_path, 15);

    // #Begin Black and White Update
    drive_voltage_setting(0x25, 0xff);

    tcom_timing_setting(0x67, 0x55);
    UC8156_set_Vcom(current_vcom);

    //waveform_type1_function();
    waveformType1_choose(waveform_place);
    slideshow_tricolor_run(mode, 4, full_path, false); // false for original, true for inverse
    //waveform_type2_function();
    waveformType2_choose(waveform_place);
    slideshow_tricolor_run(mode, 2, full_path, false); // false for original, true for inverse

   // waveform_type1_function();
    waveformType1_choose(waveform_place);
    //invertieren hier
    //  slideshow_tricolor_run(FULL_UPDATE, 20 ,"tricolor_yellow_inv.pgm", false);
    slideshow_tricolor_run(mode, 4, full_path, true); // false for original, true for inverse

    /*
     waveform_type1_function();
     slideshow_tricolor_run_GL(mode, 4 ,full_path, 0);
     waveform_type2_function();
     slideshow_tricolor_run_GL(mode, 2 ,full_path, 0);

     waveform_type1_function();

     //invertieren hier
     //  slideshow_tricolor_run(FULL_UPDATE, 20 ,"tricolor_yellow_inv.pgm", false);
     slideshow_tricolor_run_GL(mode, 4 ,full_path, 15);
     */

}

void tricolor_red_path(void)
{
    static const int vcom_int_red = 5800;
    unsigned long counter = 0;
    // # Load Waveform to do the shaky thing and load normal waveform aftwerwards
    //slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
    waveform_type1_function();

    //slideshow_tricolor_run_GL(FULL_UPDATE, 4 ,full_path, 0);

    slideshow_tricolor_run_path(FULL_UPDATE, 4, "GL0.pgm", "background", false);
    waveform_type2_function();

    //slideshow_tricolor_run(FULL_UPDATE, 2 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 2
    //  slideshow_tricolor_run_GL(FULL_UPDATE, 2 ,full_path, 0);
    slideshow_tricolor_run_path(FULL_UPDATE, 2, "GL0.pgm", "background", false);
    waveform_type1_function();

    // slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
    slideshow_tricolor_run_GL(FULL_UPDATE, 4, full_path, 0);
    //     # Update Null Frames

    while (counter < 5)
    {
        UC8156_set_Vcom(vcom_int_red);
        drive_voltage_setting(0x25, 0x00);

        //slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL15.pgm", false);
        // slideshow_tricolor_run_GL(FULL_UPDATE, 8 ,full_path, 15);
        slideshow_tricolor_run_path(FULL_UPDATE, 8, "GL15.pgm", "background",
                                    false);

        UC8156_set_Vcom(current_vcom);
        drive_voltage_setting(0x25, 0xff);

        //slideshow_tricolor_run(FULL_UPDATE, 1,"GL0.pgm", false);
        //slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);
        slideshow_tricolor_run_path(FULL_UPDATE, 1, "GL0.pgm", "background",
                                    false);
        counter++;
    }

    // # Set Vcom to 0v and Waveform to drive Null Frames
    drive_voltage_setting(0x25, 0x00);
    UC8156_set_Vcom(vcom_int_red);

    waveform_type1_function();

    //slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL15.pgm", false);
    // slideshow_tricolor_run_GL(FULL_UPDATE, 8 ,full_path, 15);
    slideshow_tricolor_run_path(FULL_UPDATE, 8, "GL15.pgm", "background",
                                false);

    // #Begin Black and White Update
    drive_voltage_setting(0x25, 0xff);

    tcom_timing_setting(0x67, 0x55);
    UC8156_set_Vcom(current_vcom);

    waveform_type1_function();
    slideshow_tricolor_run(FULL_UPDATE, 4, full_path, false); // false for original, true for inverse
    waveform_type2_function();
    slideshow_tricolor_run(FULL_UPDATE, 2, full_path, false); // false for original, true for inverse
    waveform_type1_function();
    //invertieren hier
    //  slideshow_tricolor_run(FULL_UPDATE, 20 ,"tricolor_yellow_inv.pgm", false);
    slideshow_tricolor_run(FULL_UPDATE, 20, full_path, true); // false for original, true for inverse
}

void tricolor_yellow_path(void)
{
    unsigned long counter = 0;
    const int vcom_yellow = 0;
    waveform_type1_function();

    // slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL15.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL15.png 0 0 4
    // slideshow_tricolor_run_GL(FULL_UPDATE, 4 ,full_path, 15);    // 15 for GL15
    slideshow_tricolor_run_path(FULL_UPDATE, 4, "GL15.pgm", "background",
                                false);
    waveform_type2_function();

    // slideshow_tricolor_run(FULL_UPDATE, 2 ,"GL15.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL15.png 0 0 2
//   slideshow_tricolor_run_GL(FULL_UPDATE, 2 ,full_path, 15);
    slideshow_tricolor_run_path(FULL_UPDATE, 2, "GL15.pgm", "background",
                                false);
    waveform_type1_function();

    //slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
    // slideshow_tricolor_run_GL(FULL_UPDATE, 4 ,full_path, 0);
    slideshow_tricolor_run_path(FULL_UPDATE, 4, "GL0.pgm", "background", false);
    // # Load Waveform to do the shaky thing and load normal waveform aftwerwards
    waveform_type1_function();

    // slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
    // slideshow_tricolor_run_GL(FULL_UPDATE, 4 ,full_path, 0);
    slideshow_tricolor_run_path(FULL_UPDATE, 4, "GL0.pgm", "background", false);

    waveform_type2_function();

    //  slideshow_tricolor_run(FULL_UPDATE, 2 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 2
    //slideshow_tricolor_run_GL(FULL_UPDATE, 2 ,full_path, 0);
    slideshow_tricolor_run_path(FULL_UPDATE, 2, "GL0.pgm", "background", false);

    waveform_type1_function();
//   slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
    // slideshow_tricolor_run_GL(FULL_UPDATE, 4 ,full_path, 0);
    slideshow_tricolor_run_path(FULL_UPDATE, 4, "GL0.pgm", "background", false);
    //# Update Null Frames
    while (counter < 5)
    {
        UC8156_set_Vcom(vcom_yellow);

        waveform_type2_function();
        //  slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL4.pgm", false);
        //   slideshow_tricolor_run_GL(FULL_UPDATE, 8 ,full_path, 4);
        slideshow_tricolor_run_path(FULL_UPDATE, 10, "GL4.pgm", "background",
                                    false);

        UC8156_set_Vcom(current_vcom);
        waveform_type1_function();
        //  slideshow_tricolor_run(FULL_UPDATE, 1,"GL0.pgm", false);
        // slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);
        slideshow_tricolor_run_path(FULL_UPDATE, 1, "GL0.pgm", "background",
                                    false);
        counter++;
    }

    // # Set Vcom to 0v and Waveform to drive Null Frames
    UC8156_set_Vcom(vcom_yellow);
    waveform_type2_function();
//    slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL4.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL4.png 0 0 4
    //  slideshow_tricolor_run_GL(FULL_UPDATE, 4 ,full_path, 4);
    slideshow_tricolor_run_path(FULL_UPDATE, 10, "GL4.pgm", "background",
                                false);
    // #Begin Black and White Update

    drive_voltage_setting(0x25, 0xff);
    tcom_timing_setting(0x67, 0x55);
    UC8156_set_Vcom(current_vcom);

    waveform_type1_function();
    slideshow_tricolor_run(FULL_UPDATE, 4, full_path, false); // false for original, true for inverse
    waveform_type2_function();
    slideshow_tricolor_run(FULL_UPDATE, 2, full_path, false); // false for original, true for inverse
    waveform_type1_function();
    slideshow_tricolor_run(FULL_UPDATE, 20, full_path, true); // false for original, true for inverse

}

void tricolor_binary(void)
{

    const int count = 5;
    clear_display();
// slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL15.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL15.png 0 0 4
    clear_display();
    waveform_type1_function();
//slideshow_tricolor_run_GL(FULL_UPDATE, count ,full_path, 0);

    while (1)
    {

        slideshow_run_all(FULL_UPDATE, 3000, count);
    }

}

void slideshow_run_all(int mode, u16 delay_ms, int count)
{

    DIR dir;
    FILINFO fno;
    char path_all[MAX_PATH_LEN];
    char full_path[MAX_PATH_LEN];
    unsigned int result = 0;

    sprintf(path_all, "/%s/%s", PATH, "img_test");

    if (f_opendir(&dir, path_all) != FR_OK)
        abort_now("Fatal error in: slideshow.c -> slideshow_run -> f_opendir",
                  ABORT_SD_CARD);

    do
    {
        if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0)
            break;

        // skip directories
        if ((fno.fname[0] == '.') || (fno.fattrib & AM_DIR))
            continue;

        //.. and files without the PGM extension
        if (!strstr(fno.fname, ".PGM") && !strstr(fno.fname, ".pgm"))
            continue;

        sprintf(full_path, "%s/%s", path_all, fno.fname);

        slideshow_tricolor_run(mode, count, full_path, false); // false for original, true for inverse
        mdelay(delay_ms);
    }
    while (result >= 0);

}

void tricolor_red_partial(int mode)
{
    static const int vcom_int_red = 5500;
    unsigned long counter = 0;
    // # Load Waveform to do the shaky thing and load normal waveform aftwerwards
    //slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
    waveform_type1_function();
    clear_display();
    slideshow_tricolor_run_GL(mode, 4, full_path, 0);

    // slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);
    waveform_type2_function();

    //slideshow_tricolor_run(FULL_UPDATE, 2 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 2
    slideshow_tricolor_run_GL(mode, 2, full_path, 0);
    //  slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);

    waveform_type1_function();

    slideshow_tricolor_run_GL(mode, 2, full_path, 0); // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
    //   slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);
    // slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4

    //     # Update Null Frames

    while (counter < 4)
    {
        UC8156_set_Vcom(vcom_int_red);
        drive_voltage_setting(0x25, 0x00);

        //slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL15.pgm", false);
        slideshow_tricolor_run_GL(FULL_UPDATE, 8, full_path, 15);
        //  slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 15);

        UC8156_set_Vcom(current_vcom);
        drive_voltage_setting(0x25, 0xff);

        //slideshow_tricolor_run(FULL_UPDATE, 1,"GL0.pgm", false);
        slideshow_tricolor_run_GL(FULL_UPDATE, 1, full_path, 0);
        //    slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);

        counter++;
    }

    // # Set Vcom to 0v and Waveform to drive Null Frames
    drive_voltage_setting(0x25, 0x00);
    UC8156_set_Vcom(vcom_int_red);

    waveform_type1_function();

    //slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL15.pgm", false);
    slideshow_tricolor_run_GL(FULL_UPDATE, 8, full_path, 15);

    // #Begin Black and White Update
    drive_voltage_setting(0x25, 0xff);

    tcom_timing_setting(0x67, 0x55);
    UC8156_set_Vcom(current_vcom);

    waveform_type1_function();

    slideshow_tricolor_run_path(mode, 4, "tricolor.pgm", "partial", false); // false for original, true for inverse
    slideshow_tricolor_run_path(FULL_UPDATE, 1, "tricolor.pgm", "partial",
                                false);  // false for original, true for inverse
    waveform_type2_function();
    slideshow_tricolor_run_path(mode, 2, "tricolor.pgm", "partial", false); // false for original, true for inverse
    slideshow_tricolor_run_path(FULL_UPDATE, 1, "tricolor.pgm", "partial",
                                false);  // false for original, true for inverse
    waveform_type1_function();

    //invertieren hier
    //  slideshow_tricolor_run(FULL_UPDATE, 20 ,"tricolor_yellow_inv.pgm", false);
    slideshow_tricolor_run_path(mode, 4, "tricolor.pgm", "partial", true); // false for original, true for inverse
    slideshow_tricolor_run_path(FULL_UPDATE, 1, "tricolor.pgm", "partial",
                                true);  // false for original, true for inverse

    mdelay(2000);

    waveform_type1_function();
    /*
     slideshow_tricolor_run_path(PARTIAL_UPDATE, 4 ,"tricolor_nach.pgm", "partial", false);
     slideshow_tricolor_run_path(FULL_UPDATE, 1 ,"tricolor_nach.pgm", "partial", false);
     waveform_type2_function();
     slideshow_tricolor_run_path(PARTIAL_UPDATE, 2 ,"tricolor_nach.pgm", "partial", false);
     slideshow_tricolor_run_path(FULL_UPDATE, 1 ,"tricolor_nach.pgm", "partial", false);
     waveform_type1_function();
     */
    //      slideshow_tricolor_run_path(PARTIAL_UPDATE, 3 ,"tricolor_nach.pgm", "partial", true);
    slideshow_tricolor_run_path(FULL_UPDATE, 4, "tricolor_nach.pgm", "partial",
                                true);

    mdelay(2000);

    slideshow_tricolor_run_path(FULL_UPDATE, 4, "tricolor.pgm", "partial",
                                true);  // false for original, true for inverse

    mdelay(2000);
    waveform_type2_function();
    slideshow_tricolor_run_path(PARTIAL_UPDATE, 4, "tricolor_nach.pgm",
                                "partial", true);

    waveform_type1_function();
    slideshow_tricolor_run_path(FULL_UPDATE, 1, "tricolor_nach.pgm", "partial",
                                true);
    //  waveform_type1_function();
    slideshow_tricolor_run_path(PARTIAL_UPDATE, 4, "tricolor_nach.pgm",
                                "partial", true);
    slideshow_tricolor_run_path(FULL_UPDATE, 4, "tricolor_nach.pgm", "partial",
                                true);


}

void tricolor_red_all(int mode)
{
    static const int vcom_int_red = 5100;
    unsigned long counter = 0;
    // # Load Waveform to do the shaky thing and load normal waveform aftwerwards
    //slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
    waveform_type1_function();
    clear_display();
    slideshow_tricolor_run_GL(mode, 4, full_path, 0);

    // slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);
    waveform_type2_function();

    //slideshow_tricolor_run(FULL_UPDATE, 2 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 2
    slideshow_tricolor_run_GL(mode, 2, full_path, 0);
    //  slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);

    waveform_type1_function();

    slideshow_tricolor_run_GL(mode, 4, full_path, 0); // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
    //   slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);
    // slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4

    //     # Update Null Frames

    while (counter < 4)
    {
        UC8156_set_Vcom(vcom_int_red);
        drive_voltage_setting(0x25, 0x00);

        //slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL15.pgm", false);
        slideshow_tricolor_run_GL(FULL_UPDATE, 8, full_path, 15);
        //  slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 15);

        UC8156_set_Vcom(current_vcom);
        drive_voltage_setting(0x25, 0xff);

        //slideshow_tricolor_run(FULL_UPDATE, 1,"GL0.pgm", false);
        slideshow_tricolor_run_GL(FULL_UPDATE, 1, full_path, 0);
        //    slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);

        counter++;
    }

    // # Set Vcom to 0v and Waveform to drive Null Frames
    drive_voltage_setting(0x25, 0x00);
    UC8156_set_Vcom(vcom_int_red);

    waveform_type1_function();

    //slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL15.pgm", false);
    slideshow_tricolor_run_GL(FULL_UPDATE, 8, full_path, 15);

    // #Begin Black and White Update
    drive_voltage_setting(0x25, 0xff);

    tcom_timing_setting(0x67, 0x55);
    UC8156_set_Vcom(current_vcom);

    waveform_type1_function();

    slideshow_tricolor_run_path(mode, 4, "tricolor.pgm", "partial", false); // false for original, true for inverse
    slideshow_tricolor_run_path(FULL_UPDATE, 1, "tricolor.pgm", "partial",
                                false);  // false for original, true for inverse
    waveform_type2_function();
    slideshow_tricolor_run_path(mode, 2, "tricolor.pgm", "partial", false); // false for original, true for inverse
    slideshow_tricolor_run_path(FULL_UPDATE, 1, "tricolor.pgm", "partial",
                                false);  // false for original, true for inverse
    waveform_type1_function();

    //invertieren hier
    //  slideshow_tricolor_run(FULL_UPDATE, 20 ,"tricolor_yellow_inv.pgm", false);
    slideshow_tricolor_run_path(mode, 4, "tricolor.pgm", "partial", true); // false for original, true for inverse
    slideshow_tricolor_run_path(FULL_UPDATE, 1, "tricolor.pgm", "partial",
                                true);  // false for original, true for inverse

    mdelay(2000);

    waveform_type1_function();

    slideshow_tricolor_run_path(FULL_UPDATE, 4, "tricolor_nach.pgm", "partial",
                                true);

    mdelay(2000);

    slideshow_tricolor_run_path(FULL_UPDATE, 4, "tricolor.pgm", "partial",
                                true);  // false for original, true for inverse

    // clear again

    mdelay(2000);
    waveform_type2_function();

    /*
     slideshow_tricolor_run_all(FULL_UPDATE, 3 ,"tricolor_transparency_black.pgm", "partial",  TRANSPARENCY_GS3, TRANSPARENCY_DISPLAY_ENABLE_ON , FULL_DISPLAY_UPDATE,   true);
     waveform_type1_function();

     slideshow_tricolor_run_all(FULL_UPDATE, 4 ,"tricolor_transparency_black.pgm", "partial",  TRANSPARENCY_GS3, TRANSPARENCY_DISPLAY_ENABLE_ON , FULL_DISPLAY_UPDATE,   true);
     */
    slideshow_tricolor_run_all(FULL_UPDATE, 3, "tricolor_nach.pgm", "partial",
                               TRANSPARENCY_GS3, TRANSPARENCY_DISPLAY_ENABLE_ON,
                               FULL_DISPLAY_UPDATE, true);
    waveform_type1_function();

    slideshow_tricolor_run_all(FULL_UPDATE, 4, "tricolor_nach.pgm", "partial",
                               TRANSPARENCY_GS3, TRANSPARENCY_DISPLAY_ENABLE_ON,
                               FULL_DISPLAY_UPDATE, true);

}

void tricolor_red_opti(int mode)
{
    static const int vcom_int_red = 5500;
    unsigned long counter = 0;
    // # Load Waveform to do the shaky thing and load normal waveform aftwerwards

   // waveform_type1_function();
    waveform_type1_flash_function();
    clear_display();
    slideshow_tricolor_run_GL(mode, 4, full_path, 0);


   // waveform_type2_function();
    waveform_type2_flash_function();


    slideshow_tricolor_run_GL(mode, 4, full_path, 0);


   // waveform_type1_function();
    waveform_type1_flash_function();

    slideshow_tricolor_run_GL(mode, 4, full_path, 0); // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4


    //     # Update Null Frames

    while (counter < 4)
    {
        UC8156_set_Vcom(vcom_int_red);
        drive_voltage_setting(0x25, 0x00);


        slideshow_tricolor_run_GL(FULL_UPDATE, 8, full_path, 15);


        UC8156_set_Vcom(current_vcom);
        drive_voltage_setting(0x25, 0xff);


        slideshow_tricolor_run_GL(FULL_UPDATE, 1, full_path, 0);


        counter++;
    }

    // # Set Vcom to 0v and Waveform to drive Null Frames
    drive_voltage_setting(0x25, 0x00);
    UC8156_set_Vcom(vcom_int_red);

    //waveform_type1_function();
    waveform_type1_flash_function();

    slideshow_tricolor_run_GL(FULL_UPDATE, 8, full_path, 15);

    // #Begin Black and White Update
    drive_voltage_setting(0x25, 0xff);

    tcom_timing_setting(0x67, 0x55);
    UC8156_set_Vcom(current_vcom);

    //waveform_type1_function();
    waveform_type1_flash_function();

    slideshow_tricolor_run_path(mode, 4, "pricetag.pgm", "partial", false); // false for original, true for inverse
    slideshow_tricolor_run_path(FULL_UPDATE, 1, "pricetag.pgm", "partial",
                                false);  // false for original, true for inverse
    //waveform_type2_function();
    waveform_type2_flash_function();
    slideshow_tricolor_run_path(mode, 2, "pricetag.pgm", "partial", false); // false for original, true for inverse
    slideshow_tricolor_run_path(FULL_UPDATE, 1, "pricetag.pgm", "partial",
                                false);  // false for original, true for inverse
    //waveform_type1_function();
    waveform_type1_flash_function();

    slideshow_tricolor_run_path(mode, 4, "pricetag.pgm", "partial", true); // false for original, true for inverse
    slideshow_tricolor_run_path(FULL_UPDATE, 1, "pricetag.pgm", "partial",
                                true);  // false for original, true for inverse

}

void tricolor_yellow_test(void)
{
    unsigned int counter = 0;
    u16 test_u16 = 0x187;
    u8 test_nach = (u8)test_u16;
    u8 test_vor = (u8)((test_nach >> 8  )&& 0x03 );


//    waveform_type1_function();
//   // UC8156_set_Vcom(7500);
//    //slideshow_tricolor_run_GL(FULL_UPDATE, 20, full_path, 0);
//
//waveform_type1_function();
//slideshow_tricolor_run_GL(FULL_UPDATE, 20, full_path, 0);
//
//
//          // 15 for GL15
//
//           waveform_type2_function();
//
//           // slideshow_tricolor_run(FULL_UPDATE, 2 ,"GL15.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL15.png 0 0 2
//          // slideshow_tricolor_run_GL(FULL_UPDATE, 4, full_path, 15);
//           slideshow_tricolor_run_all(FULL_UPDATE, 4 ,"GL0.pgm", "background",  TRANSPARENCY_GS3, TRANSPARENCY_DISPLAY_ENABLE_ON , FULL_DISPLAY_UPDATE,  false);
//           waveform_type1_function();
//
//           //slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
//           slideshow_tricolor_run_GL(FULL_UPDATE, 20, full_path, 15);

          // slideshow_tricolor_run_GL(FULL_UPDATE, 4, full_path, 15);

           // # Load Waveform to do the shaky thing and load normal waveform aftwerwards
        //   waveform_type1_function();




    // slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4

//    int v, blackLoop = 5;
//    for (v = 0; v < blackLoop; ++v)
//    {
//        slideshow_tricolor_run_GL(FULL_UPDATE, 4, full_path, 0);
//
//        slideshow_tricolor_run_GL(FULL_UPDATE, 8, full_path, 15);
//    }

   // waveform_type2_function();

    //  slideshow_tricolor_run(FULL_UPDATE, 2 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 2
   // slideshow_tricolor_run_GL(FULL_UPDATE, 5, full_path, 0);

    drive_voltage_setting(0x25, 0x66);
    UC8156_set_Vcom(current_vcom);
    waveform_type1_function();
    slideshow_tricolor_run_GL(FULL_UPDATE, 2, full_path, 0);
    waveform_type2_function();
    drive_voltage_setting(0x25, 0x66);
    slideshow_tricolor_run_GL(FULL_UPDATE, 5, full_path, 0);
    waveform_type1_function();
    slideshow_tricolor_run_GL(FULL_UPDATE, 2, full_path, 15);

   /*
    //# Update Null Frames
    while (counter < 4)
    {
        UC8156_set_Vcom(0);

        waveform_type2_function();
        //  slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL4.pgm", false);
        slideshow_tricolor_run_GL(FULL_UPDATE, 8, full_path, 4);

      //  UC8156_set_Vcom(current_vcom);
        UC8156_set_Vcom(3500);
        waveform_type1_function();
        //  slideshow_tricolor_run(FULL_UPDATE, 1,"GL0.pgm", false);
        slideshow_tricolor_run_GL(FULL_UPDATE, 1, full_path, 0);

        counter++;
    }
*/
    /*
     // # Set Vcom to 0v and Waveform to drive Null Frames
     UC8156_set_Vcom(vcom_yellow);
     waveform_type2_function();
     //    slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL4.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL4.png 0 0 8
     slideshow_tricolor_run_GL(FULL_UPDATE, 8 ,full_path, 4);

     // #Begin Black and White Update

     drive_voltage_setting(0x25, 0xff);
     tcom_timing_setting(0x67, 0x55);
     UC8156_set_Vcom(current_vcom);


     waveform_type1_function();
     slideshow_tricolor_run(FULL_UPDATE, 4 ,full_path, false); // false for original, true for inverse
     waveform_type2_function();
     slideshow_tricolor_run(FULL_UPDATE, 2 ,full_path, false); // false for original, true for inverse
     waveform_type1_function();
     slideshow_tricolor_run(FULL_UPDATE, 20 ,full_path, true);  // false for original, true for inverse
     */
}



void image_ini_test(void)
{

      u8 current_vcom_u8 = print_current_VCOM();   // get the Vcom from display
      current_vcom = current_vcom_u8 * 30;        // set the Vcom, unit: mV

      register_vcom_set();
    UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM = WAVEFORM_FROM_MTP; // waveform read from sd-card, 2 waveforms for tricolor
    clear_display();


      UC8156_show_image_all_set( Legio_V1_y, FULL_UPDATE, NORMAL_4GL, TRANSPARENCY_GS0, TRANSPARENCY_DISPLAY_ENABLE_OFF, FULL_DISPLAY_UPDATE,  false);

}





void HTS_test(void)
{


      u8 current_vcom_u8 = print_current_VCOM();   // get the Vcom from display
      current_vcom = current_vcom_u8 * 30;        // set the Vcom, unit: mV

      drive_voltage_setting(0x70, 0xff);
      tcom_timing_setting(0x67, 0x55);
      UC8156_set_Vcom(current_vcom_u8);

      UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM = WAVEFORM_FROM_MTP; // waveform read from sd-card, 2 waveforms for tricolor
      clear_display();


      slideshow_tricolor_run_GL(FULL_UPDATE, 5, full_path, 15);

      slideshow_tricolor_run_GL(FULL_UPDATE, 5, full_path, 11);
      slideshow_tricolor_run_GL(FULL_UPDATE, 5, full_path, 15);
    //  slideshow_tricolor_run_GL(FULL_UPDATE, 5, full_path, 0);
}



void AO_Test(void)
{
    u8 current_vcom_u8 = print_current_VCOM();   // get the Vcom from display
      current_vcom = current_vcom_u8 * 30;        // set the Vcom, unit: mV

     // drive_voltage_setting(0x70, 0xff);          // original

      drive_voltage_setting(0x25, 0xff);            // parameter from Arduino
      tcom_timing_setting(0x67, 0x55);
      UC8156_set_Vcom(current_vcom_u8);

      UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM = WAVEFORM_FROM_MTP; // waveform read from sd-card, 2 waveforms for tricolor
      clear_display();
      slideshow_tricolor_run_GL(FULL_UPDATE, 1, full_path, 15);
      mdelay(1000);
      slideshow_tricolor_run_GL(FULL_UPDATE, 1, full_path, 0);
}











void image_acep(void)
{
    display_type = read_display_type_from_MTP(); // find the type of the display
      set_display_type(display_type);             // display tp by tricolor is 2.1

      UC8156_wait_for_BUSY_inactive(); // wait for power-up completed

      // optional: additional hardware reset after 5ms delay
      mdelay(5);
      UC8156_hardware_reset(); // UC8156 hardware reset
      UC8156_wait_for_BUSY_inactive(); // wait for RESET completed

      // optional -> verifies successful power-up
      UC8156_check_RevID();

      UC8156_init_registers(); // over-writes some register values with display-specific values
      spi_write_command_4params(0x18, 0x00, 0x00,0x24, 0x07); //TPCOM=Hi-Z before update and during null-frame drive
      spi_write_command_2params(0x04, 0x22, 0x17); // Register setting for BoostSetting
      spi_write_command_1param(0x14, 0x03); // Register setting for BoostSetting
      // optional -> check for possible problems
      UC8156_check_status_register(0x00);

    UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM = WAVEFORM_FROM_MTP; // waveform read from sd-card, 2 waveforms for tricolor
    image_acep_WhiteErase(Legio_V1_bw);

    image_acep_update(Legio_V1_bw, LEGIO_BW);
//    image_acep_update(Legio_V1_y, LEGIO_Y);
//    image_acep_update(Legio_V1_g, LEGIO_G);
    image_acep_update(Legio_V1_r, LEGIO_R);
//    image_acep_update(Legio_V1_b, LEGIO_Y);
//    image_acep_update(Legio_V1_b, LEGIO_B);


}

void image_acep_update(u8 *image_buffer, u8 image_type)
{
  switch(image_type)
   {
       case LEGIO_BW:    // image_bw EPD BLACK
           UC8156_set_Vcom_Acep(0);
           drive_voltage_setting_Acep(13000);

           int i;
           for(i = 0; i < 2; i++)
           {
               Write_Previous_Buffer(image_buffer, EPD_LGRAY);
               UC8156_show_image_all_set( image_buffer, FULL_UPDATE, NORMAL_4GL, TRANSPARENCY_GS1, TRANSPARENCY_DISPLAY_ENABLE_OFF, FULL_DISPLAY_UPDATE,  INVERT);
               mdelay(1);
           }

           for(i = 0; i < 2; i++)
           {
               Write_Previous_Buffer(image_buffer, EPD_LGRAY);
               UC8156_show_image_all_set( image_buffer, FULL_UPDATE, NORMAL_4GL, TRANSPARENCY_GS1, TRANSPARENCY_DISPLAY_ENABLE_OFF, FULL_DISPLAY_UPDATE,  NORMAL);
               mdelay(1);
           }

           break;




       case LEGIO_Y:    // image_y    EPD YELLOW
           UC8156_set_Vcom_Acep(15000);                 // setTPCOM(15000)
           drive_voltage_setting_Acep(15000);           // setSourceVoltage(15000);

           Write_Previous_Buffer(image_buffer, EPD_LGRAY);

           UC8156_show_image_all_set( image_buffer, FULL_UPDATE, NORMAL_4GL, TRANSPARENCY_GS1, TRANSPARENCY_DISPLAY_ENABLE_OFF, FULL_DISPLAY_UPDATE,  INVERT);
           break;



       case LEGIO_G:    // image_g    EPD GREEN
           UC8156_set_Vcom_Acep(0);
           drive_voltage_setting_Acep(9000);
           Write_Previous_Buffer(image_buffer, EPD_WHITE);
//           int i_g;
//           for(i_g =0; i_g< 15; i_g++)
//           {
//               UC8156_show_image_all_set( image_buffer, FULL_UPDATE, NORMAL_4GL, TRANSPARENCY_GS3, TRANSPARENCY_DISPLAY_ENABLE_ON, FULL_DISPLAY_UPDATE,  NORMAL);
//           mdelay(1);
//           }
           UC8156_show_image_all_set( image_buffer, FULL_UPDATE, NORMAL_4GL, TRANSPARENCY_GS1, TRANSPARENCY_DISPLAY_ENABLE_OFF, FULL_DISPLAY_UPDATE,  NORMAL);
           //           mdelay(1);
           break;



       case LEGIO_R:    // image_r    EPD RED

           drive_voltage_setting_Acep(15000);           // setSourceVoltage(15000);
           int i_r;
           for(i_r = 0; i_r < 1; i_r++)
           {
           Write_Previous_Buffer(image_buffer, EPD_LGRAY);
           UC8156_set_Vcom_Acep(-15000);

           spi_write_command_1param(0x14, 0x03); // Register setting for Display Engine Control Register// setTPCOM(-15000)
           spi_write_command_1param(0x40, 0x02); // waveform Type2

          UC8156_show_image_all_set( image_buffer, FULL_UPDATE, FAST_2GL, TRANSPARENCY_GS0, TRANSPARENCY_DISPLAY_ENABLE_OFF, FULL_DISPLAY_UPDATE,  INVERT);
           mdelay(10);


           Write_Previous_Buffer(image_buffer, EPD_HGRAY);

           UC8156_set_Vcom_Acep(15000);
           drive_voltage_setting_Acep(15000);
           spi_write_command_1param(0x40, 0x02); // waveform Type2

           UC8156_show_image_all_set( image_buffer, FULL_UPDATE, FAST_2GL, TRANSPARENCY_GS0, TRANSPARENCY_DISPLAY_ENABLE_OFF, FULL_DISPLAY_UPDATE,  INVERT);
           mdelay(10);

           }

//           Write_Previous_Buffer(image_buffer, EPD_LGRAY);
//
//
//           drive_voltage_setting_Acep(8000);           // setSourceVoltage(8000);
//           UC8156_set_Vcom_Acep(-8000);                 // setTPCOM(-8000)
//
//           spi_write_command_1param(0x40, 0x02); // waveform Type2
//
//           UC8156_show_image_all_set( image_buffer, FULL_UPDATE, FAST_2GL, TRANSPARENCY_GS1, TRANSPARENCY_DISPLAY_ENABLE_OFF, FULL_DISPLAY_UPDATE,  NORMAL);
//            mdelay(1);


           break;



       case LEGIO_B:    // image_b    EPD BLUE
            ;
           int k;
           for(k = 0; k < 1; k++)
           {
               Write_Previous_Buffer(image_buffer, EPD_LGRAY);

               drive_voltage_setting_Acep(12000);           // setSourceVoltage(8000);
               UC8156_set_Vcom_Acep(-12000);

               spi_write_command_1param(0x40, 0x02); // Register setting for Program WS MTP
               spi_write_command_1param(0x14, 0x03); // Register setting for Display Engine Control Register// setTPCOM(-15000)

               UC8156_show_image_all_set( image_buffer, FULL_UPDATE, FAST_2GL, TRANSPARENCY_GS0, TRANSPARENCY_DISPLAY_ENABLE_OFF, FULL_DISPLAY_UPDATE,  NORMAL);
               mdelay(1);

               Write_Previous_Buffer(image_buffer, EPD_LGRAY);
               drive_voltage_setting_Acep(12000);           // setSourceVoltage(8000);
               UC8156_set_Vcom_Acep(12000);

               UC8156_show_image_all_set( image_buffer, FULL_UPDATE, FAST_2GL, TRANSPARENCY_GS0, TRANSPARENCY_DISPLAY_ENABLE_OFF, FULL_DISPLAY_UPDATE,  INVERT);
               mdelay(500);

           }
           drive_voltage_setting_Acep(12000);           // setSourceVoltage(8000);
           UC8156_set_Vcom_Acep(-12000);

           int j;
           for (j = 0; j < 2; j++)
           {
               Write_Previous_Buffer(image_buffer, EPD_LGRAY);
               UC8156_show_image_all_set( image_buffer, FULL_UPDATE, FAST_2GL, TRANSPARENCY_GS0, TRANSPARENCY_DISPLAY_ENABLE_OFF, FULL_DISPLAY_UPDATE,  NORMAL);

               mdelay(1);
           }
           break;
 }

}


void image_acep_WhiteErase(u8 *image_buffer)
{


      int i;
      for(i = 0; i < 3; i++)
      {
          UC8156_set_Vcom_Acep(12000);

          UC8156_show_image_GL(image_buffer, FULL_UPDATE, NORMAL_4GL, 15);  //clear(WHITE), buffer = 0xff
          mdelay(1);

          UC8156_set_Vcom_Acep(-12000);
          UC8156_show_image_GL(image_buffer, FULL_UPDATE, NORMAL_4GL, 0);   //clear(BLACK), case 0: EPD_BLACK
          mdelay(1);
      }

//      UC8156_set_Vcom_Acep(-15000);
//
//      UC8156_show_image_GL(image_buffer, FULL_UPDATE, NORMAL_4GL, 0);
//      UC8156_show_image_GL(image_buffer, FULL_UPDATE, NORMAL_4GL, 0);
}




void Data_Entry_Mode_Setting(bool previous)
{
    if(previous)
    {
        spi_write_command_1param(0x0F, 0x30);
    }
    else
    {
        spi_write_command_1param(0x0F, 0x20);
    }
}

void Write_Previous_Buffer(u8 *image_buffer, u8 EDP_Clear_TYPE)
{
    Data_Entry_Mode_Setting(true);
    if(EDP_Clear_TYPE == EPD_LGRAY)         // Buffer = 0xAA
    {
        UC8156_send_image_data_GL11(image_buffer);
    }
    if(EDP_Clear_TYPE == EPD_WHITE)
    {
        UC8156_send_image_data_GL15(image_buffer);
    }
    if(EDP_Clear_TYPE == EPD_BLACK)
    {
        UC8156_send_image_data_GL0(image_buffer);
    }
    if(EDP_Clear_TYPE == EPD_HGRAY)
    {
        UC8156_send_image_data_GL4(image_buffer);
    }
    Data_Entry_Mode_Setting(false);
}



void waveform_type1_flash_function(void)
{
        UC8156_send_waveform(waveformType1);

}

void waveform_type2_flash_function(void)
{
        UC8156_send_waveform(waveformType2);
}

void tricolor_display_with_4GL_waveform(void)
{
    int vcom_mv_value;
    if (sdcard_load_vcom(&vcom_mv_value))
        UC8156_set_Vcom(vcom_mv_value);
    sprintf(path_GL, "/%s/%s", PATH, "display/waveform.bin");

                if (sdcard_load_waveform(path_GL, waveform_from_file_4GL, WAVEFORM_LENGTH))
                {
                    UC8156_send_waveform(waveform_from_file_4GL);
                    UC8156_send_waveform_slave(waveform_from_file_4GL);
                    UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM =  WAVEFORM_FROM_LUT;
                   // UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM =  WAVEFORM_FROM_MTP;
                }
                clear_display();
                slideshow_run(FULL_UPDATE, 2000);
}

void waveformType1_choose(u8 waveform_place)
{
    if(waveform_place == WF_SD)
    {
        waveform_type1_function();
    }
    else
    {
        waveform_type1_flash_function();
    }
}

void waveformType2_choose(u8 waveform_place)
{
    if(waveform_place == WF_SD)
    {
        waveform_type2_function();
    }
    else
    {
        waveform_type2_flash_function();
    }
}

//void red_color_test(u8 *image_buffer)
//{
//    UC8156_HVs_on();
//    drive_voltage_setting_Acep(15000);           // setSourceVoltage(15000);
//    Write_Previous_Buffer(image_buffer, EPD_LGRAY);
//    UC8156_set_Vcom_Acep(-15000);
//    //   UC8156_show_image_all_set( image_buffer, FULL_UPDATE, FAST_2GL, TRANSPARENCY_GS0, TRANSPARENCY_DISPLAY_ENABLE_OFF, FULL_DISPLAY_UPDATE,  NORMAL);
//    // UC8156_show_image_all_set(u8 *image_data, u8 update_mode, u8 waveform_mode, u8 transparency_key_value, u8 transparency_display_enable, u8 display_mode_select, bool inv_enable)
//    // UC8156_update_display_with_power_on_off_all_set(u8 update_mode, u8 waveform_mode, u8 transparency_key_value, u8 transparency_display_enable, u8 display_mode_select)
//   // UC8156_update_display_all_set(update_mode, waveform_mode, transparency_key_value, transparency_display_enable, display_mode_select);
//
//    spi_write_command_1param(0x14, 0x03); // Register setting for Display Engine Control Register// setTPCOM(-15000)
//    spi_write_command_1param(0x40, 0x02); // waveform Type2
//
//
//    //  update(EPD_UPD_MONO, true)
//    UC8156_update_display_all_set(FULL_UPDATE, FAST_2GL, TRANSPARENCY_GS0, TRANSPARENCY_DISPLAY_ENABLE_OFF, NORMAL);
//    UC8156_send_image_data(image_data);
//
//
//
//
//
//
//
//
//
//
//
//    UC8156_HVs_off();
//}
