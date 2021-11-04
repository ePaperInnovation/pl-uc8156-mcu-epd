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
 * UC8179_MTP.c
 *
 *  Created on: 08.04.2021
 *      Author: Zhongke Dai & Matti Haugwitz
 */
//#define DEBUG_PRINTOUTS_ON

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <UC8179.h>
#include <UC8179_MTP.h>
#include <UC8179_ini.h>

#include "msp430/msp430-spi.h"
#include "read-sd.h"
#include "utils.h"
#include "config_display_type.h"

bool display_KWR;

void UC8179_KW_MODE_REGISTER()
{
    UC8179_PANEL_SETTING(LUT_FROM_REGISTER, PIXEL_KW_MODE, GATE_SCAN_UP, SOURCE_SHIFT_RIGHT, BOOSTER_ON, SOFT_RESET_NO_EFFECT);
}

void UC8179_KW_MODE_OTP()
{
    UC8179_PANEL_SETTING(LUT_FROM_OTP, PIXEL_KW_MODE, GATE_SCAN_UP, SOURCE_SHIFT_RIGHT, BOOSTER_ON, SOFT_RESET_NO_EFFECT);
}


void UC8179_KWR_MODE_REGISTER()
{
    UC8179_PANEL_SETTING(LUT_FROM_REGISTER, PIXEL_KWR_MODE, GATE_SCAN_UP, SOURCE_SHIFT_RIGHT, BOOSTER_ON, SOFT_RESET_NO_EFFECT);
}

void UC8179_KWR_MODE_OTP()
{
    UC8179_PANEL_SETTING(LUT_FROM_OTP, PIXEL_KWR_MODE, GATE_SCAN_UP, SOURCE_SHIFT_RIGHT, BOOSTER_ON, SOFT_RESET_NO_EFFECT);
}


void UC8179_image_WHITE(void)
{
    UC8179_spi_write_command(0x10);
   const unsigned long sum =  60000;
    unsigned long i;
    for (i = 0; i < sum; i++ )
    {
        UC8179_spi_write_parameter(0xFF);
    }

}

void UC8179_image_WHITE2(void)
{
    UC8179_spi_write_command(0x13);
   const unsigned long sum =  60000;
    unsigned long i;
    for (i = 0; i < sum; i++ )
    {
        UC8179_spi_write_parameter(0xFF);
    }

}

void UC8179_image_BLACK(void)
{
    UC8179_spi_write_command(0x10);
    const unsigned long sum = 60000;
    unsigned long i;
    for (i = 0; i < sum; i++ )
    {
        UC8179_spi_write_parameter(0x00);
    }

}





void UC8179_image_BLACK2(void)
{
    UC8179_spi_write_command(0x13);
    const  unsigned long sum = 60000;
   // const  unsigned long sum = 32000;
    unsigned long i;
    for (i = 0; i < sum; i++ )
    {
        UC8179_spi_write_parameter(0x00);
    }

}

void UC8179_byte_array_WRITE1(u8 *byte_array, unsigned long  array_length)
{
    UC8179_spi_write_command(0x10);
    unsigned long i;
    for (i = 0; i < array_length; i++ )
    {
        UC8179_spi_write_parameter(byte_array[i]);
    }
}

void UC8179_byte_array_WRITE2(u8 *byte_array, unsigned long array_length)
{
    UC8179_spi_write_command(0x13);
    unsigned long i;
    for (i = 0; i < array_length; i++ )
    {
        UC8179_spi_write_parameter(byte_array[i]);
    }
}


void UC8179_byte_array_WRITE2_Part(u8 *byte_array, unsigned long array_length)
{
    //UC8179_spi_write_command(0x13);
    unsigned long i;
    for (i = 0; i < array_length; i++ )
    {
        UC8179_spi_write_parameter(byte_array[i]);
    }
}








void UC8179_OTP_Register_Value_Read(int index_start, int index_end)
{

    UC8179_spi_write_command(0xA2);
      int i;
      printf("OTP Value: \n");
      for(i = 0; i<=index_end; i++)
      {
         u8  value = UC8179_spi_read_parameter();
         if(i >= index_start)
         {
             printf("0x%x\n", value);
         }

      }
}


void UC8179_KWR_OTP_Register_COMMAND_SETTING_Value_Read()
{
   const int index_start = 0x0C;
   const int index_end = 0x1E;
    UC8179_spi_write_command(0xA2);
      int i;
      printf("Command Default Setting: \n{");
      for(i = 0; i<=index_end; i++)
      {
         u8  value = UC8179_spi_read_parameter();
         if(i >= index_start &&i < index_end )
         {
             printf("0x%x,", value);
         }
         else if(i == index_end)
         {
             printf("0x%x} \n", value);
         }

      }

}



