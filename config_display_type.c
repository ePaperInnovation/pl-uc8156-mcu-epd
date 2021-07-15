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
 * config_display_type.c
 *
 *  Created on: 9 Sep 2016
 *      Author: andreas.meier
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "config_display_type.h"
#include "utils.h"
#include "read-sd.h"
#include "types.h"

u16 GATE_LINES; //global variable
u16 SOURCE_LINES; //global variable
u16 PIXEL_COUNT; //global variable
u16 NEXTLINE;
regSetting_t *REG_SETTINGS; //global variable
u8 NUMBER_OF_REGISTER_OVERWRITES; //global variable
u8 *image_data; //global variable
char PATH[64]; //global variable
bool LINE_SHARING = false; //global variable
volatile int single_display = 1;

//#pragma RETAIN(reg_settings_S011_T1_1)
//#pragma DATA_SECTION(reg_settings_S011_T1_1, ".myRegsetting");
//volatile const regSetting_t reg_settings_S011_T1_1[] =
regSetting_t reg_settings_S011_T1_1[] =
{
		{0x01, 1, {0x12}},
		{0x02, 2, {0x25, 0xff}},
		{0x06, 2, {0x67, 0x55}},
		{0x07, 1, {0x0a}},
		{0x0c, 4, {0x00, 0xef, 0x00, 0x9f}}, // {0, 240-1, 0, 160-1}
		{0x0d, 4, {0x00, 0x45, 0x00, 0x93}}, // {0, 70-1, 0, 148-1}
		{0x0e, 2, {0x00, 0x93}},             // {0, 148-1}
		{0x0f, 1, {0x02}},
		{0x18, 4, {0x00, 0x00, 0x24, 0x07}},
		{0x1d, 1, {0x04}},
		{0x1f, 3, {0x00, 0x00, 0x00}},
		{0x44, 1, {0x60}}
};

regSetting_t reg_settings_S011_T1_2[] =
{
		{0x01, 1, {0x12}},
		{0x02, 2, {0x70, 0xaa}},			// set Vgate to +17V/-25V
		{0x06, 2, {0xff, 0x55}},			// set timing to LAT=105, S2G+G2S=5
		{0x07, 1, {0x0a}},
		{0x0c, 4, {0x00, 0x45, 0x00, 0x93}}, // physical 240x160 panel resolution setting
		{0x0d, 4, {0x00, 0x45, 0x00, 0x93}}, // {0, 70-1, 0, 148-1}
		{0x0e, 2, {0x00, 0x93}},             // {0, 148-1}
		{0x0f, 1, {0x02}},
		{0x18, 4, {0x00, 0x00, 0x24, 0x07}},
		{0x1d, 1, {0x04}},
		{0x1f, 3, {0x00, 0x00, 0x00}},
		{0x44, 1, {0x60}}
};

regSetting_t reg_settings_S014_T1_1[] =
{
		{0x01, 1, {0x12}},
		{0x02, 2, {0x25, 0xff}},
		{0x06, 2, {0x67, 0x55}},
		{0x07, 1, {0x0a}},
		{0x0c, 4, {0, 0xef, 0, 0x9f}},         // {0, 240-1, 0, 160-1}
		{0x0d, 4, {0, 0xb3, 0x3c, 0x9f}},      // {0, 180-1, 60, 160-1}
		{0x0e, 2, {0, 0x9f}},                  // {0, 60}
		{0x0f, 1, {0x02}},
		{0x18, 4, {0x00, 0x00, 0x24, 0x07}},
		{0x1d, 1, {0x04}},
		{0x1f, 3, {0x00, 0x00, 0x00}},
		{0x44, 1, {0x60}}
};

regSetting_t reg_settings_S014_T1_2[] =
{
		{0x01, 1, {0x12}},
		{0x02, 2, {0x70, 0xaa}},				// set Vgate to +17V/-25V
		{0x06, 2, {0xff, 0x55}},				// set timing to LAT=105, S2G+G2S=5
		{0x07, 1, {0x0a}},
		{0x0c, 4, {0, 0xb3, 0x3c, 0x9f}},       // physical 240x160 panel resolution setting
		{0x0d, 4, {0, 0xb3, 0x3c, 0x9f}},      // {0, 180-1, 60, 160-1}
		{0x0e, 2, {0, 0x9f}},                  // {0, 60}
		{0x0f, 1, {0x02}},
		{0x18, 4, {0x00, 0x00, 0x24, 0x07}},
		{0x1d, 1, {0x04}},
		{0x1f, 3, {0x00, 0x00, 0x00}},
		{0x44, 1, {0x60}}
};







