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
    UC8177_PSR(0x65, 0x00);   //  0x61: LUT from Register; 0x41: LUT from external flash
    UC8177_PWR(0x03, 0x04, 0x00, 0x00);
    UC8177_PFS(0x03);

   UC8177_BTST(0xDF, 0xDF, 0x28);

  //  UC8177_DRF(0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    UC8177_PS(0x42);   // Power saving
    UC8177_MISCS1(0x35);  // 0x15: auto LUTT enable; 0x05 auto LUTT disable
    UC8177_PLL(0x08);
    UC8177_CDI(0x01, 0x22);
    UC8177_TCON(0x3F, 0x2D, 0x2D);
    UC8177_TRES(0x02, 0xA0, 0x01, 0xF0);
    UC8177_DAM(0x00);
  //  UC8177_EDS(0x00);
  //  UC8177_XONS(0x00, 0x00, 0x00, 0x63, 0x00, 0x1E);
    UC8177_GDOS(0x05, 0xFF);
    UC8177_VDCS(0x00);        // Vcom
    UC8177_TSE(0x40, 0x1A);
    UC8177_AWM1(0x88);


}




void UC8177_white_update(void)  // update all white
{

    UC8177_DTMW(0x00, 0x00, 0x00, 0x00, 0x02, 0xA0, 0x01, 0xF0);       // data window setting    672 (0x02A0) x 496 (0x01F0)



    //////////////////////////DTM1////////////////////
    gpio_set_value_lo(SPI_CS);
    u8 command = 0x10;
    spi_write_read_byte(command);
    u8 cur_bpp = 0x00;  // 00b: 1bpp
    spi_write_read_byte(cur_bpp);
    const unsigned long sum =   41664;
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

    UC8177_DRF(0x08, 0x00, 0x00, 0x00, 0x00, 0x02, 0xA0, 0x01, 0xF0);
    UC8177_wait_for_BUSY_inactive();
 //   mdelay(5);
//    flag_check(0);
//    mdelay(5);
    UC8177_POF();


}