void UC8179_KWR_OTP_Register_LUTBD_SETTING_Value_Read()
{
   const int index_start = 0x1F;
   const int index_end = 0x48;
    UC8179_spi_write_command(0xA2);
      int i;
      printf("BORDER LUT Setting: \n{");
      for(i = 0; i<=index_end; i++)
      {
         u8  value = UC8179_spi_read_parameter();
         if(i >= index_start &&i < index_end )
         {
             printf("0x%x,", value);
         }
         else if(i == index_end)
         {
             printf("0x%x}", value);
         }

      }

}


void UC8179_KWR_OTP_Register_TR_SETTING_Value_Read(u8 TRN)
{
 int adr_start =  UC8179_KWR_OTP_LUTTYPE_STRAT( MODE_LUT_SETTING, TRN);
 int adr_end =  UC8179_KWR_OTP_LUTTYPE_END( MODE_LUT_SETTING, TRN);
 printf("KWR Mode TR%d: \n", TRN);
 printf("address start: 0x%x \n", adr_start);
 printf("address end: 0x%x \n", adr_end);


 UC8179_spi_write_command(0xA2);

 printf("Parameter Setting: \n{");
   int i;

   for(i = 0; i<=adr_end; i++)
   {
      u8  value = UC8179_spi_read_parameter();
      if(i >= adr_start &&i < adr_end )
      {
          printf("0x%x,", value);
      }
      else if(i == adr_end)
      {
          printf("0x%x}\n", value);
      }

   }





}

void UC8179_KWR_OTP_Register_TR_LUTC_Read(u8 TRN)
{
    int adr_start =  UC8179_KWR_OTP_LUTTYPE_STRAT( MODE_LUTC, TRN);
    int adr_end =  UC8179_KWR_OTP_LUTTYPE_END( MODE_LUTC, TRN);


    printf("KWR Mode TR%d: \n", TRN);
    printf("address start: 0x%x \n", adr_start);
    printf("address end: 0x%x \n", adr_end);


    UC8179_spi_write_command(0xA2);

    printf("LUTC Setting: \n{");
      int i;

      for(i = 0; i<=adr_end; i++)
      {
         u8  value = UC8179_spi_read_parameter();
         if(i >= adr_start &&i < adr_end )
         {
             printf("0x%x,", value);
         }
         else if(i == adr_end)
         {
             printf("0x%x}\n", value);
         }

      }






}

void UC8179_KWR_OTP_Register_TR_LUTR_Read(u8 TRN)
{
    int adr_start =  UC8179_KWR_OTP_LUTTYPE_STRAT( MODE_LUTR, TRN);
    int adr_end =  UC8179_KWR_OTP_LUTTYPE_END( MODE_LUTR, TRN);


    printf("KWR Mode TR%d: \n", TRN);
      printf("address start: 0x%x \n", adr_start);
      printf("address end: 0x%x \n", adr_end);


      UC8179_spi_write_command(0xA2);

      printf("LUTR Setting: \n{");
        int i;

        for(i = 0; i<=adr_end; i++)
        {
           u8  value = UC8179_spi_read_parameter();
           if(i >= adr_start &&i < adr_end )
           {
               printf("0x%x,", value);
           }
           else if(i == adr_end)
           {
               printf("0x%x}\n", value);
           }

        }

}

void UC8179_KWR_OTP_Register_TR_LUTW_Read(u8 TRN)
{
    int adr_start =  UC8179_KWR_OTP_LUTTYPE_STRAT( MODE_LUTW, TRN);
    int adr_end =  UC8179_KWR_OTP_LUTTYPE_END( MODE_LUTW, TRN);
    printf("KWR Mode TR%d: \n", TRN);
      printf("address start: 0x%x \n", adr_start);
      printf("address end: 0x%x \n", adr_end);


      UC8179_spi_write_command(0xA2);

      printf("LUTW Setting: \n{");
        int i;

        for(i = 0; i<=adr_end; i++)
        {
           u8  value = UC8179_spi_read_parameter();
           if(i >= adr_start &&i < adr_end )
           {
               printf("0x%x,", value);
           }
           else if(i == adr_end)
           {
               printf("0x%x}\n", value);
           }

        }
}

