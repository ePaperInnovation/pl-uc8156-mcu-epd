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
/*UC8179.c
 *  Created on: 08.04.2021
 *      Author: Zhongke.Dai & Matti.Haugwitz
 */

#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include "msp430/msp430-spi.h"
#include "msp430/msp430-gpio.h"
#include "types.h"
#include "utils.h"
#include <math.h>
#include <UC8179.h>
#include <UC8179_MTP.h>


void UC8179_hardware_reset()
{
	gpio_set_value_lo(PIN_RESET);
	mdelay(1);
	gpio_set_value_hi(PIN_RESET);
}

// waits for BUSY getting inactive = 1 (BUSY pin is low-active)


unsigned int UC8179_wait_for_BUSY_inactive()
{
	unsigned long counter=0;
 	while (gpio_get_value(PIN_BUSY)==0) // BUSY loop
 	{
 		mdelay(1);
		counter++; // BUSY loop
		if (counter>2000) abort_now("Busy-Loop Timeout", ABORT_UC8179_INIT);
 	}
 	return counter;
}

 void UC8179_wait_for_BUSY_inactive_debug()
 {
 	printf("BUSY loop counter = %d\n", UC8179_wait_for_BUSY_inactive());
 }


void UC8179_wait_for_PowerON_ready_debug()
 {
	unsigned long counter = UC8179_wait_for_PowerON_ready();
 	//printf("PowerOn loop counter = %d\n", counter);
 }






u8 UC8179_READ_REVID()
{
    u8 return_value[7];

       UC8179_spi_write_command(0x70);
       return_value[0] = UC8179_spi_read_parameter();
       return_value[1] = UC8179_spi_read_parameter();
       return_value[2] = UC8179_spi_read_parameter();
       return_value[3] = UC8179_spi_read_parameter();
       return_value[4] = UC8179_spi_read_parameter();
       return_value[5] = UC8179_spi_read_parameter();
       return_value[6] = UC8179_spi_read_parameter();

      // uint16_t i = 0;
//       for(i=0; i<7; i++)
//           printf("RevID = %x\n", return_value[i]);

       return return_value[6];
}



u8 UC8171_READ_REVID()
{
    u8 return_value[2];

       UC8179_spi_write_command(0x70);
       return_value[0] = UC8179_spi_read_parameter();
       return_value[1] = UC8179_spi_read_parameter();
       uint16_t i = 0;
       for(i=0; i<2; i++)
           printf("RevID = %x\n", return_value[i]);

       return return_value[1];
}

//u8 *UC8179_READ_PRODUCT_REVISION()
//{
//    u8 value_char[4];
//
//    UC8179_spi_write_command(0x70);
//    value_char[0] =  UC8179_spi_read_parameter();
//    printf("value_char[0] = %d\n", value_char[0]);
//
//    value_char[1] = UC8179_spi_read_parameter();
//    printf("value_char[1] = %d\n", value_char[1]);
//    value_char[2] = UC8179_spi_read_parameter();
//    printf("value_char[2] = %d\n", value_char[2]);
//    value_char[3] = '\0';
//    UC8179_spi_read_parameter();
//    UC8179_spi_read_parameter();
//    UC8179_spi_read_parameter();
//    UC8179_spi_read_parameter();
//
//    return  value_char;
//
//}

//u8 *UC8179_READ_LUT_REVISION()
//{
//    u8 value_char[3];
//    UC8179_spi_write_command(0x70);
//    UC8179_spi_read_parameter();
//    UC8179_spi_read_parameter();
//    UC8179_spi_read_parameter();
//    value_char[0] = UC8179_spi_read_parameter();
//    printf("value_char[0] = %x\n", value_char[0]);
//    value_char[1] = UC8179_spi_read_parameter();
//    printf("value_char[1] = %x\n", value_char[1]);
//    value_char[2] = UC8179_spi_read_parameter();
//    printf("value_char[2] = %x\n", value_char[2]);
//    UC8179_spi_read_parameter();
//    return value_char;
//
//}






void UC8179_PANEL_SETTING(u8 REG_value, u8 KWR_value, u8 UD_value, u8 SHL_value, u8 SHD_N_value, u8 REST_N_value)
{
    u8 value = REG_value | KWR_value | UD_value | SHL_value | SHD_N_value | REST_N_value;
    UC8179_spi_write_command(0x00);
    UC8179_spi_write_parameter(value);
}


void UC8179_PANEL_SETTING_DEFAULT(void)
{
    UC8179_PANEL_SETTING(LUT_FROM_OTP, PIXEL_KWR_MODE, GATE_SCAN_UP, SOURCE_SHIFT_RIGHT, BOOSTER_ON, SOFT_RESET_NO_EFFECT);
}


