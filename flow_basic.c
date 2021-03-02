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
    u8 waveform_from_file1[WAVEFORM_LENGTH];
    u8 waveform_from_file2[WAVEFORM_LENGTH];
    int current_vcom = 0;
    extern u8 UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM;
    extern char PATH[64]; //global variable
    enum DISPLAY_TYPE display_type;
    char path1[64];
    char path2[64];
    char path[64];

#define MAX_PATH_LEN 64
    char full_path[MAX_PATH_LEN];
    char *all_file_path[MAX_PATH_LEN];

void basic_flow(int display_color)
{
    display_type = read_display_type_from_MTP();        // find the type of the display
    set_display_type(display_type);                     // display tp by tricolor is 2.1

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
    UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM =  WAVEFORM_FROM_LUT;                             // waveform read from sd-card, 2 waveforms for tricolor


	u8 current_vcom_u8 = print_current_VCOM();   // get the Vcom from display
	current_vcom = current_vcom_u8 * 30;        // set the Vcom, unit: mV

	register_vcom_set();


    path_find();

	  if(display_color == 0)   // choose the type of display, 0: yellow;
	  {
	                    // find the path for the PGM picture.
	     tricolor_yellow();
	  }
	  else if (display_color == 1)  // 1: red
 	  {
	      //tricolor_red_path();           // find the path for the PGM picture.
	     // tricolor_red(FULL_UPDATE);
	    // tricolor_red_partial(PARTIAL_UPDATE);
	     //tricolor_red_partial(FULL_UPDATE);
	     //tricolor_red_all(FULL_UPDATE);
	   //  tricolor_red_opti(FULL_UPDATE);
	      tricolor_yellow_test();
	  }

	  else if (display_color == 2)
	  {
	      tricolor_binary();
	  }

}




void waveform_type1_function(void)
   {
    if (sdcard_load_waveform(path1, waveform_from_file1, WAVEFORM_LENGTH))                                                  // BBepdcULD -set_waveform /mnt/data/PL_TEST_SOFTWARE/lib/UC8156/S021_T1.1_SPP0B9_V0.uc8156_type1
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
             else
             {
                 pgm_find = true;
             }

             sprintf(full_path, "%s/%s", path, fno.fname);
         }while(!pgm_find);
}


void tricolor_yellow(void)
{
    unsigned long counter=0;
    const int vcom_yellow = 0;

    clear_display();
      // slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL15.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL15.png 0 0 4

    waveform_type1_function();

    slideshow_tricolor_run_GL(FULL_UPDATE, 4 ,full_path, 15);    // 15 for GL15

       waveform_type2_function();

      // slideshow_tricolor_run(FULL_UPDATE, 2 ,"GL15.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL15.png 0 0 2
    slideshow_tricolor_run_GL(FULL_UPDATE, 2 ,full_path, 15);

       waveform_type1_function();

       //slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
      slideshow_tricolor_run_GL(FULL_UPDATE, 4 ,full_path, 0);

       // # Load Waveform to do the shaky thing and load normal waveform aftwerwards
       waveform_type1_function();

       // slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
      slideshow_tricolor_run_GL(FULL_UPDATE, 4 ,full_path, 0);




       waveform_type2_function();

       //  slideshow_tricolor_run(FULL_UPDATE, 2 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 2
       slideshow_tricolor_run_GL(FULL_UPDATE, 2 ,full_path, 0);

       UC8156_set_Vcom(current_vcom);

       waveform_type1_function();
    //   slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
      slideshow_tricolor_run_GL(FULL_UPDATE, 8 ,full_path, 0);

       //# Update Null Frames
       while(counter < 7)
        {
        UC8156_set_Vcom(vcom_yellow);

        waveform_type2_function();
      //  slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL4.pgm", false);
     slideshow_tricolor_run_GL(FULL_UPDATE, 8 ,full_path, 4);

        UC8156_set_Vcom(current_vcom);
         waveform_type1_function();
       //  slideshow_tricolor_run(FULL_UPDATE, 1,"GL0.pgm", false);
        slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);

          counter++;
        }


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
}