regSetting_t reg_settings_S017_T1_1[] =                       // Display 1.69 for Leitner
{
        {0x00, 1, {0x07}},                                    // PSR:  0x07: LUT from OTP (Default); 0x27: LUT from register (test)
        {0x01, 5, {0x07, 0x17, 0x3F, 0x3F, 0x0E}},            // PWR DKE 0x01: 0x07, 0x17, 0x3F, 0x3F, 0x0E
        {0x06, 4, {0x27, 0x27, 0x28, 0x17}},                  // BTST DKE 0x06: 0x27, 0x27, 0x28, 0x17
        {0x2B, 4, {0x00, 0x00, 0x00, 0x00}},                  // KWOPT DKE 0x2B: 0x00, 0x00, 0x00, 0x00
        {0x50, 2, {0x11, 0x07}},                              // CDI DKE 0x50: 0x11, 0x07
        {0x60, 1, {0x22}},                                    // TCON DKE 0x60: 0x22
        {0x61, 4, {0x02, 0x88, 0x01, 0xE0}},                  // TRES DKE 0x61: 0x02, 0x88, 0x01, 0xE0    DKE:648x480; Safepass: 400x648??
        {0x65, 4, {0x00, 0x10, 0x00, 0x00}},                  // GSST DKE 0x65: 0x00, 0x10, 0x00, 0x00
        {0xE3, 1, {0x00}},                                    // PWT DKE 0xE3: 0x00
        {0xE4, 1, {0x03}},                                    // LVSEL DKE 0XE4: 0x03
        {0xE7, 1, {0x00}},                                    // TSBDRY DKE: 0x00
        {0x30, 1, {0x06}},                                    // PLL DKE: 0x06
        {0x52, 1, {0x00}},                                    // EVS DKE: 0x00
        {0x2A, 2, {0x00, 0x00}},                              // LUTOP DKE: 0x00, 0x00
        {0x82, 1, {0x4F}}                                    // VDCS DKE: 0x1E
};






//#pragma RETAIN(reg_settings_S021_T1_1)
//#pragma DATA_SECTION(reg_settings_S021_T1_1, ".myRegsetting");
//volatile const regSetting_t reg_settings_S021_T1_1[] =
regSetting_t reg_settings_S021_T1_1[] =
{
		{0x01, 1, {0x10}},
		{0x02, 2, {0x25, 0xff}},
		{0x06, 2, {0x67, 0x55}},
		{0x07, 1, {0x0a}},
		{0x0c, 4, {0, 0xef, 0, 0x9f}},         // {0, 240-1, 0, 160-1}
		{0x0d, 4, {0, 240-1, 0x00, 146-1}},      // {0, 180-1, 60, 160-1}
		{0x0e, 2, {0, 0}},                  // {0, 0}
		{0x0f, 1, {0x20}},
		{0x18, 4, {0x00, 0x00, 0x24, 0x07}},   // for BW or tricolor
    //    {0x18, 4, {0x00, 0x00, 0x24, 0x05}},   // for Acep
		{0x1d, 1, {0x04}},
		{0x1f, 3, {0x00, 0x00, 0x00}},
		{0x44, 1, {0x60}}

};

regSetting_t reg_settings_S025_T1_1[] =
{
        {0x01, 1, {0x12}},
        {0x02, 2, {0x70, 0xff}},
        {0x06, 2, {0x67, 0x55}},
        {0x07, 1, {0x0a}},
        {0x0c, 4, {0, 0xef, 0, 0x9f}},
        {0x0d, 4, {0, 0xef, 0x00, 0x9f}},
        {0x0e, 2, {0, 0}},
        {0x0f, 1, {0x00}},
        {0x18, 4, {0x00, 0x00, 0x24, 0x07}},
        {0x1d, 1, {0x04}},
        {0x1f, 3, {0x00, 0x00, 0x00}},
        {0x44, 1, {0x60}}
};

regSetting_t reg_settings_S031_T1_1[] =
{
		{0x01, 1, {0x12}},
		{0x02, 2, {0x25, 0xff}},
		{0x06, 2, {0x67, 0x55}},
		{0x07, 1, {0x0a}},
		{0x0c, 4, {0x00, 0xef, 0x00, 0x9f}}, // {0, 240-1, 0, 160-1}
		{0x0d, 4, {0x00, 0x93, 0x00, 0x9b}}, //{0, 148-1, 0, 156-1}
		{0x0e, 2, {0x00, 0x9b}},             //{0, 156-1}
		{0x0f, 1, {0x22}},
		{0x18, 4, {0x50,0x01,0x24,0x07}},
		{0x1d, 1, {0x04}},
		{0x1f, 3, {0x00, 0x00, 0x00}},
		{0x44, 1, {0x60}}
};