void UC8179_POWER_SETTING(u8 BD_EN, u8 VSR_EN, u8 VS_EN, u8 VG_EN, u8 VCOM_SLEW, u8 VG_LVL, u8 VDH_LVL, u8 VDL_LVL, u8 VDHR_LVL)
{
    u8 value1 = BD_EN | VSR_EN | VS_EN | VG_EN;
    u8 value2 = VCOM_SLEW | VG_LVL;
    UC8179_spi_write_command(0x01);
    UC8179_spi_write_parameter(value1);
    UC8179_spi_write_parameter(value2);
    UC8179_spi_write_parameter(VDH_LVL);
    UC8179_spi_write_parameter(VDL_LVL);
    UC8179_spi_write_parameter(VDHR_LVL);
}

void UC8179_POWER_SETTING_DEFAULT(void)
{
    UC8179_POWER_SETTING( BORDER_LDO_DISABLE, INTERNAL_DC_FOR_VDHR, INTERNAL_DC_FOR_VDHL, INTERNAL_DC_FOR_VGHL, SLEW_RATE_FAST, VGHL_20V, VDH_LVL, VDL_LVL, VDHR_LVL);
}


void UC8179_POWER_OFF(void)
{
    UC8179_spi_write_command(0x02);
}

void UC8179_POWER_OFF_SEQUENCE_SETTING(u8 T_VDS_OFF)
{
    u8 value = (u8)(T_VDS_OFF << 4 );
    UC8179_spi_write_command(0x03);
    UC8179_spi_write_parameter(value);
}


void UC8179_POWER_ON(void)
{
    UC8179_spi_write_command(0x04);
}

void UC8179_POWER_ON_MEASURE(void)
{
    UC8179_spi_write_command(0x05);
}


void UC8179_BOOSTER_SOFT_START(u8 BT_PHA_PERIODE_VALUE, u8 BT_PHA_STRENGTH_VALUE, u8 BT_PHA_MIN_OFF_VALUE, u8 BT_PHB_PERIODE_VALUE, u8 BT_PHB_STRENGTH_VALUE, u8 BT_PHB_MIN_OFF_VALUE, u8 BT_PHC1_STRENGTH_VALUE, u8 BT_PHC1_MIN_OFF_VALUE, u8 BT_PHC2_ENABLE_VALUE, u8 BT_PHC2_STRENGTH_VALUE, u8 BT_PHC2_MIN_OFF_VALUE)
{
    u8 value_PHA = BT_PHA_PERIODE_VALUE | BT_PHA_STRENGTH_VALUE | BT_PHA_MIN_OFF_VALUE ;
    u8 value_PHB = BT_PHB_PERIODE_VALUE | BT_PHB_STRENGTH_VALUE | BT_PHB_MIN_OFF_VALUE ;
    u8 value_PHC1 = BT_PHC1_STRENGTH_VALUE | BT_PHC1_MIN_OFF_VALUE ;
    u8 value_PHC2  = BT_PHC2_ENABLE_VALUE |BT_PHC2_STRENGTH_VALUE | BT_PHC2_MIN_OFF_VALUE  ;
    UC8179_spi_write_command(0x06);
    UC8179_spi_write_parameter(value_PHA);
    UC8179_spi_write_parameter(value_PHB);
    UC8179_spi_write_parameter(value_PHC1);
    UC8179_spi_write_parameter(value_PHC2);
}


void UC8179_BOOSTER_SOFT_START_DEFAULT(void)
{
    UC8179_BOOSTER_SOFT_START(PH_PERIOD_10MS, PH_STRENGTH3, PH_658US,PH_PERIOD_10MS, PH_STRENGTH3, PH_658US,PH_STRENGTH3, PH_658US,PHC2_ENABLE,PH_STRENGTH3, PH_658US);
}

void UC8179_DEEP_SLEEP(void)
{
    UC8179_spi_write_command(0x07);
    UC8179_spi_write_parameter(0xA5);
}

void UC8179_DATA_TRANSMISSION1(u8 *data, int data_length)
{
    UC8179_spi_write_command(0x10);
    unsigned int i;
    for (i = 0; i < data_length; i++)
    {
        UC8179_spi_write_parameter(data[i]);
    }
}


void UC8179_DATA_TRANSMISSION2(u8 *data, int data_length)
{
    UC8179_spi_write_command(0x13);
    unsigned int i;
    for (i = 0; i < data_length; i++)
    {
        UC8179_spi_write_parameter(data[i]);
    }
}