void tricolor_red(int mode)
{
    static  const  int vcom_int_red = 5800;
    unsigned long counter=0;
    // # Load Waveform to do the shaky thing and load normal waveform aftwerwards
        //slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
         waveform_type1_function();
         clear_display();
         slideshow_tricolor_run_GL(mode, 4 ,full_path, 0);


        waveform_type2_function();

        //slideshow_tricolor_run(FULL_UPDATE, 2 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 2
      slideshow_tricolor_run_GL(mode, 2 ,full_path, 0);

        waveform_type1_function();

        slideshow_tricolor_run_GL(mode, 4 ,full_path, 0);   // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
      // slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4

    //     # Update Null Frames

        while(counter < 5)
             {
             UC8156_set_Vcom(vcom_int_red);
             drive_voltage_setting(0x25, 0x00);

             //slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL15.pgm", false);
            slideshow_tricolor_run_GL(mode, 8 ,full_path, 15);


             UC8156_set_Vcom(current_vcom);
             drive_voltage_setting(0x25, 0xff);

             //slideshow_tricolor_run(FULL_UPDATE, 1,"GL0.pgm", false);
            slideshow_tricolor_run_GL(mode, 1 ,full_path, 0);

             counter++;
             }

        // # Set Vcom to 0v and Waveform to drive Null Frames
        drive_voltage_setting(0x25, 0x00);
        UC8156_set_Vcom(vcom_int_red);

        waveform_type1_function();

        //slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL15.pgm", false);
        slideshow_tricolor_run_GL(mode, 8 ,full_path, 15);




        // #Begin Black and White Update
        drive_voltage_setting(0x25, 0xff);

        tcom_timing_setting(0x67, 0x55);
        UC8156_set_Vcom(current_vcom);


        waveform_type1_function();
        slideshow_tricolor_run(mode, 4 ,full_path, false);  // false for original, true for inverse
        waveform_type2_function();
        slideshow_tricolor_run(mode, 2 ,full_path, false);  // false for original, true for inverse

        waveform_type1_function();

        //invertieren hier
      //  slideshow_tricolor_run(FULL_UPDATE, 20 ,"tricolor_yellow_inv.pgm", false);
        slideshow_tricolor_run(mode, 4 ,full_path, true);  // false for original, true for inverse


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
    static  const  int vcom_int_red = 5800;
        unsigned long counter=0;
        // # Load Waveform to do the shaky thing and load normal waveform aftwerwards
            //slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
             waveform_type1_function();

             //slideshow_tricolor_run_GL(FULL_UPDATE, 4 ,full_path, 0);

             slideshow_tricolor_run_path(FULL_UPDATE, 4 , "GL0.pgm", "background", false);
            waveform_type2_function();

            //slideshow_tricolor_run(FULL_UPDATE, 2 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 2
          //  slideshow_tricolor_run_GL(FULL_UPDATE, 2 ,full_path, 0);
            slideshow_tricolor_run_path(FULL_UPDATE, 2 , "GL0.pgm", "background", false);
            waveform_type1_function();

            // slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
           slideshow_tricolor_run_GL(FULL_UPDATE, 4 ,full_path, 0);
        //     # Update Null Frames

            while(counter < 5)
                 {
                 UC8156_set_Vcom(vcom_int_red);
                 drive_voltage_setting(0x25, 0x00);

                 //slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL15.pgm", false);
                // slideshow_tricolor_run_GL(FULL_UPDATE, 8 ,full_path, 15);
                 slideshow_tricolor_run_path(FULL_UPDATE, 8 , "GL15.pgm", "background", false);


                 UC8156_set_Vcom(current_vcom);
                 drive_voltage_setting(0x25, 0xff);

                 //slideshow_tricolor_run(FULL_UPDATE, 1,"GL0.pgm", false);
                 //slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);
                 slideshow_tricolor_run_path(FULL_UPDATE, 1 , "GL0.pgm", "background", false);
                 counter++;
                 }

            // # Set Vcom to 0v and Waveform to drive Null Frames
            drive_voltage_setting(0x25, 0x00);
            UC8156_set_Vcom(vcom_int_red);

            waveform_type1_function();

            //slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL15.pgm", false);
           // slideshow_tricolor_run_GL(FULL_UPDATE, 8 ,full_path, 15);
            slideshow_tricolor_run_path(FULL_UPDATE, 8 , "GL15.pgm", "background", false);



            // #Begin Black and White Update
            drive_voltage_setting(0x25, 0xff);

            tcom_timing_setting(0x67, 0x55);
            UC8156_set_Vcom(current_vcom);


            waveform_type1_function();
            slideshow_tricolor_run(FULL_UPDATE, 4 ,full_path, false);  // false for original, true for inverse
            waveform_type2_function();
            slideshow_tricolor_run(FULL_UPDATE, 2 ,full_path, false);  // false for original, true for inverse
            waveform_type1_function();
            //invertieren hier
          //  slideshow_tricolor_run(FULL_UPDATE, 20 ,"tricolor_yellow_inv.pgm", false);
            slideshow_tricolor_run(FULL_UPDATE, 20 ,full_path, true);  // false for original, true for inverse
}


