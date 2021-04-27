/*
 * UC8179_flow_debug.c
 *
 *  Created on: 21.04.2021
 *      Author: zhongke.dai
 */


#include <stdlib.h>
#include <stdio.h>
#include <msp430.h>


#include <UC8156.h>
#include <UC8156_MTP.h>
#include <UC8179.h>
#include <UC8179_MTP.h>
#include "types.h"
#include "FatFs/ff.h"
#include "display_functions.h"
#include "read-sd.h"
#include "slideshow.h"
#include "config_display_type.h"
#include "slideshow_tricolor.h"
#include "UC8179_flow_debug.h"
#include "image.h"

#include "msp430/msp430-spi.h"

#include "msp430-defs.h"
#include "msp430-spi.h"
#include "msp430-gpio.h"



#define MAX_PATH_LEN 64
char path[64];
bool UC9179_sd_exist = true;
extern char PATH[64]; //global variable
char full_path[MAX_PATH_LEN];

void UC8179_basic_flow()
{

    UC8179_ini();
    u8 revID = UC8179_READ_REVID();

    printf("RevID = %x\n", revID);

    u8 *proct_revID = UC8179_READ_PRODUCT_REVISION();
    printf("proct_revID1 = %d\n", proct_revID[0]);
    printf("proct_revID2 = %d\n", proct_revID[1]);
    printf("proct_revID3 = %c\n", proct_revID[2]);


    u8 vcom_value = UC8179_VCOM_VALUE_READ();
    printf("vcom_value = %d\n", vcom_value);


//    char *lut_revision = UC8179_READ_LUT_REVION();
//      printf("lut_revision = %x\n", lut_revision);

}



void UC8179_path_find(void)
{
    DIR dir;
    FILINFO fno;

    sprintf(path, "/%s/%s", PATH, "img");
    bool pbm_find = false;
    if (f_opendir(&dir, path) != FR_OK)
    {

        UC9179_sd_exist = false;
    }

        //        abort_now("Fatal error in: slideshow.c -> slideshow_run -> f_opendir",
//                  ABORT_SD_CARD);
if( UC9179_sd_exist)
{  do
    {
        if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0)
            break;

        /* skip directories */
        if ((fno.fname[0] == '.') || (fno.fattrib & AM_DIR))
            continue;

        /* .. and files without the PGM extension */
        if (!strstr(fno.fname, ".PBM") && !strstr(fno.fname, ".pbm"))
            continue;
        else
        {
            pbm_find = true;
        }

        sprintf(full_path, "%s/%s", path, fno.fname);
    }
    while (!pbm_find);
}

}

void UC8179_ini(void)
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
        UC8179_KW_MODE();
        UC8179_POWER_ON();
}


void UC8179_KW_MODE(void)
{
    UC8179_PANEL_SETTING( LUT_FROM_OTP, PIXEL_KW_MODE,GATE_SCAN_UP,SOURCE_SHIFT_RIGHT,BOOSTER_ON, SOFT_RESET_NO_EFFECT);
    UC8179_POWER_SETTING(BORDER_LDO_DISABLE, INTERNAL_DC_FOR_VDHR, INTERNAL_DC_FOR_VDHL,INTERNAL_DC_FOR_VGHL, SLEW_RATE_FAST,VGHL_20V, VDH_LVL,VDL_LVL,VDHR_LVL );
    UC8179_BOOSTER_SOFT_START_DEFAULT();
    UC8179_PLL_CONTROL(PLL_CONTROL_FRAME_RATE_50HZ);
    UC8179_TEMPERATURE_SENSOR_ENABLE();
}


void UC8179_KWR_MODE(void)
{
    UC8179_PANEL_SETTING_DEFAULT();
    UC8179_POWER_SETTING_DEFAULT();
    UC8179_BOOSTER_SOFT_START_DEFAULT();
    UC8179_PLL_CONTROL(PLL_CONTROL_FRAME_RATE_50HZ);
    UC8179_TEMPERATURE_SENSOR_ENABLE();
}