void UC8179_KWR_OTP_Register_TR_LUTK_Read(u8 TRN)
{
    int adr_start =  UC8179_KWR_OTP_LUTTYPE_STRAT( MODE_LUTK, TRN);
    int adr_end =  UC8179_KWR_OTP_LUTTYPE_END( MODE_LUTK, TRN);
    printf("KWR Mode TR%d: \n", TRN);
      printf("address start: 0x%x \n", adr_start);
      printf("address end: 0x%x \n", adr_end);


      UC8179_spi_write_command(0xA2);

      printf("LUTK Setting: \n{");
        int i;

        for(i = 0; i<=adr_end; i++)
        {
           u8  value = UC8179_spi_read_parameter();
           if(i >= adr_start &&i < adr_end )
           {
               printf("0x%x,", value);
           }
           else if(i == adr_end)
           {
               printf("0x%x}\n", value);
           }

        }
}







int UC8179_KWR_OTP_LUTTYPE_STRAT(u8 LUC_TYPE, u8 TRN)
{
    int ADR_START;

    int trn_start;
    switch(TRN)
    {
    case TR0:
    trn_start = TR0_START;
    break;

    case TR1:
    trn_start = TR1_START;
    break;

    case TR2:
    trn_start = TR2_START;
    break;
    case TR3:
    trn_start = TR3_START;
    break;
    case TR4:
    trn_start = TR4_START;
    break;
    case TR5:
    trn_start = TR5_START;
    break;
    case TR6:
    trn_start = TR6_START;
    break;
    case TR7:
    trn_start = TR7_START;
    break;
    case TR8:
    trn_start = TR8_START;
    break;
    case TR9:
    trn_start = TR9_START;
    break;
    case TR10:
    trn_start = TR10_START;
    break;
    case TR11:
    trn_start = TR11_START;
    break;
    }

      switch(LUC_TYPE)
      {
      case  MODE_LUT_SETTING:
          ADR_START = trn_start;
          //ADR_END = trn_start + KWR_MODE_SETTING_LENGTH - 1;

      break;
      case MODE_LUTC:

          ADR_START = trn_start+ KWR_MODE_SETTING_LENGTH;
          //ADR_END = trn_start + KWR_MODE_SETTING_LENGTH + KWR_LUTC_LENGTH - 1;
      break;
      case  MODE_LUTR:
          ADR_START = trn_start + KWR_MODE_SETTING_LENGTH + KWR_LUTC_LENGTH;
         // ADR_END = trn_start + KWR_MODE_SETTING_LENGTH + KWR_LUTC_LENGTH + KWR_LUTR_LENGTH - 1;
      break;
      case MODE_LUTW:
          ADR_START = trn_start + KWR_MODE_SETTING_LENGTH + KWR_LUTC_LENGTH + KWR_LUTR_LENGTH;
          //ADR_END = trn_start + KWR_MODE_SETTING_LENGTH + KWR_LUTC_LENGTH + KWR_LUTR_LENGTH +  KWR_LUTW_LENGTH - 1;
      break;
      case MODE_LUTK:
          ADR_START = trn_start + KWR_MODE_SETTING_LENGTH + KWR_LUTC_LENGTH + KWR_LUTR_LENGTH +  KWR_LUTW_LENGTH ;
         // ADR_END = trn_start + KWR_MODE_SETTING_LENGTH + KWR_LUTC_LENGTH + KWR_LUTR_LENGTH +  KWR_LUTW_LENGTH  +  KWR_LUTK_LENGTH - 1;
      break;

      }
      return ADR_START;
}



