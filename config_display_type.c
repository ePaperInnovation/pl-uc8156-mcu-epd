/*
 * config_display_type.c
 *
 *  Created on: 9 Sep 2016
 *      Author: andreas.meier
 */

#include <string.h>
#include <stdlib.h>

#include "config_display_type.h"
#include "utils.h"

u16 GATE_LINES; //global variable
u16 SOURCE_LINES; //global variable
u16 PIXEL_COUNT; //global variable
regSetting_t *REG_SETTINGS; //global variable
u8 NUMBER_OF_REGISTER_OVERWRITES; //global variable

u8 *image_data; //global variable
char PATH[64]; //global variable

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

void set_display_type(int display_type)
{
	switch (display_type)
	{
		case S014_T1_1:
			GATE_LINES = 180;
			SOURCE_LINES = 100;
			REG_SETTINGS = reg_settings_S014_T1_1;
			NUMBER_OF_REGISTER_OVERWRITES = sizeof(reg_settings_S014_T1_1)/sizeof(regSetting_t);
			strcpy(PATH, "S014_T1.1");

			break;
		case S031_T1_1:
			GATE_LINES = 148;
			SOURCE_LINES = 156;
			REG_SETTINGS = reg_settings_S031_T1_1;
			NUMBER_OF_REGISTER_OVERWRITES = sizeof(reg_settings_S031_T1_1)/sizeof(regSetting_t);
			strcpy(PATH, "S031_T1.1");

			break;
		case S011_T1_1:
			GATE_LINES = 70;
			SOURCE_LINES = 148;
			REG_SETTINGS = reg_settings_S011_T1_1;
			NUMBER_OF_REGISTER_OVERWRITES = sizeof(reg_settings_S011_T1_1)/sizeof(regSetting_t);
			strcpy(PATH, "S011_T1.1");

			break;
		default:
			abort_now("Fatal error in: config_display_type.c -> set_display_type -> display_type unknown.", ABORT_DISP_INFO);
	}

	PIXEL_COUNT = GATE_LINES * SOURCE_LINES;
	image_data = (u8 *) malloc(PIXEL_COUNT/4);
	if (image_data == NULL)
		abort_now("Fatal error in: config_display_type.c -> set_display_type -> malloc for image_data failed.", ABORT_DISP_INFO);
}
