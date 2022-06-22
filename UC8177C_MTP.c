/*
 * UC8177C_MTP.c
 *
 *  Created on: 01.11.2021
 *      Author: zhongke.dai
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "msp430/msp430-spi.h"
#include "UC8177C.h"
#include "UC8177C_MTP.h"
#include "read-sd.h"

#include "msp430-gpio.h"


void UC8177_Eink_ini(void)    // first all settings according to datasheet with default
{
    UC8177_PSR(0x61, 0x00);   //  0x61: LUT from Register; 0x41: LUT from external flash
    UC8177_PWR(0x03, 0x04, 0x00, 0x00);
    UC8177_PFS(0x03);

   UC8177_BTST(0xEF, 0xEF, 0x28);

  //  UC8177_DRF(0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    UC8177_PS(0x42);   // Power saving
    UC8177_MISCS1(0x15);  // 0x15: auto LUTT enable; 0x05 auto LUTT disable
    UC8177_PLL(0x0E);
    UC8177_CDI(0x01, 0x22);
    UC8177_TCON(0x3F, 0x09, 0x2D);
    UC8177_TRES(0x02, 0x58, 0x01, 0xE0);
    UC8177_DAM(0x01);
  //  UC8177_EDS(0x00);
  //  UC8177_XONS(0x00, 0x00, 0x00, 0x63, 0x00, 0x1E);
    UC8177_GDOS(0x02, 0xFF);
    UC8177_VDCS(0x18);        // Vcom
    UC8177_TSE(0x00, 0x1A);
}




void UC8177_white_update(void)  // update all white
{

    UC8177_DTMW(0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x01, 0xE0);       // data window setting



    //////////////////////////DTM1////////////////////
    gpio_set_value_lo(SPI_CS);
    u8 command = 0x10;
    spi_write_read_byte(command);
    u8 cur_bpp = 0x00;  // 00b: 1bpp
    spi_write_read_byte(cur_bpp);
    const unsigned long sum =  36480;
      unsigned long i;
      for (i = sum; i > 0; i-- )
      {
          spi_write_read_byte(0xFF);
      }

    gpio_set_value_hi(SPI_CS);// write image pixel
    UC8177_PON();    // power on
    mdelay(5);

    UC8177_wait_for_BUSY_inactive();
//    mdelay(5);
//    flag_check(0);

    //////////////////////////DTM1////////////////////
                               // check from busy pin

    UC8177_DRF(0x08, 0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x01, 0xE0);
    UC8177_wait_for_BUSY_inactive();
 //   mdelay(5);
//    flag_check(0);
//    mdelay(5);
    UC8177_POF();


}

void UC8177_black_update(void)  // update all black
{
    UC8177_DTMW(0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x01, 0xE0);       // data window setting    608 x 480



    //////////////////////////DTM1////////////////////
    gpio_set_value_lo(SPI_CS);
    u8 command = 0x10;
    spi_write_read_byte(command);
    u8 cur_bpp = 0x00;  // 00b: 1bpp
    spi_write_read_byte(cur_bpp);
    const unsigned long sum =  36480;
      unsigned long i;
      for (i = sum; i > 0; i-- )
      {
          spi_write_read_byte(0x00);
      }

    gpio_set_value_hi(SPI_CS);// write image pixel
    UC8177_PON();    // power on
    mdelay(50);

    UC8177_wait_for_BUSY_inactive();
//    mdelay(5);
//    flag_check(0);
    //////////////////////////DTM1////////////////////
                               // check from busy pin

    UC8177_DRF(0x08, 0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x01, 0xE0);
    UC8177_wait_for_BUSY_inactive();
    UC8177_POF();



}



void UC8177_test_update(void)  // update all black
{
    UC8177_PON();    // power on
    UC8177_DTMW(0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x01, 0xE0);       // data window setting    600 x 480



    //////////////////////////DTM1////////////////////
    gpio_set_value_lo(SPI_CS);
    u8 command = 0x10;
    spi_write_read_byte(command);
    u8 cur_bpp = 0x00;  // 00b: 1bpp
    spi_write_read_byte(cur_bpp);
    const unsigned long sum =  36480;
      unsigned long i;
      for (i = 0; i < sum/4; i++ )
      {
          spi_write_read_byte(0x00);
      }
      for (i = 0; i < sum/4; i++ )
      {
          spi_write_read_byte(0xFF);
      }
      for (i = 0; i < sum/4; i++ )
      {
          spi_write_read_byte(0x00);
      }
      for (i = 0; i < sum/4; i++ )
      {
          spi_write_read_byte(0xFF);
      }
    gpio_set_value_hi(SPI_CS);// write image pixel

    mdelay(5);

    UC8177_wait_for_BUSY_inactive();
//    mdelay(5);
//    flag_check(0);
    //////////////////////////DTM1////////////////////
                               // check from busy pin

    UC8177_DRF(0x08, 0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x01, 0xE0);
    UC8177_wait_for_BUSY_inactive();
    UC8177_POF();
}


void UC8177_image_update(char *image_path)  // update all black
{
    UC8177_PON();    // power on
    UC8177_DTMW(0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x01, 0xE0);       // data window setting    608 x 480


    gpio_set_value_lo(SPI_CS);
    //////////////////////////DTM1////////////////////

   // bool image_read_finish =   UC8177_image_read_from_sd(image_path, data_buff);


    bool image_read_finish =   UC8177_image_read_from_sd_4bpp(image_path, 600, 480);

    printf("image_read_finish: %s\n", image_read_finish ? "true" : "false");

    gpio_set_value_hi(SPI_CS);// write image pixel
    //mdelay(5);

    UC8177_wait_for_BUSY_inactive();
    mdelay(5);
    //////////////////////////DTM1////////////////////
                               // check from busy pin

    UC8177_DRF(0x08, 0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x01, 0xE0);
    UC8177_wait_for_BUSY_inactive();
    UC8177_POF();
}



u8 flag_check(u8 flag_byte)    //
{
    u8 check_flag = 0;
    u8 flag_buffer[2];
    UC8177_FLG(flag_buffer);


    check_flag = flag_buffer[0];
    printf("Flag Byte0 = %x\n", flag_buffer[0]);
    printf("Flag Byte1 = %x\n", flag_buffer[1]);
    return check_flag;

}


void temp_check(void)    //
{

    u8 temp_buffer[2];
    UC8177_TSC(temp_buffer);
    int temp_TS = (temp_buffer[0] << 1) + (temp_buffer[1]>>7);
   int temp_real = temp_TS / 2;
    printf("Temp  = %d °C \n", temp_real );



}






void buffer_check(void)
{

    int j;
    for( j = 0; j < 15; j++)
    {      UC8177_PON();    // power on
        u8 buffer[2+32*32/8];
        buffer[0] = 0x10;
        buffer[1] = 0x00;
        memset(buffer+2, 0xFF, sizeof(buffer)-2);
     //   UC8177_DTMW(0x00, 0x00, 0x00,j*32 ,0x00, 0x00, 0x20, 0x00, 0x20);       // data window setting    608 x 480

        gpio_set_value_lo(SPI_CS);
        int i = 0;
        for ( i = 0; i < sizeof(buffer); i++ )
            {
                spi_write_read_byte(buffer[i]);
            }

        gpio_set_value_hi(SPI_CS);// write image pixel
             mdelay(5);


        UC8177_DRF(0x08, 0x00, 0x00, 0x00, 0x00, 0x02, 0x60, 0x01, 0xE0);
        UC8177_wait_for_BUSY_inactive();
        UC8177_POF();
        UC8177_wait_for_BUSY_inactive();
       // mdelay(2000);
        printf("j = %x\n", j);

    }
}

    void UC8177_image_update_partial(char *image_path)  // update all black
    {
        UC8177_PON();    // power on
        UC8177_DTMW(0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x01, 0xE0);       // data window setting    608 x 480


        gpio_set_value_lo(SPI_CS);
        //////////////////////////DTM1////////////////////

       // bool image_read_finish =   UC8177_image_read_from_sd(image_path, data_buff);


        bool image_read_finish =   UC8177_image_read_from_sd_4bpp_partial_update(image_path, 600, 480);

        printf("image_read_finish: %s\n", image_read_finish ? "true" : "false");

        gpio_set_value_hi(SPI_CS);// write image pixel
        //mdelay(5);

        UC8177_wait_for_BUSY_inactive();
        mdelay(5);
        //////////////////////////DTM1////////////////////
                                   // check from busy pin

        UC8177_DRF(0x0C, 0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x01, 0xE0);       // UPD_CPY_TO_PRE:1; DN_EN:1
        UC8177_wait_for_BUSY_inactive();
        UC8177_POF();
    }