u8 UC8179_DATA_FLAG()
{
    u8 data_flag;
    UC8179_spi_write_command(0x11);
    data_flag = (UC8179_spi_read_parameter() >> 7) & 0x01;

    return data_flag;
}

void UC8179_DISPLAY_REFRESH(void)
{
    UC8179_spi_write_command(0x12);
}


void UC8179_DUAL_SPI_MODE(u8 MM_EN, u8 DUSPI_EN)
{
    u8 value = MM_EN | DUSPI_EN;
    UC8179_spi_write_command(0x15);
    UC8179_spi_write_parameter(value);
}

void UC8179_AUTO_SEQUENCE(u8 auto_code)
{
    UC8179_spi_write_command(0x17);
    UC8179_spi_write_parameter(auto_code);
}

void UC8179_LUT_OPTION(u8 STATE10_XON, u8 STATE9_XON, u8 STATE8_XON, u8 STATE7_XON, u8 STATE6_XON, u8 STATE5_XON, u8 STATE4_XON, u8 STATE3_XON, u8 STATE2_XON, u8 STATE1_XON)
{
    u8 value1 = STATE10_XON | STATE9_XON;
    u8 value2 = STATE8_XON | STATE7_XON | STATE6_XON | STATE5_XON | STATE4_XON | STATE3_XON | STATE2_XON | STATE1_XON;
    UC8179_spi_write_command(0x2A);
    UC8179_spi_write_parameter(value1);
    UC8179_spi_write_parameter(value2);
}


void UC8179_KW_LUT_OPTION( u8 LUT_SELECT    , u8 STATE10_XON, u8 STATE9_XON, u8 STATE8_XON, u8 STATE7_XON, u8 STATE6_XON, u8 STATE5_XON, u8 STATE4_XON, u8 STATE3_XON, u8 STATE2_XON, u8 STATE1_XON)
{
    u8 value1 = STATE10_XON | STATE9_XON;
    u8 value2 = STATE8_XON | STATE7_XON | STATE6_XON | STATE5_XON | STATE4_XON | STATE3_XON | STATE2_XON | STATE1_XON;
    UC8179_spi_write_command(0x2B);
    UC8179_spi_write_parameter(LUT_SELECT);
    UC8179_spi_write_parameter(value1);
    UC8179_spi_write_parameter(value2);
}


void UC8179_PLL_CONTROL(u8 FRAME_RATE_SETTING)
{
    UC8179_spi_write_command(0x30);
    UC8179_spi_write_parameter(FRAME_RATE_SETTING); // 0b0110: 50Hz
}

u8 UC8179_TEMPERATUR_READ_INTER()
{
    u8 value;
    UC8179_spi_write_command(0x40);
    value = UC8179_spi_read_parameter();
    return value;
}

void UC8179_TEMPERATURE_SENSOR_ENABLE()
{
    UC8179_spi_write_command(0x41);
    UC8179_spi_write_parameter(0x00); // Default: using internal sensor, offset: 0
}

u8 UC8179_PANEL_GLASS_CHECK()
{
    u8 PSTA;
    UC8179_spi_write_command(0x44);
    mdelay(1);
    PSTA = UC8179_spi_read_parameter();
    return PSTA;
}

void UC8179_VCOM_AND_DATA_INTERVAL_SETTING(u8 BDZ, u8 BDV, u8 N2OCP, u8 DDX, u8 CDI )
{
    u8 value = BDZ | BDV | N2OCP | DDX;
    UC8179_spi_write_command(0x50);
    UC8179_spi_write_parameter(value);
    UC8179_spi_write_parameter(CDI);
}

void UC8179_VCOM_AND_DATA_SETTING_KWR_KW(u8 BDV, u8 DDX)
{
    UC8179_VCOM_AND_DATA_INTERVAL_SETTING(BDZ_OUTPUT_HIZ_DISABLE, BDV, COPY_NEW_DATA_TO_OLD_DATA_DIS, DDX, VCOM_AND_DATA_INTERVAL);
}



bool UC8179_LOW_POWER_DETECTION()
{
    u8 LPD_VALUE;
    bool LPD_FLAG;
    UC8179_spi_write_command(0x51);
    gpio_set_value_lo(PIN_BUSY);
    mdelay(1);
    gpio_set_value_hi(PIN_BUSY);
    LPD_VALUE = UC8179_spi_read_parameter();
    if(LPD_VALUE == 1)
    {
     LPD_FLAG = true;
    }
    else
    {
        LPD_FLAG = false;
    }
    return LPD_FLAG;
}





