/*
 * UC8177C_debug.c
 *
 *  Created on: 01.11.2021
 *      Author: zhongke.dai
 */


#include <stdlib.h>
#include <string.h>
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
//#include "msp430-i2c.h"
#include "msp430-gpio.h"
#include "msp430-nvm-spi.h"

#include "config_display_type.h"

#include "waveform.h"
//#define WAVEFORM_LENGTH_UC8177 61696    // 3856 (0x0F10) * 16



//extern const int BUFFER_LENGTH = 64;

void UC8177_basic_flow(void)
{

    char PATH[64]; //global variable
   // set_display_type(S021_T1_1);
    strcpy(PATH, "S028_T1.1");

    char path[64];
  //  char path_new[64];
   // char image_path1[64];
    char image_path2[64];
    char image_path3[64];

        UC8177_wait_for_BUSY_inactive(); // wait for power-up completed

        // optional: additional hardware reset after 5ms delay
        mdelay(5);
        UC8177_hardware_reset(); // UC8179 hardware reset
        UC8177_wait_for_BUSY_inactive(); // wait for RESET completed

        // optional -> verifies successful power-up
        UC8177_check_RevID();



        UC8177_Eink_ini();


        UC8177_DAM(0x01);



     //   waveform_sd_to_externalFlash();    // external flash waveform write













        UC8177_PON();
        mdelay(5);
        flag_check(0);



//        sprintf(path, "/%s/%s", PATH, "display/Eink_S028_double.uc8177_lut"); // short: Eink_S028.uc8177_lut; double:  Eink_S028_double.uc8177_lut; 16GL: Eink_S028_16GL.uc8177_lut; external flash: Eink_S028_Waveform.bin
//        //sprintf(path_new, "/%s/%s", PATH, "display/Eink_S028_16GL.uc8177_lut"); // short: Eink_S028.uc8177_lut; double:  Eink_S028_double.uc8177_lut; 16GL: Eink_S028_16GL.uc8177_lut
//
//        bool Waveform_read_finish = UC8177_Send_WaveformFile_to_LUTD_static(path);




        UC8177_LUTD(0x2E, WF_UC8177_LUTD, 16384 );

            UC8177_white_update();
            //sprintf(image_path1, "/%s/%s", PATH, "img/LedgerLogo_600x480_Black.pgm");
            mdelay(1000);


        sprintf(image_path2, "/%s/%s", PATH, "img/LedgerLogo_600x480_White.pgm");
        sprintf(image_path3, "/%s/%s", PATH, "img/TestPic.pgm");
        /////////////////////////////////////////////////////////////////////////// waveform too big for msp430
//         long WAVEFORM_LENGTH_UC8177 = 61696;
//        u8 waveform_from_file[WAVEFORM_LENGTH_UC8177];                      // maybe buffer here not enough for waveform UC8177, should slide to 2 parts
//        if (sdcard_load_waveform(path, waveform_from_file, WAVEFORM_LENGTH_UC8177))
//            {



        ///////////////////////////////////////////////////////////////////////

        UC8177_Eink_ini();


        //bool Waveform_read_finish = UC8177_Send_WaveformFile_to_LUTD_static(path);








              UC8177_black_update();



               printf("black update start \n");
                UC8177_black_update();


                mdelay(3000);
                printf("white update start \n");
                UC8177_white_update();
                printf("image3 update start \n");
                UC8177_image_update(image_path3);
                mdelay(3000);
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
//        printf("image1 update start \n");
//        UC8177_image_update(image_path1);
//        mdelay(3000);

        printf("black update start \n");
        UC8177_white_update();
        mdelay(3000);

        printf("image2 update start \n");
        UC8177_image_update(image_path2);
        mdelay(3000);

        printf("black update start \n");
        UC8177_white_update();
        mdelay(3000);

        printf("image3 update start \n");
        UC8177_image_update(image_path3);
        mdelay(3000);

   }while(1);

}



int waveform_sd_to_externalFlash()
{


    char PATH[64]; //global variable
      // set_display_type(S021_T1_1);
       strcpy(PATH, "S028_T1.1");

       char wf_path[64];


       sprintf(wf_path, "/%s/%s", PATH, "display/random_waveform.bin");     //test: random_waveform.bin;    full waveform: Eink_S028_Waveform.bin
       printf("wf path = %s \n", wf_path);





       if(  MX25U4033E_chip_erase() == 0)

       {
           printf("external flash erase successful.  \n");
       }
       else
       {
           printf("external flash erase failed!!!.  \n");
           return -1;
       }

       if(waveform_sd_to_externalFlash_part(wf_path,WF_SIZE_PART1_START, WF_SIZE_MAX))
           return -1;


        waveform_check(wf_path, WF_SIZE_PART1_START);




     return 0;
}


