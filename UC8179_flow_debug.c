/*
 * UC8179_flow_debug.c
 *
 *  Created on: 21.04.2021
 *      Author: zhongke.dai
 */


#include <stdlib.h>
#include <stdio.h>
#include <msp430.h>
#include <time.h>
#include <sys/timespec.h>
#include <sys/_timeval.h>

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
#include "config_display_type.h"



#define MAX_PATH_LEN 64
char path[64];
bool UC8179_sd_exist = true;
extern char PATH[64]; //global variable
extern u8 *image_data;
extern u16 GATE_LINES; //global variable
extern u16 SOURCE_LINES; //global variable
extern bool display_KWR;


char full_path[MAX_PATH_LEN];
enum UC8179_KWR_TEST {SHOW_BLACK=0x00, SHOW_WHITE=0x01, SHOW_RED = 0x02, SHOW_IMAGE = 0x03};

void UC8179_basic_flow()
{
    display_KWR = true;
    set_display_type(S041_T1_1);
    int show_BW = SHOW_WHITE;
    if(show_BW == SHOW_IMAGE || show_BW == SHOW_BLACK)
    {
        display_KWR = false;
    }

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




//    UC8179_TRES_PARAMETER(0x02, 0x98, 0x01, 0xE0 );      // TRES  DKE: 0x02, 0x88, 0x01, 0xE0
//
//    UC8179_GSST_PARAMETER(0x00, 0x10, 0x00, 0x00 );      // GSST  DKE: 0x00, 0x10, 0x00, 0x00



    if(display_KWR)
    {
        switch(show_BW)
            {
                case SHOW_BLACK:
                    UC8179_image_BLACK();
                    UC8179_image_BLACK2();
                    UC8179_POWER_ON();
                    UC8179_BUSY_N_CHECK();
                    UC8179_DISPLAY_REFRESH();
                    UC8179_BUSY_N_CHECK();
                    UC8179_POWER_OFF();
                    UC8179_BUSY_N_CHECK();


                    break;
                case SHOW_WHITE:
//                    UC8179_TRES_PARAMETER(0x00, 0x68, 0x01, 0xE0 );      // TRES  DKE: 0x02, 0x88, 0x01, 0xE0
//                    UC8179_GSST_PARAMETER(0x01, 0x00, 0x00, 0x00 );      // GSST  DKE: 0x00, 0x10, 0x00, 0x00
                   UC8179_image_WHITE();
                    UC8179_image_BLACK2();

//                    UC8171_image_WHITE();
//                    UC8171_image_BLACK2();
                    UC8179_POWER_ON();
                    UC8179_BUSY_N_CHECK();
                    UC8179_DISPLAY_REFRESH();
                    UC8179_BUSY_N_CHECK();
                    UC8179_POWER_OFF();
                    UC8179_BUSY_N_CHECK();

                    break;
                case SHOW_RED:
                    UC8179_image_BLACK();
                    UC8179_image_WHITE2();
                    UC8179_POWER_ON();
                    UC8179_BUSY_N_CHECK();
                    UC8179_DISPLAY_REFRESH();
                    UC8179_BUSY_N_CHECK();
                    UC8179_POWER_OFF();
                    UC8179_BUSY_N_CHECK();

                    break;
            }
    }
    else
    {
        switch(show_BW)
                {
                    case SHOW_BLACK:

                        UC8179_TRES_PARAMETER(0x00, 0x68, 0x01, 0xE0 );      // TRES  DKE: 0x02, 0x88, 0x01, 0xE0
                        UC8179_GSST_PARAMETER(0x00, 0x30, 0x00, 0x00 );      // GSST  DKE: 0x00, 0x10, 0x00, 0x00
                        UC8171_image_BLACK2();

                        UC8179_POWER_ON();
                        UC8179_BUSY_N_CHECK();
                        UC8179_DISPLAY_REFRESH();
                        UC8179_BUSY_N_CHECK();
                        UC8179_POWER_OFF();
                        UC8179_BUSY_N_CHECK();

                        break;
                    case SHOW_WHITE:

                        UC8179_image_WHITE2();

                        UC8179_POWER_ON();
                        UC8179_BUSY_N_CHECK();
                        UC8179_DISPLAY_REFRESH();
                        UC8179_BUSY_N_CHECK();
                        UC8179_POWER_OFF();
                        UC8179_BUSY_N_CHECK();
                        break;
                    case SHOW_IMAGE:
                        UC8179_TRES_PARAMETER(0x00, 0x68, 0x01, 0xE0 );      // TRES  DKE: 0x02, 0x88, 0x01, 0xE0
                        UC8179_GSST_PARAMETER(0x01, 0x00, 0x00, 0x00 );      // GSST  DKE: 0x00, 0x10, 0x00, 0x00
                        UC8171_image_update_S041_flash();
                        break;
                    default:
                        UC8179_image_WHITE2();

                        UC8179_POWER_ON();
                        UC8179_BUSY_N_CHECK();
                        UC8179_DISPLAY_REFRESH();
                        UC8179_BUSY_N_CHECK();
                        UC8179_POWER_OFF();
                        UC8179_BUSY_N_CHECK();
                        break;
                }
    }





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


void UC8179_TIME(void)
{


    clock_t t1, t2;
       t1 = clock();
       mdelay(500);

       t2 = clock();

       float diff = ((float)(t2 - t1) / CLOCKS_PER_SEC ) * 1000;
       printf("%f",diff);

}


void UC8171_basic_flow()
{
    UC8171_ini();
    mdelay(500);
    u8 revID = UC8171_READ_REVID();

    printf("RevID = %x\n", revID);
}







char *UC8171_image_path(char *file_name, char *file_path)
{

    DIR dir;
    char* image_path =  malloc(MAX_PATH_LEN);;
    char path_bg[MAX_PATH_LEN];
    char full_path_bg[MAX_PATH_LEN];


    sprintf(path_bg, "/%s/%s", PATH, file_path);


    if (f_opendir(&dir, path_bg) != FR_OK)
        abort_now("Fatal error in: slideshow.c -> slideshow_run -> f_opendir", ABORT_SD_CARD);


    sprintf(full_path_bg, "%s/%s", path_bg, file_name);
    int i;
            for (i = 0; i<MAX_PATH_LEN; i++ )
            {
                image_path [i] = full_path_bg[i];
            }

    return image_path;

}


void UC8171_image_update(char *file_name, char *file_path)
{
    char *image_path = UC8171_image_path(file_name, file_path);
    sdcard_load_image(image_path, image_data);
    int count = GATE_LINES * SOURCE_LINES/8;
    UC8179_byte_array_WRITE2(image_data, count);


     UC8179_POWER_ON();
     UC8179_BUSY_N_CHECK();
     UC8179_DISPLAY_REFRESH();
     UC8179_BUSY_N_CHECK();

     UC8179_POWER_OFF();
     UC8179_BUSY_N_CHECK();
}


void UC8171_image_update_S041_flash()
{
    int count = GATE_LINES * SOURCE_LINES/8;
    int i = 0;
    for(i = 0; i < count; i++)
    {
        image_data[i] = my_image[i+10];
    }




    UC8179_byte_array_WRITE2(image_data, count);


     UC8179_POWER_ON();
     UC8179_BUSY_N_CHECK();
     UC8179_DISPLAY_REFRESH();
     UC8179_BUSY_N_CHECK();
     UC8179_PTOUT_PARAMETER();
     UC8179_POWER_OFF();
     UC8179_BUSY_N_CHECK();
}




void UC8171_image_clear(void)
{

     UC8171_image_WHITE2();
     UC8179_POWER_ON();
     UC8179_BUSY_N_CHECK();
     UC8179_DISPLAY_REFRESH();
     UC8179_BUSY_N_CHECK();
     UC8179_PTOUT_PARAMETER();
     UC8179_POWER_OFF();
     UC8179_BUSY_N_CHECK();
}




void UC8179_image_BW_clear(void)
{

     UC8179_image_WHITE2();
     UC8179_POWER_ON();
     UC8179_BUSY_N_CHECK();
     UC8179_DISPLAY_REFRESH();
     UC8179_BUSY_N_CHECK();

     UC8179_POWER_OFF();
     UC8179_BUSY_N_CHECK();
}




void UC8179_image_BW_black(void)
{

     UC8179_image_BLACK2();
     UC8179_POWER_ON();
     UC8179_BUSY_N_CHECK();
     UC8179_DISPLAY_REFRESH();
     UC8179_BUSY_N_CHECK();

     UC8179_POWER_OFF();
     UC8179_BUSY_N_CHECK();
}








void UC8171_image_black(void)
{

     UC8171_image_BLACK2();
     UC8179_POWER_ON();
     UC8179_BUSY_N_CHECK();
     UC8179_DISPLAY_REFRESH();
     UC8179_BUSY_N_CHECK();

     UC8179_POWER_OFF();
     UC8179_BUSY_N_CHECK();
}


void UC8171_ini(void)
{


    set_display_type(S041_T1_1);


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
        UC8171_MANUAL_INI();



        UC8171_image_clear();
        int i = 0;
       do{
          // UC8171_image_black();
           UC8171_image_clear();
           UC8171_image_update_S041_flash();
          //  UC8171_image_update("UC8171_1.pgm", "img");
           //UC8171_image_update("UC8171_2.pgm", "img");
           i++;
           printf("i = %x\n", i);
       }
       while(1);
}


void UC8179_BW_TEST(void)
{
      display_KWR = false;
      set_display_type(S041_T1_1);
      UC8179_ini();
      mdelay(500);
//      UC8179_CDI_PARAMETER( 0x19, 0x07 );       // N2OCP = 1: Copy NEW data to OLD data Enabled
//
//      bool partial_update = false;
//      if(partial_update)    // partial update
//      {
//          UC8179_PTL_PARAMETER(0x00, 0x00, 0x00, 0x00, 0x00,0x00,0x00,0x00,0x00 );      // TRES  DKE: 0x02, 0x88, 0x01, 0xE0
//
//          UC8171_image_update_S041_flash_partial();
//
//      }
//      else              // full update
//      {
//
//          UC8179_TRES_PARAMETER(0x02, 0x88, 0x01, 0xE0 );      // TRES  DKE: 0x02, 0x88, 0x01, 0xE0
//          UC8179_GSST_PARAMETER(0x00, 0x10, 0x00, 0x00 );      // GSST  DKE: 0x00, 0x10, 0x00, 0x00
//          UC8171_image_update_S041_flash();
//      }
      int show_BW = SHOW_IMAGE;
//      UC8179_TRES_PARAMETER(0x02, 0x88, 0x01, 0xE0 );      // TRES  DKE: 0x02, 0x88, 0x01, 0xE0
//      UC8179_GSST_PARAMETER(0x00, 0x00, 0x00, 0x00 );      // GSST  DKE: 0x00, 0x10, 0x00, 0x00
      UC8179_PTL_PARAMETER(0x00, 0x30, 0x00, 0x97, 0x00, 0x00, 0x01, 0xE0, 0x01);
      UC8179_PTIN_PARAMETER();
      bool PTL_flag = UC8179_GET_STATUS(PTL_FLAG);

      printf("%s\n", PTL_flag ? "true" : "false");
      switch(show_BW)
                      {
                          case SHOW_BLACK:



                               UC8171_image_BLACK2();
                                     UC8179_POWER_ON();
                                     UC8179_BUSY_N_CHECK();
                                     UC8179_DISPLAY_REFRESH();
                                     UC8179_BUSY_N_CHECK();
                                     UC8179_PTOUT_PARAMETER();
                                     UC8179_POWER_OFF();
                                     UC8179_BUSY_N_CHECK();


                              break;
                          case SHOW_WHITE:

//                              UC8179_image_WHITE2();
//                              UC8179_POWER_ON();
//                              UC8179_BUSY_N_CHECK();
//                              UC8179_DISPLAY_REFRESH();
//                              UC8179_BUSY_N_CHECK();
//                              UC8179_PTOUT_PARAMETER();
//                              UC8179_POWER_OFF();
//                              UC8179_BUSY_N_CHECK();

                            // UC8171_image_WHITE2();
                              UC8171_image_clear();
                              break;
                          case SHOW_IMAGE:
                              UC8171_image_update_S041_flash();
                              break;


                          default:
                              UC8171_image_WHITE2();
                            //  UC8179_image_WHITE2();
                              break;
                      }
//      do{
//          UC8179_PTIN_PARAMETER();
//          UC8171_image_update_S041_flash();
//          UC8179_PTIN_PARAMETER();
//          UC8171_image_clear();
//
//      } while(1);
//      UC8179_POWER_ON();
//      UC8179_BUSY_N_CHECK();
//      UC8179_DISPLAY_REFRESH();
//      UC8179_BUSY_N_CHECK();
//      UC8179_PTOUT_PARAMETER();
//      UC8179_POWER_OFF();
//      UC8179_BUSY_N_CHECK();

}




void UC8171_image_update_S041_flash_partial()
{
    int count = GATE_LINES * SOURCE_LINES/8;
    int i = 0;
    for(i = 0; i < count; i++)
    {
        image_data[i] = my_image[i+10];
    }

    UC8179_PTIN_PARAMETER();
    UC8179_byte_array_WRITE2(image_data, count);

     UC8179_POWER_ON();
     UC8179_BUSY_N_CHECK();
     UC8179_DISPLAY_REFRESH();
     UC8179_BUSY_N_CHECK();
     UC8179_PTOUT_PARAMETER();
     UC8179_POWER_OFF();
     UC8179_BUSY_N_CHECK();
}