int UC8179_KWR_OTP_LUTTYPE_END(u8 LUC_TYPE, u8 TRN)
{
    int ADR_END;

    int trn_start;
    switch(TRN)
    {
    case TR0:
    trn_start = TR0_START;
    break;

    case TR1:
    trn_start = TR1_START;
    break;

    case TR2:
    trn_start = TR2_START;
    break;
    case TR3:
    trn_start = TR3_START;
    break;
    case TR4:
    trn_start = TR4_START;
    break;
    case TR5:
    trn_start = TR5_START;
    break;
    case TR6:
    trn_start = TR6_START;
    break;
    case TR7:
    trn_start = TR7_START;
    break;
    case TR8:
    trn_start = TR8_START;
    break;
    case TR9:
    trn_start = TR9_START;
    break;
    case TR10:
    trn_start = TR10_START;
    break;
    case TR11:
    trn_start = TR11_START;
    break;
    }

      switch(LUC_TYPE)
      {
      case  MODE_LUT_SETTING:
         // ADR_START = trn_start;
          ADR_END = trn_start + KWR_MODE_SETTING_LENGTH - 1;

      break;
      case MODE_LUTC:

          //ADR_START = trn_start+ KWR_MODE_SETTING_LENGTH;
          ADR_END = trn_start + KWR_MODE_SETTING_LENGTH + KWR_LUTC_LENGTH - 1;
      break;
      case  MODE_LUTR:
          //ADR_START = trn_start + KWR_MODE_SETTING_LENGTH + KWR_LUTC_LENGTH;
          ADR_END = trn_start + KWR_MODE_SETTING_LENGTH + KWR_LUTC_LENGTH + KWR_LUTR_LENGTH - 1;
      break;
      case MODE_LUTW:
          //ADR_START = trn_start + KWR_MODE_SETTING_LENGTH + KWR_LUTC_LENGTH + KWR_LUTR_LENGTH;
          ADR_END = trn_start + KWR_MODE_SETTING_LENGTH + KWR_LUTC_LENGTH + KWR_LUTR_LENGTH +  KWR_LUTW_LENGTH - 1;
      break;
      case MODE_LUTK:
          //ADR_START = trn_start + KWR_MODE_SETTING_LENGTH + KWR_LUTC_LENGTH + KWR_LUTR_LENGTH +  KWR_LUTW_LENGTH ;
           ADR_END = trn_start + KWR_MODE_SETTING_LENGTH + KWR_LUTC_LENGTH + KWR_LUTR_LENGTH +  KWR_LUTW_LENGTH  +  KWR_LUTK_LENGTH - 1;
      break;

      }
      return ADR_END;
}

void UC8179_LUT_INI_SETTING(u8 *LUTBD_ARRAY ,u8 *LUTC_ARRAY, u8 *LUTWW_ARRAY, u8 *LUTR_ARRAY, u8 *LUTW_ARRAY, u8 *LUTK_ARRAY)
                             // lutbd_array,    lut_C_array,    lut_WW_array,    lut_KW_array  , lut_WK_array, lut_KK_array
{
    UC8179_KWR_LUTC_SETTING(LUTC_ARRAY);      // VCOM LUT, LUTC
    UC8179_KW_LUTWW_SETTING(LUTWW_ARRAY);     // LUTWW
    UC8179_KWR_LUTR_SETTING(LUTR_ARRAY);      // LUTKW / LUTR
    UC8179_KWR_LUTW_SETTING(LUTW_ARRAY);      // LUTWK / LUTW
    UC8179_KWR_LUTK_SETTING(LUTK_ARRAY);      // LUTKK / LUTK
    UC8179_KWR_LUTBD_SETTING(LUTBD_ARRAY);    // LUTBD

}