void tricolor_yellow_path(void)
{
    unsigned long counter=0;
    const int vcom_yellow = 0;
    waveform_type1_function();

  // slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL15.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL15.png 0 0 4
  // slideshow_tricolor_run_GL(FULL_UPDATE, 4 ,full_path, 15);    // 15 for GL15
   slideshow_tricolor_run_path(FULL_UPDATE, 4 , "GL15.pgm", "background", false);
   waveform_type2_function();

  // slideshow_tricolor_run(FULL_UPDATE, 2 ,"GL15.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL15.png 0 0 2
//   slideshow_tricolor_run_GL(FULL_UPDATE, 2 ,full_path, 15);
   slideshow_tricolor_run_path(FULL_UPDATE, 2 , "GL15.pgm", "background", false);
   waveform_type1_function();

   //slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
  // slideshow_tricolor_run_GL(FULL_UPDATE, 4 ,full_path, 0);
   slideshow_tricolor_run_path(FULL_UPDATE, 4 , "GL0.pgm", "background", false);
   // # Load Waveform to do the shaky thing and load normal waveform aftwerwards
   waveform_type1_function();

   // slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
  // slideshow_tricolor_run_GL(FULL_UPDATE, 4 ,full_path, 0);
   slideshow_tricolor_run_path(FULL_UPDATE, 4 , "GL0.pgm", "background", false);



   waveform_type2_function();

   //  slideshow_tricolor_run(FULL_UPDATE, 2 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 2
   //slideshow_tricolor_run_GL(FULL_UPDATE, 2 ,full_path, 0);
   slideshow_tricolor_run_path(FULL_UPDATE, 2 , "GL0.pgm", "background", false);


   waveform_type1_function();
//   slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
  // slideshow_tricolor_run_GL(FULL_UPDATE, 4 ,full_path, 0);
   slideshow_tricolor_run_path(FULL_UPDATE, 4 , "GL0.pgm", "background", false);
   //# Update Null Frames
   while(counter < 5)
    {
    UC8156_set_Vcom(vcom_yellow);

    waveform_type2_function();
  //  slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL4.pgm", false);
 //   slideshow_tricolor_run_GL(FULL_UPDATE, 8 ,full_path, 4);
    slideshow_tricolor_run_path(FULL_UPDATE, 10 , "GL4.pgm", "background", false);

    UC8156_set_Vcom(current_vcom);
     waveform_type1_function();
   //  slideshow_tricolor_run(FULL_UPDATE, 1,"GL0.pgm", false);
    // slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);
     slideshow_tricolor_run_path(FULL_UPDATE, 1 , "GL0.pgm", "background", false);
      counter++;
    }


   // # Set Vcom to 0v and Waveform to drive Null Frames
   UC8156_set_Vcom(vcom_yellow);
   waveform_type2_function();
//    slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL4.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL4.png 0 0 4
 //  slideshow_tricolor_run_GL(FULL_UPDATE, 4 ,full_path, 4);
   slideshow_tricolor_run_path(FULL_UPDATE, 10 , "GL4.pgm", "background", false);
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

}


void tricolor_binary(void)
{

const int count = 5;
clear_display();
// slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL15.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL15.png 0 0 4
clear_display();
waveform_type1_function();
//slideshow_tricolor_run_GL(FULL_UPDATE, count ,full_path, 0);

while(1)
    {

        slideshow_run_all(FULL_UPDATE, 3000, count );
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
            abort_now("Fatal error in: slideshow.c -> slideshow_run -> f_opendir", ABORT_SD_CARD);

        do {
            if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0)
                break;

            // skip directories
            if ((fno.fname[0] == '.') || (fno.fattrib & AM_DIR))
                continue;

            //.. and files without the PGM extension
            if (!strstr(fno.fname, ".PGM") && !strstr(fno.fname, ".pgm"))
                continue;

            sprintf(full_path, "%s/%s", path_all, fno.fname);

           slideshow_tricolor_run(mode, count ,full_path, false); // false for original, true for inverse
            mdelay(delay_ms);
        } while (result >= 0);


}