void UC8179_END_VOLTAGE_SETTING(u8 VCEND, u8 BDEND)
{
    u8 value = VCEND | BDEND;
    UC8179_spi_write_command(0x52);
    UC8179_spi_write_parameter(value);
}











void UC8179_TCON_SETTING(u8 S2G, u8 G2S)
{
    u8 value = S2G | G2S;
    UC8179_spi_write_command(0x60);
    UC8179_spi_write_parameter(value);
}




void UC8179_RESOLUTION_SETTING(int HORIZON_RESOLUTION, int VERTICAL_RESOLUTION)
{
    u8 value_hori_h = (u8)((HORIZON_RESOLUTION>>8)  & 0x03);
    u8 value_hori_l = (u8)(HORIZON_RESOLUTION  & 0xF8);
    u8 value_vert_h = (u8)((VERTICAL_RESOLUTION>>8)  & 0x03);
    u8 value_vert_l = (u8)(VERTICAL_RESOLUTION  & 0xFF);
    UC8179_spi_write_command(0x61);
    UC8179_spi_write_parameter(value_hori_h);
    UC8179_spi_write_parameter(value_hori_l);
    UC8179_spi_write_parameter(value_vert_h);
    UC8179_spi_write_parameter(value_vert_l);
}

void UC8179_GATE_SOURCE_START_SETTING(int SOURCE_START, int GATE_START)
{
    u8 value_source_h = (u8)((SOURCE_START>>8)  & 0x03);
    u8 value_source_l = (u8)(SOURCE_START  & 0xF8);
    u8 value_gate_h = (u8)((GATE_START>>8)  & 0x03);
    u8 value_gate_l = (u8)(GATE_START  & 0xFF);
    UC8179_spi_write_command(0x65);
    UC8179_spi_write_parameter(value_source_h);
    UC8179_spi_write_parameter(value_source_l);
    UC8179_spi_write_parameter(value_gate_h);
    UC8179_spi_write_parameter(value_gate_l);
}



bool UC8179_GET_STATUS(u8 STATUS_FLAG)
{
    bool status_flag = false;
    UC8179_spi_write_command(0x71);
    u8 status_flag_u8 = UC8179_spi_read_parameter();
    u8 status_flag_check = 0;
    switch(STATUS_FLAG)
    {
        case BUSY_N_FLAG:
            status_flag_check = status_flag_u8 & 0x01;
            break;
        case POF_FLAG:
            status_flag_check = status_flag_u8 & 0x02;
            break;
        case PON_FLAG:
            status_flag_check = status_flag_u8 & 0x04;
            break;
        case DATA_FLAG:
            status_flag_check = status_flag_u8 & 0x08;
            break;
        case I2C_BUSY_N:
            status_flag_check = status_flag_u8 & 0x10;
            break;
        case I2C_ERR:
            status_flag_check = status_flag_u8 & 0x20;
            break;

        case PTL_FLAG:
            status_flag_check = status_flag_u8 & 0x40;
            break;
    }
    if(status_flag_check > 0)
    {
        status_flag = true;
    }
    return status_flag;
}


void UC8179_AUTO_MEASURE_VCOM(u8 AMVT, u8 XON, u8 AMVS)
{
    u8 value = AMVT | XON | AMVS;
    UC8179_spi_write_command(0x80);
    UC8179_spi_write_parameter(value);
}

u8 UC8179_VCOM_VALUE_READ()
{
    u8 value_read;
    UC8179_spi_write_command(0x81);
    value_read = UC8179_spi_read_parameter();
    return value_read;
}

void UC8179_VCOM_DC_SETTING(int VCOM_DC)
{
    u8 value;
    value = ((-VCOM_DC) -10)/5;
    printf("vcom_DC_value = %d\n", value);
    UC8179_spi_write_command(0x82);
    UC8179_spi_write_parameter(value);

}


void UC8179_PARTIAL_WINDOW(int HRST, int HRED, int VRST, int VRED, u8 PT_SCAN)
{
    u8 hrst_h = (u8)((HRST>>8)  & 0x03);
    u8 hrst_l = (u8)(HRST& 0xF8);

    u8 hred_h = (u8)((HRED>>8)  & 0x03);
    u8 hred_l = (u8)(HRST& 0xF8);

    u8 vrst_h = (u8)((VRST>>8)  & 0x03);
    u8 vrst_l = (u8)(VRST  & 0xFF);

    u8 vred_h = (u8)((VRED>>8)  & 0x03);
    u8 vred_l = (u8)(VRED  & 0xFF);


    UC8179_spi_write_command(0x90);

    UC8179_spi_write_command(hrst_h);
    UC8179_spi_write_command(hrst_l);
    UC8179_spi_write_command(hred_h);
    UC8179_spi_write_command(hred_l);
    UC8179_spi_write_command(vrst_h);
    UC8179_spi_write_command(vrst_l);
    UC8179_spi_write_command(vred_h);
    UC8179_spi_write_command(vred_l);
    UC8179_spi_write_command(PT_SCAN);
}




