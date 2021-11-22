/*
 * UC8177C_MTP.c
 *
 *  Created on: 01.11.2021
 *      Author: zhongke.dai
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "msp430/msp430-spi.h"
#include "UC8177C.h"
#include "UC8177C_MTP.h"
#include "read-sd.h"

#include "msp430-gpio.h"


void UC8177_Eink_ini(void)    // first all settings according to datasheet with default
{
    UC8177_PSR(0x75, 0x03);
    UC8177_PWR(0x03, 0x03, 0x06, 0x00);
    UC8177_PFS(0x03);

    UC8177_BTST(0xEF, 0xEF, 0x28);
  //  UC8177_DRF(0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    UC8177_PS(0x42);   // Power saving
    UC8177_MISCS1(0x05);
    UC8177_PLL(0x0E);
    UC8177_CDI(0x01, 0x99);
    UC8177_TCON(0x1D, 0x02, 0x1A);
    UC8177_TRES(0x01, 0xE0, 0x02, 0x58);
    UC8177_DAM(0x00);
    UC8177_EDS(0x00);
    UC8177_XONS(0x00, 0x00, 0x00, 0x63, 0x00, 0x1E);
    UC8177_GDOS(0x02, 0x00);
    UC8177_VDCS(0x30);
    UC8177_TSE(0x00, 0x00);
}


void UC8177_image_update( u8 *image_data)
{
    UC8177_PON();    // power on
   // UC8177_DTMW(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);       // data window setting
   // DTM1                                                               // write image pixel

    UC8177_wait_for_BUSY_inactive();                                  // check from busy pin

   // UC8177_DRF();
    UC8177_wait_for_BUSY_inactive();
    UC8177_POF();


}

void UC8177_white_update(void)  // update all white
{
    UC8177_PON();    // power on
   // UC8177_DTMW(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);       // data window setting



    //////////////////////////DTM1////////////////////
    gpio_set_value_lo(SPI_CS);
    u8 command = 0x10;
    command &= ~0x80;
    spi_write_read_byte(command);
    u8 cur_bpp = 0x00;  // 00b: 1bpp
    spi_write_read_byte(cur_bpp);
    const unsigned long sum =  36000;
      unsigned long i;
      for (i = 0; i < sum; i++ )
      {
          spi_write_read_byte(0xFF);
      }

    gpio_set_value_hi(SPI_CS);// write image pixel
    spi_write_only_command(0x11);                                                              // write image pixel
    //////////////////////////DTM1////////////////////
    UC8177_wait_for_BUSY_inactive();                                  // check from busy pin

    UC8177_DRF(0x00, 0x00, 0x00, 0x00, 0x01, 0xE0, 0x02, 0x58   );
    UC8177_wait_for_BUSY_inactive();
    UC8177_POF();


}

void UC8177_black_update(void)  // update all black
{
    UC8177_PON();    // power on
   // UC8177_DTMW(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);       // data window setting



//////////////////////////DTM1////////////////////
    gpio_set_value_lo(SPI_CS);
    u8 command = 0x10;
    command &= ~0x80;
    u8 cur_bpp = 0x00;  // 00b: 1bpp
    spi_write_read_byte(command);
    spi_write_read_byte(cur_bpp);


    const unsigned long sum =  36000;
     unsigned long i;
     for (i = 0; i < sum; i++ )
     {
         spi_write_read_byte(0x00);
     }

    gpio_set_value_hi(SPI_CS);// write image pixel
    UC8177_wait_for_BUSY_inactive();                                  // check from busy pin

    spi_write_only_command(0x11);                                                              // write image pixel

    //////////////////////////DTM1////////////////////

    UC8177_wait_for_BUSY_inactive();
    UC8177_DRF(0x00, 0x00, 0x00, 0x00, 0x01, 0xE0, 0x02, 0x58 );
    UC8177_wait_for_BUSY_inactive();
    UC8177_POF();


}