void tricolor_red_partial(int mode)
{
    static  const  int vcom_int_red = 5500;
       unsigned long counter=0;
       // # Load Waveform to do the shaky thing and load normal waveform aftwerwards
           //slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
            waveform_type1_function();
            clear_display();
            slideshow_tricolor_run_GL(mode, 4 ,full_path, 0);

           // slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);
           waveform_type2_function();

           //slideshow_tricolor_run(FULL_UPDATE, 2 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 2
         slideshow_tricolor_run_GL(mode, 2 ,full_path, 0);
       //  slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);


           waveform_type1_function();

           slideshow_tricolor_run_GL(mode, 2 ,full_path, 0);   // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
        //   slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);
           // slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4

       //     # Update Null Frames

           while(counter < 4)
                {
                UC8156_set_Vcom(vcom_int_red);
                drive_voltage_setting(0x25, 0x00);

                //slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL15.pgm", false);
               slideshow_tricolor_run_GL(FULL_UPDATE, 8 ,full_path, 15);
             //  slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 15);

                UC8156_set_Vcom(current_vcom);
                drive_voltage_setting(0x25, 0xff);

                //slideshow_tricolor_run(FULL_UPDATE, 1,"GL0.pgm", false);
               slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);
           //    slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);

                counter++;
                }

           // # Set Vcom to 0v and Waveform to drive Null Frames
           drive_voltage_setting(0x25, 0x00);
           UC8156_set_Vcom(vcom_int_red);

           waveform_type1_function();

           //slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL15.pgm", false);
           slideshow_tricolor_run_GL(FULL_UPDATE, 8 ,full_path, 15);




           // #Begin Black and White Update
           drive_voltage_setting(0x25, 0xff);

           tcom_timing_setting(0x67, 0x55);
           UC8156_set_Vcom(current_vcom);


           waveform_type1_function();

           slideshow_tricolor_run_path(mode, 4 ,"tricolor.pgm", "partial" ,false);  // false for original, true for inverse
           slideshow_tricolor_run_path(FULL_UPDATE, 1 ,"tricolor.pgm", "partial", false);  // false for original, true for inverse
           waveform_type2_function();
           slideshow_tricolor_run_path(mode, 2 ,"tricolor.pgm", "partial", false);  // false for original, true for inverse
            slideshow_tricolor_run_path(FULL_UPDATE, 1 ,"tricolor.pgm", "partial", false);  // false for original, true for inverse
           waveform_type1_function();

           //invertieren hier
         //  slideshow_tricolor_run(FULL_UPDATE, 20 ,"tricolor_yellow_inv.pgm", false);
           slideshow_tricolor_run_path(mode, 4 ,"tricolor.pgm", "partial",true);  // false for original, true for inverse
           slideshow_tricolor_run_path(FULL_UPDATE, 1 ,"tricolor.pgm", "partial", true);  // false for original, true for inverse

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
           slideshow_tricolor_run_path(FULL_UPDATE, 4 ,"tricolor_nach.pgm", "partial", true);

           mdelay(2000);

           slideshow_tricolor_run_path(FULL_UPDATE, 4 ,"tricolor.pgm", "partial", true);  // false for original, true for inverse

           mdelay(2000);
           waveform_type2_function();
           slideshow_tricolor_run_path(PARTIAL_UPDATE, 4 ,"tricolor_nach.pgm", "partial", true);

           waveform_type1_function();
          slideshow_tricolor_run_path(FULL_UPDATE, 1 ,"tricolor_nach.pgm", "partial", true);
         //  waveform_type1_function();
           slideshow_tricolor_run_path(PARTIAL_UPDATE, 4 ,"tricolor_nach.pgm", "partial", true);
           slideshow_tricolor_run_path(FULL_UPDATE, 4 ,"tricolor_nach.pgm", "partial", true);
           /*
           waveform_type2_function();

           slideshow_tricolor_run_path(mode, 2 ,"test_black.pgm", "partial", false);  // false for original, true for inverse
           slideshow_tricolor_run_path(FULL_UPDATE, 1 ,"test_black.pgm", "partial", false);
           waveform_type1_function();
           slideshow_tricolor_run_path(PARTIAL_UPDATE, 4 ,"test_black.pgm", "partial", true);
           slideshow_tricolor_run_path(FULL_UPDATE, 1 ,"test_black.pgm", "partial", true);
           */
           /*
                  slideshow_tricolor_run_path(FULL_UPDATE, 1 ,"black_50.pgm", "img_test", false);
                  waveform_type2_function();
                  slideshow_tricolor_run_path(mode, 2 ,"black_50.pgm", "img_test", false);  // false for original, true for inverse
                  slideshow_tricolor_run_path(FULL_UPDATE, 1 ,"black_50.pgm", "img_test", false);
                  waveform_type1_function();
                  slideshow_tricolor_run_path(mode, 4 ,"black_50.pgm", "img_test", true);
                  slideshow_tricolor_run_path(FULL_UPDATE,1,"black_50.pgm", "img_test", true);
           */

           /*
           UC8156_set_Vcom(vcom_int_red);
            drive_voltage_setting(0x25, 0x00);
            slideshow_tricolor_run_path(mode, 15 ,"pricetag.pgm", "partial", false);
            slideshow_tricolor_run_path(FULL_UPDATE, 1 ,"pricetag.pgm", "partial", false);




            drive_voltage_setting(0x25, 0xff);
            tcom_timing_setting(0x67, 0x55);
            UC8156_set_Vcom(current_vcom);
           slideshow_tricolor_run_path(mode, 4 ,"pricetag.pgm", "partial", false);
           slideshow_tricolor_run_path(FULL_UPDATE, 1 ,"pricetag.pgm", "partial", false);
           waveform_type2_function();
           slideshow_tricolor_run_path(mode, 2 ,"pricetag.pgm", "partial", false);  // false for original, true for inverse
           slideshow_tricolor_run_path(FULL_UPDATE, 1 ,"pricetag.pgm", "partial", false);
           waveform_type1_function();
           slideshow_tricolor_run_path(mode, 4 ,"pricetag.pgm", "partial", true);
           slideshow_tricolor_run_path(FULL_UPDATE,1,"pricetag.pgm", "partial", true);

*/








}