void UC8179_MANUAL_INI(void)   // from DKE OTP
{


  if(display_KWR)
  {
    UC8179_PSR_PARAMETER( 0x07 );      // PSR: 0x07 | 0x20 (LUT from Register)
    UC8179_BTST_PARAMETER(0x27, 0x27, 0x28, 0x17);      // BTST  DKE: 0x27, 0x27, 0x28, 0x17



    UC8179_PWR_PARAMETER( 0x07, 0x17, 0x3F, 0x3F, 0x0E);      // PWR  DKE: 0x07, 0x17, 0x3F, 0x3F, 0x0E
    //UC8179_PWR_PARAMETER( 0x07, 0x97, 0x26, 0x26, 0x1B);      // PWR AKM: 0x07, 0x97, 0x26, 0x26, 0x1B

    UC8179_KWOPT_PARAMETER(0x00, 0x00, 0x00);      // KWOPT DKE: 0x00, 0x00, 0x00
    //UC8179_KWOPT_PARAMETER(0x01, 0x00, 0x00);      // KWOPT AKM: 0x00, 0x00, 0x00



    UC8179_CDI_PARAMETER( 0x11, 0x07 );       // CDI DKE: 0x11, 0x07

    UC8179_TCON_PARAMETER( 0x22 );             // TCON  DKE: 0x22

    UC8179_TRES_PARAMETER(0x02, 0x88, 0x01, 0xE0 );      // TRES  DKE: 0x02, 0x88, 0x01, 0xE0

    UC8179_GSST_PARAMETER(0x00, 0x10, 0x00, 0x00 );      // GSST  DKE: 0x00, 0x10, 0x00, 0x00

    UC8179_PWT_PARAMETER(0x00 );       // PWT  DKE: 0x00

    UC8179_LVSEL_PARAMETER( 0x03 );       // LVSEL  DKE: 0x03

    UC8179_TSBDRY_PARAMETER(0x00 );       // TSBDRY   DKE: 0x00

    UC8179_PLL_PARAMETER(0x06 );       // PLL  DKE: 0x06

    UC8179_EVS_PARAMETER( 0x00 );       // EVS   DKE: 0x00

    UC8179_LUTOPT_PARAMETER(0x00, 0x00 );       // LUTOPT  DKE: 0x00, 0x00

    UC8179_VDCS_PARAMETER( 0x4F );       // VDCS  DKE: 0x1E

    UC8179_LUT_INI_SETTING(lutbd_array, lutc_array, lut_WW_array ,lutr_array, lutw_array, lutk_array);  // u8 *LUTBD_ARRAY ,u8 *LUTC_ARRAY, u8 *LUTWW_ARRAY, u8 *LUTR_ARRAY, u8 *LUTW_ARRAY, u8 *LUTK_ARRAY

    //UC8179_LUT_INI_SETTING(lutbd_array, lut_C_array, lut_WW_array, lut_KW_array, lut_WK_array, lut_KK_array);  // KW LUT Data
  }
  else              // KW Mode
  {
          UC8179_PSR_PARAMETER( 0x3F );      // PSR: 0x07 | 0x20 (LUT from Register) | 0x10 (0x10: KW Mode, 0x00: KWR Mode )    )
          UC8179_BTST_PARAMETER(0x27, 0x27, 0x28, 0x17);      // BTST  DKE: 0x27, 0x27, 0x28, 0x17

          UC8179_PWR_PARAMETER( 0x07, 0x17, 0x3F, 0x3F, 0x0E);      // PWR DKE: 0x07, 0x17, 0x3F, 0x3F, 0x0E
          UC8179_KWOPT_PARAMETER(0x00, 0x00, 0x00);      // KWOPT AKM: 0x00, 0x00, 0x00


          UC8179_CDI_PARAMETER( 0x19, 0x07 );       // CDI DKE: 0x11, 0x07



          UC8179_TCON_PARAMETER( 0x22 );             // TCON  DKE: 0x22

          UC8179_TRES_PARAMETER(0x02, 0x80, 0x01, 0x90 );      // TRES  DKE: 0x02, 0x88, 0x01, 0xE0

          UC8179_GSST_PARAMETER(0x00, 0x00, 0x00, 0x00 );      // GSST  DKE: 0x00, 0x10, 0x00, 0x00

          UC8179_PWT_PARAMETER(0x00 );       // PWT  DKE: 0x00

          UC8179_LVSEL_PARAMETER( 0x03 );       // LVSEL  DKE: 0x03

          UC8179_TSBDRY_PARAMETER(0x00 );       // TSBDRY   DKE: 0x00

          UC8179_PLL_PARAMETER(0x06 );       // PLL  DKE: 0x06

          UC8179_EVS_PARAMETER( 0x00 );       // EVS   DKE: 0x00

          UC8179_LUTOPT_PARAMETER(0x00, 0x00 );       // LUTOPT  DKE: 0x00, 0x00
          UC8179_PTYPE(); // for PL P-type Display

          UC8179_VDCS_PARAMETER( 0x1E );       // VDCS  DKE: 0x1E

          UC8179_LUT_INI_SETTING(lutbd_array, lut_C_array, lut_WW_array, lut_KW_array, lut_WK_array, lut_KK_array);  // KW LUT Data



  }





 }

void UC8179_OTP_BANK(u8 bank_nr)
{
    if(bank_nr == BANK1)
    {
        UC8179_spi_write_command(0xA2);
        UC8179_spi_write_parameter(0x00);
        UC8179_spi_write_parameter(0x00);
        UC8179_spi_write_parameter(0x0B);
        UC8179_spi_write_parameter(0xFF);


    }
    else
    {
        UC8179_spi_write_command(0xA2);
        UC8179_spi_write_parameter(0x0C);
        UC8179_spi_write_parameter(0x00);
        UC8179_spi_write_parameter(0x17);
        UC8179_spi_write_parameter(0xFF);

    }




}

void UC8179_WRITE_COMMAND_1PARAM(u8 commad, u8 param1)
{
    UC8179_spi_write_command(commad);
    UC8179_spi_write_parameter(param1);
}


