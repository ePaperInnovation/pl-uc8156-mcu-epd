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
#include "UC8179_ini.h"



#define MAX_PATH_LEN 64
char path[64];
bool UC8179_sd_exist = true;
extern char PATH[64]; //global variable
char full_path[MAX_PATH_LEN];

void UC8179_basic_flow()
{

    UC8179_ini();
    mdelay(500);

    u8 revID = UC8179_READ_REVID();

    printf("RevID = %x\n", revID);

//    u8 temp_read = UC8179_TEMPERATUR_READ_INTER();
//    printf("temp read = %x\n", temp_read);
    mdelay(500);
    //UC8179_KWR_OTP_Register_TR_SETTING_Value_Read(TR4);





//    UC8179_KWR_OTP_Register_TR_LUTC_Read(TR4);
//    UC8179_KWR_OTP_Register_TR_LUTR_Read(TR4);
//    UC8179_KWR_OTP_Register_TR_LUTW_Read(TR4);
//    UC8179_KWR_OTP_Register_TR_LUTK_Read(TR4);







    bool PTL_flag = UC8179_GET_STATUS(PTL_FLAG);

    printf("%s\n", PTL_flag ? "true" : "false");

//    UC8179_PARTIAL_WINDOW(0, 320, 0 , 320, 1);
//    UC8179_PARTIAL_IN();

         // UC8179_image_BLACK();
          UC8179_image_WHITE();
          UC8179_image_WHITE2();
          //UC8179_image_BLACK2();
          UC8179_BUSY_N_CHECK();
          UC8179_POWER_ON();
         u8 data_finish_flag = UC8179_DATA_FLAG();
         if(data_finish_flag == 1)
         {
             printf("data read finish");
         }
         else
         {
             printf("data read not finish");
         }



         UC8179_DISPLAY_REFRESH();
         UC8179_BUSY_N_CHECK();
         UC8179_POWER_OFF();
         UC8179_BUSY_N_CHECK();
      //   UC8179_PARTIAL_OUT();

}



void UC8179_path_find(void)
{
    DIR dir;
    FILINFO fno;

    sprintf(path, "/%s/%s", PATH, "img");
    bool pbm_find = false;
    if (f_opendir(&dir, path) != FR_OK)
    {

        UC8179_sd_exist = false;
    }

        //        abort_now("Fatal error in: slideshow.c -> slideshow_run -> f_opendir",
//                  ABORT_SD_CARD);
if( UC8179_sd_exist)
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
        mdelay(500);
        UC8179_hardware_reset(); // UC8156 hardware reset
        gpio_set_value_hi(SPI_CS);
        mdelay(10);
        UC8179_wait_for_BUSY_inactive(); // wait for RESET completed
       // UC8179_KWR_MODE();
        UC8179_MANUAL_INI();


}


void UC8179_KW_MODE(void)
{
    UC8179_KW_MODE_OTP();
    UC8179_POWER_SETTING(BORDER_LDO_DISABLE, INTERNAL_DC_FOR_VDHR, INTERNAL_DC_FOR_VDHL,INTERNAL_DC_FOR_VGHL, SLEW_RATE_FAST,VGHL_20V, VDH_LVL,VDL_LVL,VDHR_LVL );
 //   UC8179_TEMPERATURE_SENSOR_ENABLE();

    UC8179_BOOSTER_SOFT_START_DEFAULT();
    UC8179_KW_LUT_OPTION(KW_LUT_ONLY, 0,0,0,0,0,0,0,0,0,0);
    UC8179_PLL_CONTROL(PLL_CONTROL_FRAME_RATE_50HZ);

    UC8179_TCON_SETTING(0x02, 0x02);
    UC8179_RESOLUTION_SETTING(800,600);
    UC8179_GATE_SOURCE_START_SETTING(0,0);
    UC8179_LVD_VOLTAGE_SELECT(LVD_25V);
    UC8179_TEMPERATUR_BOUNDRY_C2();
    UC8179_POWER_SAVING();


}


void UC8179_KWR_MODE(void)
{
   // UC8179_PANEL_SETTING_DEFAULT();
   // UC8179_POWER_SETTING_DEFAULT();
    UC8179_BOOSTER_SOFT_START_DEFAULT();
    UC8179_KW_LUT_OPTION(KWR_LUT_ALWAYS, 0,0,0,0,0,0,0,0,0,0);
    UC8179_PLL_CONTROL(PLL_CONTROL_FRAME_RATE_50HZ);
    UC8179_TCON_SETTING(0x02, 0x02);
    UC8179_RESOLUTION_SETTING(800,600);
    UC8179_GATE_SOURCE_START_SETTING(0,0);
    UC8179_LVD_VOLTAGE_SELECT(LVD_25V);
    UC8179_TEMPERATUR_BOUNDRY_C2();
    UC8179_POWER_SAVING();
}