void tricolor_red_all(int mode)
{
    static  const  int vcom_int_red = 5100;
       unsigned long counter=0;
       // # Load Waveform to do the shaky thing and load normal waveform aftwerwards
           //slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
            waveform_type1_function();
            clear_display();
            slideshow_tricolor_run_GL(mode, 4 ,full_path, 0);

           // slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);
           waveform_type2_function();

           //slideshow_tricolor_run(FULL_UPDATE, 2 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 2
         slideshow_tricolor_run_GL(mode, 2 ,full_path, 0);
       //  slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);


           waveform_type1_function();

           slideshow_tricolor_run_GL(mode, 2 ,full_path, 0);   // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
        //   slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);
           // slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4

       //     # Update Null Frames

           while(counter < 4)
                {
                UC8156_set_Vcom(vcom_int_red);
                drive_voltage_setting(0x25, 0x00);

                //slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL15.pgm", false);
               slideshow_tricolor_run_GL(FULL_UPDATE, 8 ,full_path, 15);
             //  slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 15);

                UC8156_set_Vcom(current_vcom);
                drive_voltage_setting(0x25, 0xff);

                //slideshow_tricolor_run(FULL_UPDATE, 1,"GL0.pgm", false);
               slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);
           //    slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);

                counter++;
                }

           // # Set Vcom to 0v and Waveform to drive Null Frames
           drive_voltage_setting(0x25, 0x00);
           UC8156_set_Vcom(vcom_int_red);

           waveform_type1_function();

           //slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL15.pgm", false);
           slideshow_tricolor_run_GL(FULL_UPDATE, 8 ,full_path, 15);




           // #Begin Black and White Update
           drive_voltage_setting(0x25, 0xff);

           tcom_timing_setting(0x67, 0x55);
           UC8156_set_Vcom(current_vcom);


           waveform_type1_function();

           slideshow_tricolor_run_path(mode, 4 ,"tricolor.pgm", "partial" ,false);  // false for original, true for inverse
           slideshow_tricolor_run_path(FULL_UPDATE, 1 ,"tricolor.pgm", "partial", false);  // false for original, true for inverse
           waveform_type2_function();
           slideshow_tricolor_run_path(mode, 2 ,"tricolor.pgm", "partial", false);  // false for original, true for inverse
            slideshow_tricolor_run_path(FULL_UPDATE, 1 ,"tricolor.pgm", "partial", false);  // false for original, true for inverse
           waveform_type1_function();

           //invertieren hier
         //  slideshow_tricolor_run(FULL_UPDATE, 20 ,"tricolor_yellow_inv.pgm", false);
           slideshow_tricolor_run_path(mode, 4 ,"tricolor.pgm", "partial",true);  // false for original, true for inverse
           slideshow_tricolor_run_path(FULL_UPDATE, 1 ,"tricolor.pgm", "partial", true);  // false for original, true for inverse

           mdelay(2000);

           waveform_type1_function();

           slideshow_tricolor_run_path(FULL_UPDATE, 4 ,"tricolor_nach.pgm", "partial", true);

           mdelay(2000);

           slideshow_tricolor_run_path(FULL_UPDATE, 4 ,"tricolor.pgm", "partial", true);  // false for original, true for inverse

           // clear again

           mdelay(2000);
           waveform_type2_function();