void UC8179_WRITE_COMMAND_2PARAM(u8 commad, u8 param1, u8 param2)
{
    UC8179_spi_write_command(commad);
    UC8179_spi_write_parameter(param1);
    UC8179_spi_write_parameter(param2);
}

void UC8179_WRITE_COMMAND_3PARAM(u8 commad, u8 param1, u8 param2, u8 param3)
{
    UC8179_spi_write_command(commad);
    UC8179_spi_write_parameter(param1);
    UC8179_spi_write_parameter(param2);
    UC8179_spi_write_parameter(param3);
}

void UC8179_WRITE_COMMAND_4PARAM(u8 commad, u8 param1, u8 param2, u8 param3, u8 param4)
{
    UC8179_spi_write_command(commad);
    UC8179_spi_write_parameter(param1);
    UC8179_spi_write_parameter(param2);
    UC8179_spi_write_parameter(param3);
    UC8179_spi_write_parameter(param4);
}

void UC8179_WRITE_COMMAND_5PARAM(u8 commad, u8 param1, u8 param2, u8 param3, u8 param4, u8 param5)
{
    UC8179_spi_write_command(commad);
    UC8179_spi_write_parameter(param1);
    UC8179_spi_write_parameter(param2);
    UC8179_spi_write_parameter(param3);
    UC8179_spi_write_parameter(param4);
    UC8179_spi_write_parameter(param5);
}


void UC8179_WRITE_COMMAND_9PARAM(u8 commad, u8 param1, u8 param2, u8 param3, u8 param4, u8 param5, u8 param6, u8 param7, u8 param8, u8 param9)
{
    UC8179_spi_write_command(commad);
    UC8179_spi_write_parameter(param1);
    UC8179_spi_write_parameter(param2);
    UC8179_spi_write_parameter(param3);
    UC8179_spi_write_parameter(param4);
    UC8179_spi_write_parameter(param5);
    UC8179_spi_write_parameter(param6);
    UC8179_spi_write_parameter(param7);
    UC8179_spi_write_parameter(param8);
    UC8179_spi_write_parameter(param9);
}



void UC8179_PSR_PARAMETER(u8 param1 )
{
    UC8179_WRITE_COMMAND_1PARAM(0x00, param1);      // PSR: 0x07 | 0x20 (LUT from Register)
}


void UC8179_PWR_PARAMETER(u8 param1, u8 param2, u8 param3, u8 param4, u8 param5)
{
  UC8179_WRITE_COMMAND_5PARAM(0x01, param1, param2, param3, param4, param5);      // PWR
}


void UC8179_BTST_PARAMETER( u8 param1, u8 param2, u8 param3, u8 param4)
{
    UC8179_WRITE_COMMAND_4PARAM(0x06, param1, param2, param3, param4);      // BTST
}

void UC8171_BTST_PARAMETER( u8 param1, u8 param2, u8 param3)
{
    UC8179_WRITE_COMMAND_3PARAM(0x06, param1, param2, param3);      // BTST
}

void UC8179_KWOPT_PARAMETER(u8 param1, u8 param2, u8 param3)
{
   UC8179_WRITE_COMMAND_3PARAM(0x2B,  param1, param2, param3 );      // KWOPT
}


void UC8179_CDI_PARAMETER(u8 param1, u8 param2)
{
   UC8179_WRITE_COMMAND_2PARAM(0x50, param1, param2 );       // CDI
}


void UC8179_TCON_PARAMETER(u8 param1)
{
   UC8179_WRITE_COMMAND_1PARAM(0x60, param1 );             // TCON
}

void UC8179_TRES_PARAMETER(u8 param1, u8 param2, u8 param3, u8 param4)
{
   UC8179_WRITE_COMMAND_4PARAM(0x61, param1, param2, param3, param4);      // TRES
}


void UC8171_TRES_PARAMETER(u8 param1, u8 param2, u8 param3)
{
   UC8179_WRITE_COMMAND_3PARAM(0x61, param1, param2, param3);      // TRES
}


void UC8179_GSST_PARAMETER(u8 param1, u8 param2, u8 param3, u8 param4)
{
    UC8179_WRITE_COMMAND_4PARAM(0x65, param1, param2, param3, param4);      // GSST
}


void UC8171_GSST_PARAMETER(u8 param1, u8 param2, u8 param3)
{
    UC8179_WRITE_COMMAND_3PARAM(0x65, param1, param2, param3);      // GSST
}


void UC8179_PWT_PARAMETER(u8 param1)
{
    UC8179_WRITE_COMMAND_1PARAM(0xE3, param1 );       // PWT
}

