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
#include <UC8179.h>
#include <UC8179_MTP.h>
#include "types.h"
#include "FatFs/ff.h"
#include "display_functions.h"
#include "read-sd.h"
#include "slideshow.h"
#include "config_display_type.h"
#include "slideshow_tricolor.h"
#include "flow_basis.h"
#include "image.h"
//#include "image_acep.h"
#include "msp430/msp430-spi.h"

#include "msp430-defs.h"
#include "msp430-spi.h"
#include "msp430-gpio.h"


u8 waveform_from_file1[WAVEFORM_LENGTH];
u8 waveform_from_file2[WAVEFORM_LENGTH];
static int current_vcom = 0;
bool sd_exist = true;
extern u8 UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM;
extern char PATH[64]; //global variable
enum DISPLAY_TYPE display_type;
char path1[64];
char path2[64];
char path[64];

#define MAX_PATH_LEN 64
char full_path[MAX_PATH_LEN];
char *all_file_path[MAX_PATH_LEN];

enum image_acep_type {LEGIO_BW=0x00, LEGIO_Y=0x01, LEGIO_G=0x02, LEGIO_R=0x03, LEGIO_B=0x04 };
enum imgae_invert_type { NORMAL = false, INVERT = true};
enum image_clear{EPD_LGRAY = 0x00, EPD_WHITE = 0x01, EPD_BLACK = 0x02 , EPD_HGRAY = 0x03 };

void basic_flow(int display_color)
{
//    display_type = read_display_type_from_MTP(); // find the type of the display
//    set_display_type(display_type);             // display tp by tricolor is 2.1

   // UC8179_wait_for_BUSY_inactive(); // wait for power-up completed

//    // optional: additional hardware reset after 5ms delay
//    mdelay(5);
//    UC8179_hardware_reset(); // UC8179 hardware reset
//    UC8179_wait_for_BUSY_inactive(); // wait for RESET completed

    uc_8179_ini();

    u8 revID = UC8179_read_RevID();

        printf("RevID = %x\n", revID);



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

void uc_8179_ini(void)
{

        gpio_set_value_lo(PIN_RESET);
        //gpio_set_value_lo(PIN_3V3_ENABLE);
        mdelay(500);
        //gpio_set_value_hi(PIN_3V3_ENABLE);
        gpio_set_value_hi(PIN_RESET);
        mdelay(1);
        udelay(500);
        UC8179_hardware_reset(); // UC8156 hardware reset
        gpio_set_value_hi(SPI_CS);
        mdelay(10);
        UC8179_wait_for_BUSY_inactive(); // wait for RESET completed


}