regSetting_t reg_settings_S036_T1_1[] =                       // Display 3.57 for Safepass
{
        {0x00, 1, {0x07}},                                    // PSR:  0x07: LUT from OTP (Default); 0x27: LUT from register (test)
        {0x01, 5, {0x07, 0x17, 0x3F, 0x3F, 0x0E}},            // PWR DKE 0x01: 0x07, 0x17, 0x3F, 0x3F, 0x0E
        {0x06, 4, {0x27, 0x27, 0x28, 0x17}},                  // BTST DKE 0x06: 0x27, 0x27, 0x28, 0x17
        {0x2B, 4, {0x00, 0x00, 0x00, 0x00}},                  // KWOPT DKE 0x2B: 0x00, 0x00, 0x00, 0x00
        {0x50, 2, {0x11, 0x07}},                              // CDI DKE 0x50: 0x11, 0x07
        {0x60, 1, {0x22}},                                    // TCON DKE 0x60: 0x22
        {0x61, 4, {0x02, 0x88, 0x01, 0xE0}},                  // TRES DKE 0x61: 0x02, 0x88, 0x01, 0xE0    DKE:648x480; Safepass: 400x648??
        {0x65, 4, {0x00, 0x10, 0x00, 0x00}},                  // GSST DKE 0x65: 0x00, 0x10, 0x00, 0x00
        {0xE3, 1, {0x00}},                                    // PWT DKE 0xE3: 0x00
        {0xE4, 1, {0x03}},                                    // LVSEL DKE 0XE4: 0x03
        {0xE7, 1, {0x00}},                                    // TSBDRY DKE: 0x00
        {0x30, 1, {0x06}},                                    // PLL DKE: 0x06
        {0x52, 1, {0x00}},                                    // EVS DKE: 0x00
        {0x2A, 2, {0x00, 0x00}},                              // LUTOP DKE: 0x00, 0x00
        {0x82, 1, {0x4F}}                                    // VDCS DKE: 0x1E
};







regSetting_t reg_settings_D011_T1_1[] =
{
		{0x01, 1, {0x10}}, //GVS=01b (gate active = VGL), SOO=0
		{0x02, 2, {0x25, 0xff}},
		{0x06, 2, {0x67, 0x55}},
		{0x07, 1, {0x0a}},
		{0x0c, 4, {0x00, 0xef, 0x00, 0x9f}},      // {0, 240-1, 0, 160-1}
		{0x0d, 4, {0x00, 0xef, 0x00, 0x73}},      // {0, 240-1, 0, 116-1}
		{0x0e, 2, {0x00, 0x00}},                  // {0, 0}
		{0x0f, 1, {0x20}}, //X-increment, Y-increment
		{0x18, 4, {0x00, 0x00, 0x24, 0x07}}, // TPCOM end of update -> GND, TPCOM_idle -> VCOM -> 70.0L*
		{0x1d, 1, {0x04}},
		{0x1f, 3, {0x00, 0x00, 0x00}},
		{0x44, 1, {0x60}}
};