void UC8179_LVSEL_PARAMETER(u8 param1)
{
    UC8179_WRITE_COMMAND_1PARAM(0xE4, param1 );       // LVSEL
}

void UC8179_GATESETTING_PARAMETER(u8 param1)
{
    UC8179_WRITE_COMMAND_1PARAM(0xE1, param1 );
}



void UC8179_TSBDRY_PARAMETER(u8 param1)
{
    UC8179_WRITE_COMMAND_1PARAM(0xE7, param1 );       // TSBDRY
}


void UC8179_PLL_PARAMETER(u8 param1)
{
    UC8179_WRITE_COMMAND_1PARAM(0x30, param1 );       // PLL
}

void UC8179_EVS_PARAMETER(u8 param1)
{
    UC8179_WRITE_COMMAND_1PARAM(0x52, param1 );       // EVS
}

void UC8179_LUTOPT_PARAMETER(u8 param1, u8 param2)
{
    UC8179_WRITE_COMMAND_2PARAM(0x2A, param1, param2 );       // LUTOPT
}

void UC8179_VDCS_PARAMETER(u8 param1)
{
    UC8179_WRITE_COMMAND_1PARAM(0x82, param1);       // VDCS
}

void UC8179_PTL_PARAMETER(u8 param1, u8 param2,u8 param3,u8 param4,u8 param5,u8 param6,u8 param7, u8 param8, u8 param9)
{
    UC8179_WRITE_COMMAND_9PARAM(0x90, param1, param2, param3, param4, param5, param6, param7, param8, param9);       // VDCS
}


void UC8179_PTIN_PARAMETER()
{
    UC8179_spi_write_command(0x91);
}

void UC8179_PTOUT_PARAMETER()
{
    UC8179_spi_write_command(0x92);
}






void UC8179_OTP_WRITE(u8 *byte_packet, unsigned long packet_length)
{
    UC8179_PROGRAMM_MODE();
    UC8179_byte_array_WRITE1(byte_packet, packet_length);

    UC8179_PWR_PARAMETER( 0x07, 0x97, 0x3F, 0x3F, 0x0E);      // PWR  DKE: 0x07, 0x17, 0x3F, 0x3F, 0x0E.               0x97, for VPP_EN = true;
    UC8179_POWER_ON();
    UC8179_BUSY_N_CHECK();
    UC8179_PROGRAMM_MODE();
    UC8179_BUSY_N_CHECK();
    UC8179_POWER_OFF();



}

bool UC8179_BUSY_N_CHECK()
{
    bool check_finish = false;
    while(!check_finish)
    {
        check_finish = UC8179_GET_STATUS(BUSY_N_FLAG);
        mdelay(50);
    }
    return check_finish;
}

