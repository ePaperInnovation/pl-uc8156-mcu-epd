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
 * UC8179_MTP.h
 *
 *  Created on: 08.04.2021
 *  Author: Zhongke.Dai & Matti.Haugwitz
 */

#ifndef UC8179_MTP_H_
#define UC8179_MTP_H_

#include "types.h"
#include "config_display_type.h"
#include <stddef.h>

#define UC8179_DISPLAY_SOURCE 800
#define UC8179_DISPLAY_GATE 600



#define TR0_START    0x0049
#define TR1_START    0x0140
#define TR2_START    0x0237
#define TR3_START    0x032E
#define TR4_START    0x0425
#define TR5_START    0x051C
#define TR6_START    0x0613
#define TR7_START    0x070A
#define TR8_START    0x0801
#define TR9_START    0x08F8
#define TR10_START   0x09EF
#define TR11_START   0x0AE6

#define KWR_MODE_SETTING_LENGTH 7
#define KWR_LUTBD_LENGTH        42
#define KWR_LUTC_LENGTH         60
#define KWR_LUTR_LENGTH         60
#define KWR_LUTW_LENGTH         60
#define KWR_LUTK_LENGTH         60



enum TR_NUMBER  {TR0 = 0x00, TR1= 0x01, TR2= 0x02, TR3= 0x03, TR4= 0x04, TR5= 0x05, TR6= 0x06, TR7= 0x07, TR8= 0x08, TR9= 0x09, TR10= 0x0A, TR11= 0x0B  };
enum TR_KWR_TYPE {MODE_LUT_SETTING= 0x00, MODE_LUTC = 0x01, MODE_LUTR =0x02,MODE_LUTW = 0x03, MODE_LUTK = 0x04 };
enum OTP_BANK {BANK1 = 0x01, BAN2=0x02};


void UC8179_image_WHITE(void);
void UC8179_image_WHITE2(void);
void UC8179_image_BLACK(void);
void UC8179_image_BLACK2(void);
void UC8179_KW_MODE_REGISTER();
void UC8179_KW_MODE_OTP();
void UC8179_KWR_MODE_REGISTER();
void UC8179_KWR_MODE_OTP();
void UC8179_OTP_Register_Value_Read(int index_start, int index_end);
void UC8179_KWR_OTP_Register_COMMAND_SETTING_Value_Read();

int UC8179_KWR_OTP_LUTTYPE_STRAT(u8 LUC_TYPE, u8 TRN);
int UC8179_KWR_OTP_LUTTYPE_END(u8 LUC_TYPE, u8 TRN);
void UC8179_KWR_OTP_Register_TR_SETTING_Value_Read(u8 TRN);
void UC8179_KWR_OTP_Register_TR_LUTC_Read(u8 TRN);
void UC8179_KWR_OTP_Register_TR_LUTR_Read(u8 TRN);
void UC8179_KWR_OTP_Register_TR_LUTW_Read(u8 TRN);
void UC8179_KWR_OTP_Register_TR_LUTK_Read(u8 TRN);
void UC8179_LUT_INI_SETTING(u8 *LUTBD_ARRAY ,u8 *LUTC_ARRAY, u8 *LUTWW_ARRAY, u8 *LUTR_ARRAY, u8 *LUTW_ARRAY, u8 *LUTK_ARRAY);
void UC8179_KWR_OTP_Register_LUTBD_SETTING_Value_Read();


void UC8179_MANUAL_INI(void);

void UC8179_OTP_BANK(u8 bank_nr);

void UC8179_WRITE_COMMAND_1PARAM(u8 commad, u8 param1);
void UC8179_WRITE_COMMAND_2PARAM(u8 commad, u8 param1, u8 param2);
void UC8179_WRITE_COMMAND_3PARAM(u8 commad, u8 param1, u8 param2, u8 param3);
void UC8179_WRITE_COMMAND_4PARAM(u8 commad, u8 param1, u8 param2, u8 param3, u8 param4);
void UC8179_WRITE_COMMAND_5PARAM(u8 commad, u8 param1, u8 param2, u8 param3, u8 param4, u8 param5);
void UC8179_WRITE_COMMAND_9PARAM(u8 commad, u8 param1, u8 param2, u8 param3, u8 param4, u8 param5, u8 param6, u8 param7, u8 param8, u8 param9);


void UC8179_PSR_PARAMETER(u8 param1);
void UC8179_PWR_PARAMETER(u8 param1, u8 param2, u8 param3, u8 param4, u8 param5);
void UC8179_BTST_PARAMETER( u8 param1, u8 param2, u8 param3, u8 param4);
void UC8179_KWOPT_PARAMETER(u8 param1, u8 param2, u8 param3);
void UC8179_CDI_PARAMETER(u8 param1, u8 param2);
void UC8179_TCON_PARAMETER(u8 param1);
void UC8179_TRES_PARAMETER(u8 param1, u8 param2, u8 param3, u8 param4);
void UC8179_GSST_PARAMETER(u8 param1, u8 param2, u8 param3, u8 param4);
void UC8179_PWT_PARAMETER(u8 param1);
void UC8179_LVSEL_PARAMETER(u8 param1);
void UC8179_TSBDRY_PARAMETER(u8 param1);
void UC8179_PLL_PARAMETER(u8 param1);
void UC8179_EVS_PARAMETER(u8 param1);
void UC8179_LUTOPT_PARAMETER(u8 param1, u8 param2);
void UC8179_VDCS_PARAMETER(u8 param1);
bool UC8179_BUSY_N_CHECK();
void UC8179_byte_array_WRITE1(u8 *byte_array, unsigned long  array_length);
void UC8179_byte_array_WRITE2(u8 *byte_array, unsigned long  array_length);
void UC8179_OTP_WRITE(u8 *byte_packet, unsigned long packet_length);
void UC8179_GATESETTING_PARAMETER(u8 param1);
void UC8179_PTL_PARAMETER(u8 param1, u8 param2,u8 param3,u8 param4,u8 param5,u8 param6,u8 param7, u8 param8, u8 param9);
void UC8179_PTIN_PARAMETER();
void UC8179_PTOUT_PARAMETER();

void UC8171_MANUAL_INI(void);
void UC8171_BTST_PARAMETER( u8 param1, u8 param2, u8 param3);
void UC8171_TRES_PARAMETER(u8 param1, u8 param2, u8 param3);
void UC8171_GSST_PARAMETER(u8 param1, u8 param2, u8 param3);
void UC8171_image_WHITE(void);
void UC8171_image_WHITE2(void);
void UC8171_image_BLACK(void);
void UC8171_image_BLACK2(void);
void UC8179_byte_array_WRITE2_Part(u8 *byte_array, unsigned long array_length);

#endif /* UC8179_MTP_H_ */