void UC8177_black_update(void)  // update all black
{
    UC8177_PON();    // power on
    UC8177_DTMW(0x00, 0x00, 0x00, 0x00, 0x02, 0xA0, 0x01, 0xF0);       // data window setting    672 (0x02A0) x 496 (0x01F0)



    //////////////////////////DTM1////////////////////
    gpio_set_value_lo(SPI_CS);
    u8 command = 0x10;
    spi_write_read_byte(command);
    u8 cur_bpp = 0x00;  // 00b: 1bpp
    spi_write_read_byte(cur_bpp);
    const unsigned long sum =  41664;
      unsigned long i;
      for (i = sum; i > 0; i-- )
      {
          spi_write_read_byte(0x00);
      }

    gpio_set_value_hi(SPI_CS);// write image pixel

    mdelay(50);

    UC8177_wait_for_BUSY_inactive();
//    mdelay(5);
//    flag_check(0);
    //////////////////////////DTM1////////////////////
                               // check from busy pin

    UC8177_DRF(0x08, 0x00, 0x00, 0x00, 0x00, 0x02, 0xA0, 0x01, 0xF0);
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


void UC8177_image_update(char *image_path, u8 DN_EN)  //
{
    UC8177_PON();    // power on
    UC8177_DTMW(0x00, 0x00, 0x00, 0x00, 0x02, 0xA0, 0x01, 0xF0);       // data window setting    672 x 496


    gpio_set_value_lo(SPI_CS);
    //////////////////////////DTM1////////////////////

   // bool image_read_finish =   UC8177_image_read_from_sd(image_path, data_buff);


    bool image_read_finish =   UC8177_image_read_from_sd_4bpp(image_path, 672, 496);

    printf("image_read_finish: %s\n", image_read_finish ? "true" : "false");

    gpio_set_value_hi(SPI_CS);// write image pixel
    //mdelay(5);

    UC8177_wait_for_BUSY_inactive();
    mdelay(5);
    //////////////////////////DTM1////////////////////
                               // check from busy pin
  u8 partial_update = 0x08;
    if(DN_EN == 1)
    {
        partial_update = 0x0C;
    }

    UC8177_DRF(partial_update, 0x00, 0x00, 0x00, 0x00, 0x02, 0xA0, 0x01, 0xF0);
    UC8177_wait_for_BUSY_inactive();
    UC8177_POF();
}




void UC8177_image_update2(char *image_path, u8 DN_EN)  // power on later
{

  //  UC8177_PON();    // power on
    UC8177_DTMW(0x00, 0x00, 0x00, 0x00, 0x02, 0xA0, 0x01, 0xF0);       // data window setting    672 x 496


    gpio_set_value_lo(SPI_CS);
    //////////////////////////DTM1////////////////////

   // bool image_read_finish =   UC8177_image_read_from_sd(image_path, data_buff);


    bool image_read_finish =   UC8177_image_read_from_sd_4bpp(image_path, 672, 496);

    printf("image_read_finish: %s\n", image_read_finish ? "true" : "false");

    gpio_set_value_hi(SPI_CS);// write image pixel
    //mdelay(5);

    UC8177_wait_for_BUSY_inactive();
    mdelay(5);
    //////////////////////////DTM1////////////////////
                               // check from busy pin
  u8 patial_update = 0x08;
    if(DN_EN == 1)
    {
        patial_update = 0x0C;
    }


    UC8177_PON();    // power on
    UC8177_wait_for_BUSY_inactive();
    UC8177_DRF(patial_update, 0x00, 0x00, 0x00, 0x00, 0x02, 0xA0, 0x01, 0xF0);
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



    void UC8177_black_partial_update(u16 p_x, u16 p_y, u16 area_w, u16 area_h)  // update area black
    {
        u8 p_x1 = p_x >> 8;
        u8 p_x2 = p_x & 0xff;

        u8 p_y1 = p_y >> 8;
        u8 p_y2 = p_y & 0xff;

        u8 w1 = area_w >> 8;
        u8 w2 = area_w & 0xff;

        u8 h1 = area_h >> 8;
        u8 h2 = area_h & 0xff;
        UC8177_DTMW(p_x1, p_x2, p_y1, p_y2, w1, w2, h1, h2);       // data window setting    p_x -> start point x achse;  p_y -> start point y achse;
        printf("p_x1 = %x\n", p_x1);
        printf("p_x2 = %x\n", p_x2);
        printf("p_y1 = %x\n", p_y1);
        printf("p_y2 = %x\n", p_y2);
        printf("w1 = %x\n", w1);
        printf("w2 = %x\n", w2);
        printf("h1 = %x\n", h1);
        printf("h2 = %x\n", h2);
        //////////////////////////DTM1////////////////////
        unsigned long sum =  area_w * area_h / 8 ;
        printf("sum = %d\n", sum);
           //////////////////////////DTM1////////////////////


        gpio_set_value_lo(SPI_CS);

        spi_write_read_byte(0x10);
       // cur_bpp = 0x00;  // 00b: 1bpp
        spi_write_read_byte(0x00);


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

        UC8177_DRF(0x0C, 0x00, 0x00, 0x00, 0x00, 0x02, 0xA0, 0x01, 0xF0);
        UC8177_wait_for_BUSY_inactive();
        UC8177_POF();



    }


    void UC8177_white_partial_update(u16 p_x, u16 p_y, u16 area_w, u16 area_h)  // update area white
     {
         u8 p_x1 = p_x >> 8;
         u8 p_x2 = p_x & 0xff;

         u8 p_y1 = p_y >> 8;
         u8 p_y2 = p_y & 0xff;

         u8 w1 = area_w >> 8;
         u8 w2 = area_w & 0xff;

         u8 h1 = area_h >> 8;
         u8 h2 = area_h & 0xff;
         printf("p_x1 = %x\n", p_x1);
         printf("p_x2 = %x\n", p_x2);
         printf("p_y1 = %x\n", p_y1);
         printf("p_y2 = %x\n", p_y2);
         printf("w1 = %x\n", w1);
         printf("w2 = %x\n", w2);
         printf("h1 = %x\n", h1);
         printf("h2 = %x\n", h2);
         UC8177_DTMW(p_x1, p_x2, p_y1, p_y2, w1, w2, h1, h2);       // data window setting    p_x -> start point x achse;  p_y -> start point y achse;

         //////////////////////////DTM1////////////////////
         unsigned long sum =  area_w * area_h / 8;



         printf("sum = %d\n", sum);





            //////////////////////////DTM1////////////////////
 //           gpio_set_value_lo(SPI_CS);
 //           u8 command = 0x13;
 //           spi_write_read_byte(command);
 //           u8 cur_bpp = 0x00;  // 00b: 1bpp
 //           spi_write_read_byte(cur_bpp);
 //             unsigned long j;
 //             for (j = sum; j > 0;j-- )
 //             {
 //                 spi_write_read_byte(0xFF);
 //             }
 //           gpio_set_value_hi(SPI_CS);// write image pixel















         gpio_set_value_lo(SPI_CS);

         spi_write_read_byte(0x10);
        // cur_bpp = 0x00;  // 00b: 1bpp
         spi_write_read_byte(0x00);


           unsigned long i;
           for (i = sum; i > 0; i-- )
           {
               spi_write_read_byte(0xFF);
           }

         gpio_set_value_hi(SPI_CS);// write image pixel
         UC8177_PON();    // power on
         mdelay(50);

         UC8177_wait_for_BUSY_inactive();
     //    mdelay(5);
     //    flag_check(0);
         //////////////////////////DTM1////////////////////
                                    // check from busy pin

         UC8177_DRF(0x0C, 0x00, 0x00, 0x00, 0x00, 0x02, 0xA0, 0x01, 0xF0);
         UC8177_wait_for_BUSY_inactive();
         UC8177_POF();



     }









    void UC8177_black_partial_pipeline_update(u16 p_x, u16 p_y, u16 area_w, u16 area_h)  // update area black
    {
        u8 p_x1 = p_x >> 8;
        u8 p_x2 = p_x & 0xff;

        u8 p_y1 = p_y >> 8;
        u8 p_y2 = p_y & 0xff;

        u8 w1 = area_w >> 8;
        u8 w2 = area_w & 0xff;

        u8 h1 = area_h >> 8;
        u8 h2 = area_h & 0xff;
        UC8177_DTMW(p_x1, p_x2, p_y1, p_y2, w1, w2, h1, h2);       // data window setting    p_x -> start point x achse;  p_y -> start point y achse;
        printf("p_x1 = %x\n", p_x1);
        printf("p_x2 = %x\n", p_x2);
        printf("p_y1 = %x\n", p_y1);
        printf("p_y2 = %x\n", p_y2);
        printf("w1 = %x\n", w1);
        printf("w2 = %x\n", w2);
        printf("h1 = %x\n", h1);
        printf("h2 = %x\n", h2);
        //////////////////////////DTM1////////////////////
        unsigned long sum =  area_w * area_h / 8 ;
        printf("sum = %d\n", sum);
           //////////////////////////DTM1////////////////////


        gpio_set_value_lo(SPI_CS);

        spi_write_read_byte(0x10);
       // cur_bpp = 0x00;  // 00b: 1bpp
        spi_write_read_byte(0x00);


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

        UC8177_DRF(0x0C, 0x00, 0x00, 0x00, 0x00, 0x02, 0xA0, 0x01, 0xF0);
        UC8177_wait_for_BUSY_inactive();
        UC8177_POF();



    }



    void UC8177_white_partial_all_update(void)  // update area white
     {

        UC8177_DTMW(0x00, 0x00, 0x00, 0x00, 0x02, 0xA0, 0x01, 0xF0);       // data window setting    672 (0x02A0) x 496 (0x01F0)



        //////////////////////////DTM1////////////////////
        gpio_set_value_lo(SPI_CS);
        u8 command = 0x10;
        spi_write_read_byte(command);
        u8 cur_bpp = 0x00;  // 00b: 1bpp
        spi_write_read_byte(cur_bpp);
        const unsigned long sum =   41664;
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

        UC8177_DRF(0x0C, 0x00, 0x00, 0x00, 0x00, 0x02, 0xA0, 0x01, 0xF0);
        UC8177_wait_for_BUSY_inactive();
     //   mdelay(5);
    //    flag_check(0);
    //    mdelay(5);
        UC8177_POF();



     }




    void UC8177_black_partial_edge_update_test(void)  // edge ghosting test
       {



        //                UC8177_black_partial_update(0, 0, 104, 104 );
        //                UC8177_black_partial_update(104, 104, 104, 104 );
        //                UC8177_black_partial_update(208, 208, 104, 104 );
        //                UC8177_black_partial_update(316, 316, 104, 104 );






           u16 rec1_p_x = 20;
           u16 rec1_p_y = 0;
           u8 rec1_area_w = 104;
           u8 rec1_area_h = 104;
           bool rec1_show = true;

           u16 rec2_p_x = 104;
           u16 rec2_p_y = 104;
           u8 rec2_area_w = 104;
           u8 rec2_area_h = 104;
           bool rec2_show = false;



           u16 rec3_p_x = 208;
           u16 rec3_p_y = 208;
           u8 rec3_area_w = 104;
           u8 rec3_area_h = 104;
           bool rec3_show = false;




           u16 rec4_p_x = 312;
           u16 rec4_p_y = 312;
           u8 rec4_area_w = 104;
           u8 rec4_area_h = 104;
           bool rec4_show = false;








           u8 rec1_p_x1 = rec1_p_x >> 8;
           u8 rec1_p_x2 = rec1_p_x & 0xff;

           u8 rec1_p_y1 = rec1_p_y >> 8;
           u8 rec1_p_y2 = rec1_p_y & 0xff;

           u8 rec1_w1 = rec1_area_w >> 8;
           u8 rec1_w2 = rec1_area_w & 0xff;

           u8 rec1_h1 = rec1_area_h >> 8;
           u8 rec1_h2 = rec1_area_h & 0xff;




           u8 rec2_p_x1 = rec2_p_x >> 8;
           u8 rec2_p_x2 = rec2_p_x & 0xff;

           u8 rec2_p_y1 = rec2_p_y >> 8;
           u8 rec2_p_y2 = rec2_p_y & 0xff;

           u8 rec2_w1 = rec2_area_w >> 8;
           u8 rec2_w2 = rec2_area_w & 0xff;

           u8 rec2_h1 = rec2_area_h >> 8;
           u8 rec2_h2 = rec2_area_h & 0xff;


           u8 rec3_p_x1 = rec3_p_x >> 8;
           u8 rec3_p_x2 = rec3_p_x & 0xff;

           u8 rec3_p_y1 = rec3_p_y >> 8;
           u8 rec3_p_y2 = rec3_p_y & 0xff;

           u8 rec3_w1 = rec3_area_w >> 8;
           u8 rec3_w2 = rec3_area_w & 0xff;

           u8 rec3_h1 = rec3_area_h >> 8;
           u8 rec3_h2 = rec3_area_h & 0xff;


           u8 rec4_p_x1 = rec4_p_x >> 8;
           u8 rec4_p_x2 = rec4_p_x & 0xff;

           u8 rec4_p_y1 = rec4_p_y >> 8;
           u8 rec4_p_y2 = rec4_p_y & 0xff;

           u8 rec4_w1 = rec4_area_w >> 8;
           u8 rec4_w2 = rec4_area_w & 0xff;

           u8 rec4_h1 = rec4_area_h >> 8;
           u8 rec4_h2 = rec4_area_h & 0xff;






           if(rec1_show)
           {
               UC8177_DTMW(rec1_p_x1, rec1_p_x2, rec1_p_y1, rec1_p_y2, rec1_w1, rec1_w2, rec1_h1, rec1_h2);       // data window setting    p_x -> start point x achse;  p_y -> start point y achse;
           printf("rec1_p_x1 = %x\n", rec1_p_x1);
           printf("rec1_p_x2 = %x\n", rec1_p_x2);
           printf("rec1_p_y1 = %x\n", rec1_p_y1);
           printf("rec1_p_y2 = %x\n", rec1_p_y2);
           printf("rec1_w1 = %x\n", rec1_w1);
           printf("rec1_w2 = %x\n", rec1_w2);
           printf("rec1_h1 = %x\n", rec1_h1);
           printf("rec1_h2 = %x\n", rec1_h2);
           //////////////////////////DTM1////////////////////
           unsigned long rec1_sum =  rec1_area_w * rec1_area_h / 8 ;
           printf("rec1_sum = %d\n", rec1_sum);
              //////////////////////////DTM1////////////////////


           gpio_set_value_lo(SPI_CS);

           spi_write_read_byte(0x10);
          // cur_bpp = 0x00;  // 00b: 1bpp
           spi_write_read_byte(0x00);


             unsigned long rec1_i;
             for (rec1_i = rec1_sum; rec1_i > 0; rec1_i-- )
             {
                 spi_write_read_byte(0x00);
             }

           gpio_set_value_hi(SPI_CS);// write image pixel
           }



           if(rec2_show)
           {
           UC8177_DTMW(rec2_p_x1, rec2_p_x2, rec2_p_y1, rec2_p_y2, rec2_w1, rec2_w2, rec2_h1, rec2_h2);       // data window setting    p_x -> start point x achse;  p_y -> start point y achse;
           printf("rec2_p_x1 = %x\n", rec2_p_x1);
           printf("rec2_p_x2 = %x\n", rec2_p_x2);
           printf("rec2_p_y1 = %x\n", rec2_p_y1);
           printf("rec2_p_y2 = %x\n", rec2_p_y2);
           printf("rec2_w1 = %x\n", rec2_w1);
           printf("rec2_w2 = %x\n", rec2_w2);
           printf("rec2_h1 = %x\n", rec2_h1);
           printf("rec2_h2 = %x\n", rec2_h2);
           //////////////////////////DTM1////////////////////
           unsigned long rec2_sum =  rec2_area_w * rec2_area_h / 8 ;
           printf("rec2_sum = %d\n", rec2_sum);
              //////////////////////////DTM1////////////////////


           gpio_set_value_lo(SPI_CS);

           spi_write_read_byte(0x10);
          // cur_bpp = 0x00;  // 00b: 1bpp
           spi_write_read_byte(0x00);


             unsigned long rec2_i;
             for (rec2_i = rec2_sum; rec2_i > 0; rec2_i-- )
             {
                 spi_write_read_byte(0x00);
             }

           gpio_set_value_hi(SPI_CS);// write image pixel
           }



        if(rec3_show)
        {
           UC8177_DTMW(rec3_p_x1, rec3_p_x2, rec3_p_y1, rec3_p_y2, rec3_w1, rec3_w2, rec3_h1, rec3_h2);       // data window setting    p_x -> start point x achse;  p_y -> start point y achse;
           printf("rec3_p_x1 = %x\n", rec3_p_x1);
           printf("rec3_p_x2 = %x\n", rec3_p_x2);
           printf("rec3_p_y1 = %x\n", rec3_p_y1);
           printf("rec3_p_y2 = %x\n", rec3_p_y2);
           printf("rec3_w1 = %x\n", rec3_w1);
           printf("rec3_w2 = %x\n", rec3_w2);
           printf("rec3_h1 = %x\n", rec3_h1);
           printf("rec3_h2 = %x\n", rec3_h2);
           //////////////////////////DTM1////////////////////
           unsigned long rec3_sum =  rec3_area_w * rec3_area_h / 8 ;
           printf("rec3_sum = %d\n", rec3_sum);
              //////////////////////////DTM1////////////////////


           gpio_set_value_lo(SPI_CS);

           spi_write_read_byte(0x10);
          // cur_bpp = 0x00;  // 00b: 1bpp
           spi_write_read_byte(0x00);


             unsigned long rec3_i;
             for (rec3_i = rec3_sum; rec3_i > 0; rec3_i-- )
             {
                 spi_write_read_byte(0x00);
             }

           gpio_set_value_hi(SPI_CS);// write image pixel
        }








        if(rec4_show)
        {

           UC8177_DTMW(rec4_p_x1, rec4_p_x2, rec4_p_y1, rec4_p_y2, rec4_w1, rec4_w2, rec4_h1, rec4_h2);       // data window setting    p_x -> start point x achse;  p_y -> start point y achse;
           printf("rec4_p_x1 = %x\n", rec4_p_x1);
           printf("rec4_p_x2 = %x\n", rec4_p_x2);
           printf("rec4_p_y1 = %x\n", rec4_p_y1);
           printf("rec4_p_y2 = %x\n", rec4_p_y2);
           printf("rec4_w1 = %x\n", rec4_w1);
           printf("rec4_w2 = %x\n", rec4_w2);
           printf("rec4_h1 = %x\n", rec4_h1);
           printf("rec4_h2 = %x\n", rec4_h2);
           //////////////////////////DTM1////////////////////
           unsigned long rec4_sum =  rec4_area_w * rec4_area_h / 8 ;
           printf("rec4_sum = %d\n", rec4_sum);
              //////////////////////////DTM1////////////////////


           gpio_set_value_lo(SPI_CS);

           spi_write_read_byte(0x10);
          // cur_bpp = 0x00;  // 00b: 1bpp
           spi_write_read_byte(0x00);


             unsigned long rec4_i;
             for (rec4_i = rec4_sum; rec4_i > 0; rec4_i-- )
             {
                 spi_write_read_byte(0x00);
             }

           gpio_set_value_hi(SPI_CS);// write image pixel
        }
























           UC8177_PON();    // power on


           UC8177_wait_for_BUSY_inactive();
       //    mdelay(5);
       //    flag_check(0);
           //////////////////////////DTM1////////////////////
                                      // check from busy pin

           UC8177_DRF(0x0C, 0x00, 0x00, 0x00, 0x00, 0x02, 0xA0, 0x01, 0xF0);
           UC8177_wait_for_BUSY_inactive();
           UC8177_POF();



       }