void UC8171_MANUAL_INI(void)   // from DKE OTP
{
    bool display_KWR_UC8171 = false;
  if(display_KWR_UC8171)
  {  UC8179_PSR_PARAMETER( 0x27 );      // PSR: 0x07 | 0x20 (LUT from Register)
    UC8179_BTST_PARAMETER(0x27, 0x27, 0x28, 0x17);      // BTST  DKE: 0x27, 0x27, 0x28, 0x17



    UC8179_PWR_PARAMETER( 0x07, 0x17, 0x3F, 0x3F, 0x0E);      // PWR  DKE: 0x07, 0x17, 0x3F, 0x3F, 0x0E
    //UC8179_PWR_PARAMETER( 0x07, 0x97, 0x26, 0x26, 0x1B);      // PWR AKM: 0x07, 0x97, 0x26, 0x26, 0x1B

    UC8179_KWOPT_PARAMETER(0x00, 0x00, 0x00);      // KWOPT DKE: 0x00, 0x00, 0x00
    //UC8179_KWOPT_PARAMETER(0x01, 0x00, 0x00);      // KWOPT AKM: 0x00, 0x00, 0x00



    UC8179_CDI_PARAMETER( 0x19, 0x07 );       // CDI DKE: 0x11, 0x07

    UC8179_TCON_PARAMETER( 0x22 );             // TCON  DKE: 0x22

    UC8179_TRES_PARAMETER(0x02, 0x88, 0x01, 0xE0 );      // TRES  DKE: 0x02, 0x88, 0x01, 0xE0

    UC8179_GSST_PARAMETER(0x00, 0x10, 0x00, 0x00 );      // GSST  DKE: 0x00, 0x10, 0x00, 0x00

    UC8179_PWT_PARAMETER(0x00 );       // PWT  DKE: 0x00

    UC8179_LVSEL_PARAMETER( 0x03 );       // LVSEL  DKE: 0x03

    UC8179_TSBDRY_PARAMETER(0x00 );       // TSBDRY   DKE: 0x00

    UC8179_PLL_PARAMETER(0x06 );       // PLL  DKE: 0x06

    UC8179_EVS_PARAMETER( 0x00 );       // EVS   DKE: 0x00

    UC8179_LUTOPT_PARAMETER(0x00, 0x00 );       // LUTOPT  DKE: 0x00, 0x00

    UC8179_VDCS_PARAMETER( 0x4F );       // VDCS  DKE: 0x1E

    UC8179_LUT_INI_SETTING(lutbd_array, lutc_array, lut_WW_array ,lutr_array, lutw_array, lutk_array);  // u8 *LUTBD_ARRAY ,u8 *LUTC_ARRAY, u8 *LUTWW_ARRAY, u8 *LUTR_ARRAY, u8 *LUTW_ARRAY, u8 *LUTK_ARRAY

    //UC8179_LUT_INI_SETTING(lutbd_array, lut_C_array, lut_WW_array, lut_KW_array, lut_WK_array, lut_KK_array);  // KW LUT Data
  }
  else              // KW Mode
  {
          UC8179_PSR_PARAMETER( 0x37 );      //
          UC8171_BTST_PARAMETER(0x3f, 0x3f, 0x3f);      // BTST UC8179: 3 Parameters
          UC8179_PWR_PARAMETER( 0x07, 0x07, 0x3F, 0x3F, 0x03);      // PWR AKM: 0x07, 0x97, 0x26, 0x26, 0x1B



      //    UC8179_KWOPT_PARAMETER(0x01, 0x00, 0x00);      // KWOPT AKM: 0x00, 0x00, 0x00

          UC8179_CDI_PARAMETER( 0x31, 0x07 );       // CDI DKE: 0x11, 0x07

          UC8179_TCON_PARAMETER( 0x00 );             // TCON  DKE: 0x22

          UC8171_TRES_PARAMETER(0x68, 0x02, 0x00 );      // TRES  DKE: 0x02, 0x88, 0x01, 0xE0

          UC8171_GSST_PARAMETER(0x00, 0x00, 0x00);      // GSST  DKE: 0x00, 0x10, 0x00, 0x00

          UC8179_PWT_PARAMETER(0x00 );       // PWT  DKE: 0x00

          UC8179_LVSEL_PARAMETER( 0x03 );       // LVSEL  DKE: 0x03

          UC8179_GATESETTING_PARAMETER(0x10 );       // P-Transistor

          UC8179_PLL_PARAMETER(0x04 );       // PLL  DKE: 0x06

          UC8179_EVS_PARAMETER( 0x00 );       // EVS   DKE: 0x00

         // UC8179_LUTOPT_PARAMETER(0x00, 0x00 );       // LUTOPT  DKE: 0x00, 0x00

          UC8179_VDCS_PARAMETER( 0xFF );       // VDCS  DKE: 0x1E

          UC8179_LUT_INI_SETTING(lutbd_array, lut_C_array, lut_WW_array, lut_KW_array, lut_WK_array, lut_KK_array);  // KW LUT Data



  }


 }



void UC8171_image_WHITE(void)
{
    UC8179_spi_write_command(0x10);
   const unsigned long sum =  6656;
    unsigned long i;
    for (i = 0; i < sum; i++ )
    {
        UC8179_spi_write_parameter(0xFF);
    }

}

void UC8171_image_WHITE2(void)
{
    UC8179_spi_write_command(0x13);
  // const unsigned long sum =  6656;  // 104*512/8
    const unsigned long sum =  6240;   // 104*480/8
    unsigned long i;
    for (i = 0; i < sum; i++ )
    {
        UC8179_spi_write_parameter(0xFF);
    }

}

void UC8171_image_BLACK(void)
{
    UC8179_spi_write_command(0x10);
    const unsigned long sum = 6656;
    unsigned long i;
    for (i = 0; i < sum; i++ )
    {
        UC8179_spi_write_parameter(0x00);
    }

}





void UC8171_image_BLACK2(void)
{
    UC8179_spi_write_command(0x13);
    const  unsigned long sum = 6656;
    unsigned long i;
    for (i = 0; i < sum; i++ )
    {
        UC8179_spi_write_parameter(0x00);
    }

}





