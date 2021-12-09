/*
 * UC8177C_debug.c
 *
 *  Created on: 01.11.2021
 *      Author: zhongke.dai
 */


#include <stdlib.h>
#include <stdio.h>
#include <msp430.h>
#include <time.h>
#include <sys/timespec.h>
#include <sys/_timeval.h>

#include <UC8177C.h>
#include <UC8177C_MTP.h>
#include "types.h"
#include "FatFs/ff.h"
#include "display_functions.h"
#include "read-sd.h"
#include "slideshow.h"
#include "config_display_type.h"
#include "slideshow_tricolor.h"
#include "UC8177C_flow_debug.h"
#include "image.h"

#include "msp430/msp430-spi.h"

#include "msp430-defs.h"
#include "msp430-spi.h"
#include "msp430-gpio.h"

#include "config_display_type.h"
//#define WAVEFORM_LENGTH_UC8177 61696    // 3856 (0x0F10) * 16


extern u8 *data_buffer;
//extern const int BUFFER_LENGTH = 64;

void UC8177_basic_flow(void)
{


    char PATH[64]; //global variable
   // set_display_type(S021_T1_1);
    strcpy(PATH, "S028_T1.1");

    char path[64];
    char image_path1[64];
    char image_path2[64];
    char path_new[64];
        UC8177_wait_for_BUSY_inactive(); // wait for power-up completed

        // optional: additional hardware reset after 5ms delay
        mdelay(5);
        UC8177_hardware_reset(); // UC8179 hardware reset
        UC8177_wait_for_BUSY_inactive(); // wait for RESET completed

        // optional -> verifies successful power-up
        UC8177_check_RevID();
        UC8177_PON();
        mdelay(5);
        flag_check(0);


       // int VCOM_Setting = 4000;  // for -4 V
       // UC8177_set_Vcom(VCOM_Setting);
        sprintf(path, "/%s/%s", PATH, "display/Eink_S028_16GL.uc8177_lut"); // short: Eink_S028.uc8177_lut; double:  Eink_S028_double.uc8177_lut; 16GL: Eink_S028_16GL.uc8177_lut
      //  sprintf(path_new, "/%s/%s", PATH, "display/Eink_S028_16GL.uc8177_lut"); // short: Eink_S028.uc8177_lut; double:  Eink_S028_double.uc8177_lut; 16GL: Eink_S028_16GL.uc8177_lut
        sprintf(image_path1, "/%s/%s", PATH, "img/LedgerLogo_608x480_Black.pgm");
        sprintf(image_path2, "/%s/%s", PATH, "img/LedgerLogo_608x480_White.pgm");

        /////////////////////////////////////////////////////////////////////////// waveform too big for msp430
//         long WAVEFORM_LENGTH_UC8177 = 61696;
//        u8 waveform_from_file[WAVEFORM_LENGTH_UC8177];                      // maybe buffer here not enough for waveform UC8177, should slide to 2 parts
//        if (sdcard_load_waveform(path, waveform_from_file, WAVEFORM_LENGTH_UC8177))
//            {



        ///////////////////////////////////////////////////////////////////////

        UC8177_Eink_ini();
        data_buffer = (u8 *) malloc(600 /8);
        bool Waveform_read_finish = UC8177_Send_WaveformFile_to_LUTD_static(path);

        printf("%s\n", Waveform_read_finish ? "true" : "false");





//                 UC8177_white_update();
//                 mdelay(5000);
        UC8177_black_update();
//         mdelay(5000);
//         buffer_check();
   do{



       printf("black update start \n");
        UC8177_black_update();


        mdelay(3000);
        printf("white update start \n");
        UC8177_white_update();


        mdelay(3000);
//        UC8177_white_update();
//        mdelay(5000);
//        UC8177_test_update();
//        mdelay(5000);
//      // temp_check();
//       mdelay(2000);
        printf("image1 update start \n");
         UC8177_image_update(image_path1, data_buffer);
               mdelay(3000);

               UC8177_black_update();
               mdelay(3000);
               printf("image2 update start \n");
         UC8177_image_update(image_path2, data_buffer);
                     mdelay(3000);

   }while(1);

}