int waveform_sd_to_externalFlash_part(char *wf_path, size_t address_start,uint32_t buffer_to_read_length)
{
    FIL file;
    if (f_open(&file, wf_path, FA_READ))
        return -1;

    if(f_lseek(&file,  address_start))
        return -1;
    uint32_t bytes_to_transfer = buffer_to_read_length;

    static const int chunkSize = 1024;
    u8 data_buffer[1024];

    uint16_t count;
    uint32_t register_address = address_start;
    uint32_t byte_offset = 0;

    uint8_t data[1024];
    do
      {

            size_t transferChunkSize = (bytes_to_transfer >= chunkSize) ? chunkSize : bytes_to_transfer;   // choose the bigger one from chunkSize and bytes_to_transfer

                  f_read(&file, data_buffer, transferChunkSize, &count);                     // file, buffer_array[], buffer_length, sum

                  if(nvm_MX25U4033E_spi_pgm(register_address, data_buffer,  transferChunkSize, data) != 0)
                  {
                      printf("register_address stoped in %ld \n", register_address);
                      printf("rest bytes_to_transfer = %ld \n", bytes_to_transfer);
                      f_close(&file);
                      return -1;

                  }


            byte_offset += transferChunkSize;
            register_address += transferChunkSize;
            bytes_to_transfer -= transferChunkSize;

      }    while(bytes_to_transfer > 0);



      printf("byte_offset = %ld \n", byte_offset);
      printf("register_address = %ld \n", register_address);
      printf("bytes_to_transfer = %ld \n", bytes_to_transfer);
      printf("external flash saved successfully!!!.  \n");

    f_close(&file);


    return 0;
}



int waveform_sd_to_externalFlash_part_no_check(char *wf_path, size_t address_start,uint32_t buffer_to_read_length)
{
    FIL file;
    if (f_open(&file, wf_path, FA_READ))
        return -1;

    if(f_lseek(&file,  address_start))
        return -1;
    uint32_t bytes_to_transfer = buffer_to_read_length;

    static const int chunkSize = 1024;
    u8 data_buffer[1024];

    uint16_t count;
    uint32_t register_address = address_start;
    uint32_t byte_offset = 0;

    uint8_t data[1024];
    do
      {

            size_t transferChunkSize = (bytes_to_transfer >= chunkSize) ? chunkSize : bytes_to_transfer;   // choose the bigger one from chunkSize and bytes_to_transfer

                  f_read(&file, data_buffer, chunkSize, &count);                     // file, buffer_array[], buffer_length, sum

                  if(nvm_MX25U4033E_spi_pgm_no_check(register_address, data_buffer,  chunkSize, data) != 0)
                  {
                      printf("register_address stoped in %ld \n", register_address);
                      printf("rest bytes_to_transfer = %ld \n", bytes_to_transfer);
                      f_close(&file);
                      return -1;

                  }


            byte_offset += transferChunkSize;
            register_address += transferChunkSize;
            bytes_to_transfer -= transferChunkSize;

      }    while(bytes_to_transfer > 0);



      printf("byte_offset = %ld \n", byte_offset);
      printf("register_address = %ld \n", register_address);
      printf("bytes_to_transfer = %ld \n", bytes_to_transfer);
      printf("external flash saved successfully!!!.  \n");

    f_close(&file);


    return 0;
}


int waveform_check(char *wf_path, uint32_t register_address_start )
{

    FIL file;
      if (f_open(&file, wf_path, FA_READ))
          return -1;


      uint32_t bytes_to_transfer = WF_SIZE_MAX;

      static const int chunkSize = 1024;
      u8 blob[1024];

      uint16_t count;
      uint32_t register_address = register_address_start ;
      uint32_t byte_offset = 0;
      if(f_lseek(&file,  register_address))
          return -1;
      uint8_t data_read[1024];
      uint8_t cmp_blob[1024];
      printf("check start \n");
      do
        {

              size_t transferChunkSize = (bytes_to_transfer >= chunkSize) ? chunkSize : bytes_to_transfer;   // choose the bigger one from chunkSize and bytes_to_transfer

                    f_read(&file, blob, transferChunkSize, &count);                     // file, buffer_array[], buffer_length, sum

                if (   nvm_MX25U4033E_spi_read(register_address, cmp_blob, transferChunkSize, data_read) != -1)
                {
                            if (memcmp(blob, cmp_blob, transferChunkSize) != 0)
                            {

                                printf("blob[0] = %X, %X \n", blob[0], cmp_blob[0]);
                                printf("blob[1] = %X, %X \n", blob[1], cmp_blob[1]);
                                printf("blob[2] = %X, %X \n", blob[2], cmp_blob[2]);


                                printf("register_address stoped in %ld \n", register_address);
                                printf("rest bytes_to_transfer = %ld \n", bytes_to_transfer);
                                f_close(&file);

                                return -1;
                            }
                }
                else

                {
                    printf("register_address stoped in %ld \n", register_address);
                    printf("rest bytes_to_transfer = %ld \n", bytes_to_transfer);
                    f_close(&file);
                    return -1;

                }

              byte_offset += transferChunkSize;
              register_address += transferChunkSize;
              bytes_to_transfer -= transferChunkSize;

        }    while(bytes_to_transfer > 0);



        printf("byte_offset = %ld \n", byte_offset);
        printf("register_address = %ld \n", register_address);
        printf("bytes_to_transfer = %ld \n", bytes_to_transfer);
        printf("external flash saved successfully!!!.  \n");

      f_close(&file);





    return 0;
}




