/*
         slideshow_tricolor_run_all(FULL_UPDATE, 3 ,"tricolor_transparency_black.pgm", "partial",  TRANSPARENCY_GS3, TRANSPARENCY_DISPLAY_ENABLE_ON , FULL_DISPLAY_UPDATE,   true);
         waveform_type1_function();

         slideshow_tricolor_run_all(FULL_UPDATE, 4 ,"tricolor_transparency_black.pgm", "partial",  TRANSPARENCY_GS3, TRANSPARENCY_DISPLAY_ENABLE_ON , FULL_DISPLAY_UPDATE,   true);
*/
           slideshow_tricolor_run_all(FULL_UPDATE, 3 ,"tricolor_nach.pgm", "partial",  TRANSPARENCY_GS3, TRANSPARENCY_DISPLAY_ENABLE_ON , FULL_DISPLAY_UPDATE,   true);
           waveform_type1_function();

           slideshow_tricolor_run_all(FULL_UPDATE, 4 ,"tricolor_nach.pgm", "partial",  TRANSPARENCY_GS3, TRANSPARENCY_DISPLAY_ENABLE_ON , FULL_DISPLAY_UPDATE,   true);

}

void tricolor_red_opti(int mode)
{
    static  const  int vcom_int_red = 5500;
        unsigned long counter=0;
        // # Load Waveform to do the shaky thing and load normal waveform aftwerwards
            //slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
             waveform_type1_function();
             clear_display();
             slideshow_tricolor_run_GL(mode, 4 ,full_path, 0);

            // slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);
            waveform_type2_function();

            //slideshow_tricolor_run(FULL_UPDATE, 2 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 2
          slideshow_tricolor_run_GL(mode, 4 ,full_path, 0);
        //  slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);


            waveform_type1_function();

            slideshow_tricolor_run_GL(mode, 4 ,full_path, 0);   // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
         //   slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);
            // slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4

        //     # Update Null Frames

            while(counter < 4)
                 {
                 UC8156_set_Vcom(vcom_int_red);
                 drive_voltage_setting(0x25, 0x00);

                 //slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL15.pgm", false);
                slideshow_tricolor_run_GL(FULL_UPDATE, 8 ,full_path, 15);
              //  slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 15);

                 UC8156_set_Vcom(current_vcom);
                 drive_voltage_setting(0x25, 0xff);

                 //slideshow_tricolor_run(FULL_UPDATE, 1,"GL0.pgm", false);
                slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);
            //    slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);

                 counter++;
                 }

            // # Set Vcom to 0v and Waveform to drive Null Frames
            drive_voltage_setting(0x25, 0x00);
            UC8156_set_Vcom(vcom_int_red);

            waveform_type1_function();

            //slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL15.pgm", false);
            slideshow_tricolor_run_GL(FULL_UPDATE, 8 ,full_path, 15);




            // #Begin Black and White Update
            drive_voltage_setting(0x25, 0xff);

            tcom_timing_setting(0x67, 0x55);
            UC8156_set_Vcom(current_vcom);


            waveform_type1_function();

            slideshow_tricolor_run_path(mode, 4 ,"pricetag.pgm", "partial" ,false);  // false for original, true for inverse
            slideshow_tricolor_run_path(FULL_UPDATE, 1 ,"pricetag.pgm", "partial", false);  // false for original, true for inverse
            waveform_type2_function();
            slideshow_tricolor_run_path(mode, 2 ,"pricetag.pgm", "partial", false);  // false for original, true for inverse
             slideshow_tricolor_run_path(FULL_UPDATE, 1 ,"pricetag.pgm", "partial", false);  // false for original, true for inverse
            waveform_type1_function();

            //invertieren hier
          //  slideshow_tricolor_run(FULL_UPDATE, 20 ,"tricolor_yellow_inv.pgm", false);
            slideshow_tricolor_run_path(mode, 4 ,"pricetag.pgm", "partial",true);  // false for original, true for inverse
            slideshow_tricolor_run_path(FULL_UPDATE, 1 ,"pricetag.pgm", "partial", true);  // false for original, true for inverse
