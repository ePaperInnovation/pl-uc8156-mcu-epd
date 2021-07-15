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
 * display_functions.c
 *
 *  Created on: 24 Aug 2016
 *      Author: andreas.meier
 */

#include "msp430/msp430-spi.h"
#include "msp430/msp430-gpio.h"
#include "UC8156.h"
#include "read-sd.h"
#include "utils.h"
#include "config_display_type.h"
#include "image.h"

extern u16 PIXEL_COUNT;
extern u8 *image_data;

extern u16 GATE_LINES; //global variable
extern u16 SOURCE_LINES; //global variable
extern u16 NEXTLINE;


void clear_display()
{
	u8 reg0fh_value = spi_read_command_1param(0x0f);

	spi_write_command_1param(0x0f, reg0fh_value|0x10); //
	UC8156_send_repeated_image_data(0xff); // 0xff is white
	spi_write_command_1param(0x0f, reg0fh_value&(~0x10)); //
	UC8156_send_repeated_image_data(0xff); // 0xff is white
	if(single_display){
		UC8156_HVs_on();
		UC8156_update_display(FULL_UPDATE, NORMAL_4GL);
		UC8156_update_display(FULL_UPDATE, NORMAL_4GL);
		//UC8156_update_display(INIT_UPDATE);
		UC8156_HVs_off();
	}else{
		spi_write_command_1param_slave(0x0f, reg0fh_value|0x10); //
		UC8156_send_repeated_image_data_slave(0xff); // 0xff is white
		spi_write_command_1param_slave(0x0f, reg0fh_value&(~0x10)); //
		UC8156_send_repeated_image_data_slave(0xff); // 0xff is white

		spi_write_command_1param_slave(0x03, 0xc6); //Enable external HV supply for Slave

		UC8156_HVs_on_dual();

		UC8156_update_display_dual(FULL_UPDATE, NORMAL_4GL);
		UC8156_update_display_dual(FULL_UPDATE, NORMAL_4GL);

		UC8156_HVs_off_dual();
	}
}

// loads image from SD-card and updates it on the display using a 4GL FULL update
void show_image_from_SDcard(char *image, int mode)
{
	sdcard_load_image(image, image_data);

   	UC8156_show_image(image_data, mode, NORMAL_4GL);
}



void show_image_from_SDcard_inv(char *image, int mode, bool inv_bool)
{

    sdcard_load_image(image, image_data);

    if(!inv_bool)
    {
        UC8156_show_image(image_data, mode, NORMAL_4GL);

    }
    else
    {
        UC8156_show_image_inv(image_data, mode, NORMAL_4GL);
    }
}








void show_image_from_SDcard_all_set(char *image, int mode,  u8 transparency_key_value, u8 transparency_display_enable, u8 display_mode_select, bool inv_bool)
{

    sdcard_load_image(image, image_data);

    UC8156_show_image_all_set( image_data, mode, NORMAL_4GL, transparency_key_value, transparency_display_enable, display_mode_select, inv_bool);
}





void show_image_from_SDcard_GL(char *image, int mode, int GL_name)
{
    sdcard_load_image(image, image_data);
    UC8156_show_image_GL(image_data, mode, NORMAL_4GL, GL_name);

}

void show_image_from_GL(int mode, int GL_name)
{
    UC8156_show_image_GL(image_data, mode, NORMAL_4GL, GL_name);

}

void show_image_from_SDcard_bg(char *image, int mode)
{
         sdcard_load_image(image, image_data);
         UC8156_show_image(image_data, mode, NORMAL_4GL);
}




// loads image from SD-card and updates it on the display using given update mode and waveform table
void show_image_from_SDcard_V2(char *image, int mode, int waveform_table)
{
	sdcard_load_image(image, image_data);

   	UC8156_show_image(image_data, mode, waveform_table);
}

void show_image_from_SDcard_dual(char *image, int mode)
{
 	sdcard_load_image(image, image_data);
 	UC8156_show_image_dual(image_data, mode, NORMAL_4GL);
}

void tri_color_display()
{
    u8 reg0fh_value = spi_read_command_1param(0x0f);

        spi_write_command_1param(0x0f, reg0fh_value|0x10); //
        UC8156_send_repeated_image_data(0x00); // 0xff is white
        spi_write_command_1param(0x0f, reg0fh_value&(~0x10)); //
        UC8156_send_repeated_image_data(0x00); // 0xff is white
        if(single_display)
        {
            UC8156_HVs_on();
            UC8156_update_display(FULL_UPDATE, NORMAL_4GL);
          //  UC8156_update_display(FULL_UPDATE, NORMAL_4GL);
            //UC8156_update_display(INIT_UPDATE);
            UC8156_HVs_off();
        }
}




int getPixel(int x, int y)
{
    if ((x < 0) || (x >= SOURCE_LINES) || (y < 0) || (y >= GATE_LINES)) return 5;

    uint16_t byteIndex = x/4 + (y) * NEXTLINE + 10;
    switch (x%4) {
        case 0: return ((unsigned int)(my_image[byteIndex] & 0xC0) >> 6);
        case 1: return ((unsigned int)(my_image[byteIndex] & 0x30) >> 4);
        case 2: return ((unsigned int)(my_image[byteIndex] & 0x0C) >> 2);
        case 3: return ((unsigned int)(my_image[byteIndex] & 0x03));
        }
  return 0;
}


void drawPixel( int x, int y, int color)
{
   if ((x < 0) || (x >= SOURCE_LINES) || (y < 0) || (y >= GATE_LINES) || (color>4 )) return;

   uint16_t byteIndex = x/4 + (y) * NEXTLINE;
    uint8_t pixels = image_data[byteIndex];
  switch (x%4) {                      //2-bit grayscale dot
      case 0: image_data[byteIndex] = (pixels & 0x3F) | ((uint8_t)color << 6); break;
      case 1: image_data[byteIndex] = (pixels & 0xCF) | ((uint8_t)color << 4); break;
      case 2: image_data[byteIndex] = (pixels & 0xF3) | ((uint8_t)color << 2); break;
      case 3: image_data[byteIndex] = (pixels & 0xFC) | (uint8_t)color; break;
  }
}


void image_scramble_from_Memory(int display_type)
{
    //display_type = 0;
    switch (display_type) {

                  case S021_T1_1:
                      {
                          uint16_t x, y;
                        for (y=0; y<146; y++)
                        {                   // for each gateline...
                             for (x=0; x<240/2; x++)
                             {             // for each sourceline...
                               int color1 = getPixel(x,y);
                               int color2 = getPixel(x+120,y);
                               drawPixel(239-x, y, color1);
                               drawPixel(x, y, color2);
                             }
                         }
                      }
                  break;


                  case S031_T1_1:
                      {
                          uint16_t x, y;
                        for (y=0; y<156; y++)
                        {                   // for each gateline...
                             for (x=0; x<74; x++)
                             {             // for each sourceline...
                               int color1 = getPixel(x + 74,y);
                               int color2 = getPixel(x,y);
                               drawPixel(x*2, y, color1);
                               drawPixel(x*2 + 1, y, color2);
                             }
                         }
                      }
                  break;



                 default:
                 {
                     uint16_t x;
                    for (x=0; x < PIXEL_COUNT / 4 ; x++)
                        {
                        image_data[x] =  my_image[x + 10];
                        }
                 }
                 break;
        }
}

void show_image_from_flash(int mode, bool inv_bool)
{

    if(!inv_bool)
      {
          UC8156_show_image(image_data, mode, NORMAL_4GL);
      }
      else
      {
          UC8156_show_image_inv(image_data, mode, NORMAL_4GL);
      }
}