void UC8179_PARTIAL_IN()
{
    UC8179_spi_write_command(0x91);
}

void UC8179_PARTIAL_OUT()
{
    UC8179_spi_write_command(0x92);
}


void UC8179_PROGRAMM_MODE()
{
    UC8179_spi_write_command(0xA0);
}

void UC8179_ACTIVE_PROGRAMM()
{
    UC8179_spi_write_command(0xA1);
}


void UC8179_NTYPE()
{
    UC8179_spi_write_command(0xE1);
    UC8179_spi_write_parameter(0x00);
}


void UC8179_PTYPE()
{
    UC8179_spi_write_command(0xE1);
    UC8179_spi_write_parameter(0x10);
}


void UC8179_POWER_SAVING()
{
    UC8179_spi_write_command(0xE3);
    UC8179_spi_write_parameter(0x00);
}


void UC8179_LVD_VOLTAGE_SELECT(u8 LVD_SEL)
{
    UC8179_spi_write_command(0xE4);
    UC8179_spi_write_parameter(LVD_SEL);
}

void UC8179_TEMPERATUR_BOUNDRY_C2()
{
    UC8179_spi_write_command(0xE7);
    UC8179_spi_write_parameter(0x00);
}


void UC8179_LUT_SETTING(u8 LUT_TYPE, u8 LELVEL_SELECT, u8 FRAME0, u8 FRAME1, u8 FRAME2, u8 FRAME3, u8 TIME_REPEAT )
{
    UC8179_spi_write_command(LUT_TYPE);
    UC8179_spi_write_parameter(LELVEL_SELECT);
    UC8179_spi_write_parameter(FRAME0);
    UC8179_spi_write_parameter(FRAME1);
    UC8179_spi_write_parameter(FRAME2);
    UC8179_spi_write_parameter(FRAME3);
    UC8179_spi_write_parameter(TIME_REPEAT);
}

u8 UC8179_OTP_READ(int ADDRESS)
{
    u8 value;
    UC8179_spi_write_command(0xA2);
    int i;

    for(i = 0; i<=ADDRESS; i++)
    {
        value = UC8179_spi_read_parameter();
    }
    //value = UC8179_spi_read_parameter();
    return value;
}

void UC8179_KWR_LUTC_SETTING(u8 *LUTC_ARRAY)
{
    UC8179_spi_write_command(0x20);
    uint16_t i;
    for(i = 0; i < KWR_LUTC_LENGTH; i++)
    {
        UC8179_spi_write_parameter(LUTC_ARRAY[i]);
    }
}


void UC8179_KW_LUTWW_SETTING(u8 *LUTC_ARRAY)
{
    UC8179_spi_write_command(0x21);
    uint16_t i;
    for(i = 0; i < KWR_LUTC_LENGTH; i++)
    {
        UC8179_spi_write_parameter(LUTC_ARRAY[i]);
    }
}



void UC8179_KWR_LUTR_SETTING(u8 *LUTR_ARRAY)
{
    UC8179_spi_write_command(0x22);
    uint16_t i;
    for(i = 0; i < KWR_LUTR_LENGTH; i++)
    {
        UC8179_spi_write_parameter(LUTR_ARRAY[i]);
    }
}

void UC8179_KWR_LUTW_SETTING(u8 *LUTW_ARRAY)
{
    UC8179_spi_write_command(0x23);
    uint16_t i;
    for(i = 0; i < KWR_LUTW_LENGTH; i++)
    {
        UC8179_spi_write_parameter(LUTW_ARRAY[i]);
    }
}

void UC8179_KWR_LUTK_SETTING(u8 *LUTK_ARRAY)
{
    UC8179_spi_write_command(0x24);
    uint16_t i;
    for(i = 0; i < KWR_LUTK_LENGTH; i++)
    {
        UC8179_spi_write_parameter(LUTK_ARRAY[i]);
    }
}


void UC8179_KWR_LUTBD_SETTING(u8 *LUTBD_ARRAY)
{
    UC8179_spi_write_command(0x25);
    uint16_t i;
    for(i = 0; i < KWR_LUTBD_LENGTH; i++)
    {
        UC8179_spi_write_parameter(LUTBD_ARRAY[i]);
    }
}