/*
            mdelay(2000);

            waveform_type1_function();

            slideshow_tricolor_run_path(FULL_UPDATE, 4 ,"tricolor_nach.pgm", "partial", true);

            mdelay(2000);

            slideshow_tricolor_run_path(FULL_UPDATE, 4 ,"tricolor.pgm", "partial", true);  // false for original, true for inverse

            // clear again

            mdelay(2000);
            waveform_type2_function();


          slideshow_tricolor_run_all(FULL_UPDATE, 3 ,"tricolor_nach.pgm", "partial",  TRANSPARENCY_GS3, TRANSPARENCY_DISPLAY_ENABLE_ON , FULL_DISPLAY_UPDATE,   true);
          waveform_type1_function();

          slideshow_tricolor_run_all(FULL_UPDATE, 4 ,"tricolor_nach.pgm", "partial",  TRANSPARENCY_GS3, TRANSPARENCY_DISPLAY_ENABLE_ON , FULL_DISPLAY_UPDATE,   true);

*/
}


void tricolor_yellow_test(void)
{
    unsigned long counter=0;
    const int vcom_yellow = 0;

    clear_display();
      // slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL15.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL15.png 0 0 4
    UC8156_set_Vcom(vcom_yellow);
    waveform_type1_function();

    slideshow_tricolor_run_GL(FULL_UPDATE, 20 ,full_path, 15);    // 15 for GL15

    waveform_type2_function();
    slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);    // 15 for GL15
    slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 15);    // 15 for GL15

    UC8156_set_Vcom(current_vcom);

    waveform_type1_function();

     slideshow_tricolor_run_GL(FULL_UPDATE, 20 ,full_path, 15);    // 15 for GL15

     waveform_type2_function();
     slideshow_tricolor_run_GL(FULL_UPDATE, 6 ,full_path, 15);    // 15 for GL15


      // slideshow_tricolor_run(FULL_UPDATE, 2 ,"GL15.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL15.png 0 0 2
    slideshow_tricolor_run_GL(FULL_UPDATE, 6 ,full_path, 15);

       waveform_type2_function();

       //slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
      slideshow_tricolor_run_GL(FULL_UPDATE, 4 ,full_path, 0);

       // # Load Waveform to do the shaky thing and load normal waveform aftwerwards
       waveform_type1_function();

       // slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
      slideshow_tricolor_run_GL(FULL_UPDATE, 4 ,full_path, 0);




       waveform_type2_function();

       //  slideshow_tricolor_run(FULL_UPDATE, 2 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 2
       slideshow_tricolor_run_GL(FULL_UPDATE, 2 ,full_path, 0);

       UC8156_set_Vcom(current_vcom);

       waveform_type1_function();
    //   slideshow_tricolor_run(FULL_UPDATE, 4 ,"GL0.pgm", false);     // source ~/scripts/update_image.sh ./images_test/GL0.png 0 0 4
      slideshow_tricolor_run_GL(FULL_UPDATE, 8 ,full_path, 0);

       //# Update Null Frames
       while(counter < 7)
        {
        UC8156_set_Vcom(vcom_yellow);

        waveform_type2_function();
      //  slideshow_tricolor_run(FULL_UPDATE, 8 ,"GL4.pgm", false);
     slideshow_tricolor_run_GL(FULL_UPDATE, 8 ,full_path, 4);

        UC8156_set_Vcom(current_vcom);
         waveform_type1_function();
       //  slideshow_tricolor_run(FULL_UPDATE, 1,"GL0.pgm", false);
        slideshow_tricolor_run_GL(FULL_UPDATE, 1 ,full_path, 0);

          counter++;
        }

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