void set_display_type(int display_type)
{
	switch (display_type)
	{
		case S011_T1_1:
			GATE_LINES = 70;
			SOURCE_LINES = 148;
			REG_SETTINGS = reg_settings_S011_T1_1;





			NUMBER_OF_REGISTER_OVERWRITES = sizeof(reg_settings_S011_T1_1)/sizeof(regSetting_t);
//			uint16_t i;
//			for (i = 0; i < NUMBER_OF_REGISTER_OVERWRITES; i++)
//			{
//			    REG_SETTINGS[i] = reg_settings_S011_T1_1[i];
//			}




			strcpy(PATH, "S011_T1.1");
			read_image_data_from_file = read_image_data_from_file_S011_T1;

			break;
		case S011_T1_2:
			GATE_LINES = 70;
			SOURCE_LINES = 148;
			REG_SETTINGS = reg_settings_S011_T1_2;
			NUMBER_OF_REGISTER_OVERWRITES = sizeof(reg_settings_S011_T1_2)/sizeof(regSetting_t);
			strcpy(PATH, "S011_T1.2");
			read_image_data_from_file = read_image_data_from_file_default;

			break;
		case S014_T1_1:
			GATE_LINES = 180;
			SOURCE_LINES = 100;
			REG_SETTINGS = reg_settings_S014_T1_1;
			NUMBER_OF_REGISTER_OVERWRITES = sizeof(reg_settings_S014_T1_1)/sizeof(regSetting_t);
			strcpy(PATH, "S014_T1.1");
			read_image_data_from_file = read_image_data_from_file_default;

			break;
		case S014_T1_2:
			GATE_LINES = 180;
			SOURCE_LINES = 100;
			REG_SETTINGS = reg_settings_S014_T1_2;
			NUMBER_OF_REGISTER_OVERWRITES = sizeof(reg_settings_S014_T1_2)/sizeof(regSetting_t);
			strcpy(PATH, "S014_T1.2");
			read_image_data_from_file = read_image_data_from_file_default;

			break;
		case S021_T1_1:
			GATE_LINES = 146;
			SOURCE_LINES = 240;
			REG_SETTINGS = reg_settings_S021_T1_1;
			NUMBER_OF_REGISTER_OVERWRITES = sizeof(reg_settings_S021_T1_1)/sizeof(regSetting_t);
			strcpy(PATH, "S021_T1.1");
			read_image_data_from_file = read_image_data_from_file_S021_T1;

			break;

		case S025_T1_1:
            GATE_LINES = 160;
            SOURCE_LINES = 240;
            REG_SETTINGS = reg_settings_S025_T1_1;
            NUMBER_OF_REGISTER_OVERWRITES = sizeof(reg_settings_S025_T1_1)/sizeof(regSetting_t);
            strcpy(PATH, "S025_T1.1");
            read_image_data_from_file = read_image_data_from_file_default;

            break;
		case S031_T1_1:
		    GATE_LINES = 156;  //GATE_LINES = 312
		    SOURCE_LINES = 148;;//SOURCE_LINES = 74;
			REG_SETTINGS = reg_settings_S031_T1_1;
			NUMBER_OF_REGISTER_OVERWRITES = sizeof(reg_settings_S031_T1_1)/sizeof(regSetting_t);
			strcpy(PATH, "S031_T1.1");
			LINE_SHARING = true;
			read_image_data_from_file = read_image_data_from_file_S031_T1;

			break;
		case D011_T1_1:
			GATE_LINES = 116;
			SOURCE_LINES = 240;
			REG_SETTINGS = reg_settings_D011_T1_1;
			NUMBER_OF_REGISTER_OVERWRITES = sizeof(reg_settings_D011_T1_1)/sizeof(regSetting_t);
			strcpy(PATH, "D011_T1.1");
			single_display = 0;
			read_image_data_from_file = read_image_data_from_file_D011_T1;


        case S017_T1_1:
            GATE_LINES = 116;    // still not set
            SOURCE_LINES = 240;  // still not set
            REG_SETTINGS = reg_settings_S017_T1_1;
            NUMBER_OF_REGISTER_OVERWRITES = sizeof(reg_settings_S017_T1_1)/sizeof(regSetting_t);
            strcpy(PATH, "S017_T1.1");
            single_display = 0;
            read_image_data_from_file = read_image_data_from_file_default;



        case S036_T1_1:
            GATE_LINES = 400;       // not sure
            SOURCE_LINES = 640;     // not sure
            REG_SETTINGS = reg_settings_S036_T1_1;
            NUMBER_OF_REGISTER_OVERWRITES = sizeof(reg_settings_S036_T1_1)/sizeof(regSetting_t);
            strcpy(PATH, "S036_T1.1");
            single_display = 0;
            read_image_data_from_file = read_image_data_from_file_default;


			break;
		default:
			abort_now("Fatal error in: config_display_type.c -> set_display_type -> display_type unknown.", ABORT_DISP_INFO);
	}

	if( (display_type != S036_T1_1) || S017_T1_1 )
	{
	    PIXEL_COUNT = GATE_LINES * SOURCE_LINES;
	    NEXTLINE = SOURCE_LINES / 4;
	    image_data = (u8 *) malloc(PIXEL_COUNT/4);
	    if (image_data == NULL)
		abort_now("Fatal error in: config_display_type.c -> set_display_type -> malloc for image_data failed.", ABORT_DISP_INFO);
	}
	else
	{
	    if(display_type == S036_T1_1)
	    {
	        image_data = (u8 *) malloc( (GATE_LINES * SOURCE_LINES/8) / 4 );
	        if (image_data == NULL)
	           abort_now("Fatal error in: config_display_type.c -> set_display_type -> malloc for image_data failed.", ABORT_DISP_INFO);
	    }
	}

}
