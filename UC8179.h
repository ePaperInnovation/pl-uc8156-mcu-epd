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
 * UC8179.h
 *
 *  Created on: 08.04.2021
 *      Author: Zhongke.Dai & Matti.Haugwitz
 */

#ifndef UC8179_H_
#define UC8179_H_

#include "types.h"
#include <stddef.h>



////////////////////////////////////// Parameter for Panel Setting
enum UC8179_LUT_SETTING {LUT_FROM_OTP=0x00, LUT_FROM_REGISTER=0x20}; // panel setting bit 5 0: LUT from OTP (Default); 1: LUT from register
enum UC8179_KWR{ PIXEL_KWR_MODE = 0x00, PIXEL_KW_MODE = 0x10}; // 0: Pixel with Black/White/Red (Default); 1: Pixel with Black/White
enum UC8179_GATE_SCAN_DIRECTION{GATE_SCAN_DOWN = 0x00,  GATE_SCAN_UP = 0x08}; //  0: First line to Last line: Gn-1 ->  Gn-2 ->... ->G0; 1: G0 -> G1...->Gn
enum UC8179_SOURCE_SHIFT_DIRECTION{SOURCE_SHIFT_LEFT = 0x00,  SOURCE_SHIFT_RIGHT = 0x04}; // First data to Last data 0 (shift left): Sn-1 -> Sn-2 ->...-> S0; 1 (shift right): S0 -> S1 ->...-> Sn-1
enum UC8179_BOOSTER_SWITCH{ BOOSTER_OFF = 0x00,  BOOSTER_ON = 0x02};  // 0: Booster OFF; 1: Booster ON (Default)
enum UC8179_SOFT_RESET{SOFT_RESET = 0x00, SOFT_RESET_NO_EFFECT = 0x01 }; // 0: Soft Reset; 1: No effect
////////////////////////////////////////////////


/////////////////////////////////////Parameter for Power Setting//////////
enum UC8179_BORDER_LDO{BORDER_LDO_DISABLE = 0x00, BORDER_LDO_ENABLE = 0x10}; // 0: Border LDO disable (Default)
enum UC8179_SOURCE_LV_POWER_SELECTION{ EXTERNAL_POWER_FOR_VDHR = 0x00, INTERNAL_DC_FOR_VDHR = 0x04};
enum UC8179_SOURCE_POWER_SELECTION{ EXTERNAL_POWER_FOR_VDHL = 0x00, INTERNAL_DC_FOR_VDHL = 0x02};
enum UC8179_GATE_POWER_SELECTION{ EXTERNAL_POWER_FOR_VGHL = 0x00, INTERNAL_DC_FOR_VGHL = 0x01};
enum UC8179_VCOM_SLEW{SLEW_RATE_SLOW = 0x00, SLEW_RATE_FAST = 0x10};
enum UC8179_VG_LVL{VGHL_9V = 0x00, VGHL_10V = 0x01, VGHL_11V = 0x02, VGHL_12V = 0x03, VGHL_17V = 0x04, VGHL_18V = 0x05, VGHL_19V = 0x06, VGHL_20V = 0x07  };
enum UC8179_VDH_VDL_VDHR {VDH_LVL = 0x58, VDL_LVL = 0x58, VDHR_LVL = 0x03};


///////////////////////////////////// Parameter for Boost Soft Start
enum UC8179_BT_PH_PERIOD {PH_PERIOD_10MS = 0x00, PH_PERIOD_20MS = 0x40, PH_PERIOD_30MS = 0x80 , PH_PERIOD_40MS = 0xC0};
enum UC8179_BT_PH_DRIVING_STRENGTH { PH_STRENGTH1 = 0x00, PH_STRENGTH2 = 0x08, PH_STRENGTH3 = 0x10, PH_STRENGTH4 = 0x18, PH_STRENGTH5 = 0x20, PH_STRENGTH6 = 0x00, PH_STRENGTH7 = 0x30, PHA_STRENGTH8 = 0x38};
enum UC8179_BT_PH_MIN_OFF_TIME{ PH_027US = 0x00,  PH_034US = 0x01, PH_040US = 0x02, PH_054US = 0x3, PH_080US = 0x04, PH_154US = 0x05, PH_334US = 0x06, PH_658US = 0x07};
enum UC8179_BT_PHC2{ PHC2_ENABLE = 0x80, PHC2_DISABLE = 0x00  };



enum UC8179_DUAL_SPI{MM_INPUT_DISABLE = 0x00, MM_INPUT_ENABLE = 0x20, DUAL_SPI_MODE_DISABLE = 0x00, DUAL_SPI_MODE_ENABLE = 0x10};
enum UC8179_AUTO_CODE{PON_DRF_POF = 0xA5, PON_DRF_POF_DSLP = 0xA7};

enum UC8179_STATE_XON_ENABLE{STATE_XON_DISABLE = 0x00, STATE1_ENABLE = 0x01, STATE2_ENABLE = 0x02, STATE3_ENABLE = 0x04, STATE4_ENABLE = 0x08, STATE5_ENABLE = 0x10, STATE6_ENABLE = 0x20, STATE7_ENABLE = 0x40, STATE8_ENABLE = 0x80, STATE9_ENABLE = 0x40, STATE10_ENABLE = 0x80 };

enum UC8179_KWR_LUT_SELECT{KWR_LUT_ALWAYS = 0x00, KW_LUT_ONLY = 0x01, RED_AUTO_DETECT = 0x02};
enum UC8179_VCOM_END_VOLTAGE_SELECTION{VCOM_END_VOLTAGE_VCOM = 0x00, VCOM_END_VOLTAGE_FLOATING = 0x08};
enum UC8179_BORDER_END_VOLTAGE_SELECTION{BORDER_END_VOLTAGE_0 = 0x00, BORDER_END_VOLTAGE_VCOM = 0x02, BORDER_END_VOLTAGE_FLOATING = 0x03 };

enum UC8179_GET_STATUS_FLAG{BUSY_N_FLAG = 0x00, POF_FLAG = 0x01, PON_FLAG = 0x02, DATA_FLAG = 0x03, I2C_BUSY_N = 0x04, I2C_ERR = 0x05, PTL_FLAG = 0x06};

enum UC8179_AUTO_MEASURE_VCOM_TIME_SETTING{AMVT_3S = 0x00, AMVT_5S = 0x10, AMVT_8S = 0x20, AMVT_10S = 0x30 };
enum UC8179_AUTO_MEASURE_VCOM_XON_SETTING{GATE_NORMAL = 0x00, GATE_ALL_ON = 0x08};
enum UC8179_AUTO_MEASURE_VCOM_SOURCE_OUTPUT_SETTING{SOURCE_OUTPUT_0 = 0x00,  SOURCE_OUTPUT_VDHR = 0x04};

enum UC8179_LVD_VOLTAGE_SELECT_SETTING{LVD_SEL_22V = 0x00, LVD_23V = 0x01, LVD_24V = 0x02, LVD_25V = 0x03 };

enum UC8179_PLL_CONTROL_FRAME_RATE{PLL_CONTROL_FRAME_RATE_50HZ = 0x06};

enum UC8179_VCOM_DATA_INTER_SETTING_BORDER_OUTPUT_HIZ{BDZ_OUTPUT_HIZ_DISABLE = 0x80, BDZ_OUTPUT_HIZ_ENABLE = 0x80 };
enum UC8179_VCOM_DATA_INTER_SETTING_BORDER_LUT_SELECTION_KWR{BORDER_LUT_LUTK = 0x00, BORDER_LUT_LUTW = 0x10, BORDER_LUT_LUTR = 0x20};
enum UC8179_VCOM_DATA_INTER_SETTING_BORDER_LUT_SELECTION_KW{BORDER_LUT_LUTKK = 0x00, BORDER_LUT_LUTWK = 0x10, BORDER_LUT_LUTKW = 0x20, BORDER_LUT_LUTWW = 0x30 };
enum UC8179_VCOM_DATA_INTER_SETTING_N2OCP{COPY_NEW_DATA_TO_OLD_DATA_DIS = 0x00, COPY_NEW_DATA_TO_OLD_DATA_EN = 0x08};
enum UC8179_VCOM_DATA_INTER_INTERVAL{VCOM_AND_DATA_INTERVAL = 0x07};
enum UC8179_PARTIAL_WINDOW_PT_SCAN{GATE_ONLY_INSIDE = 0x00, GATE_BOTH_INSIDE_AND_OUTSIDE = 0x01};
enum UC8179_LUT_TYPE_SETTING{LUTC = 0x20, LUTWW = 0x21, LUTKW_LUTR = 0x22, LUTBD = 0x25 };


void UC8179_hardware_reset();
unsigned int UC8179_wait_for_BUSY_inactive();
unsigned long UC8179_wait_for_PowerON_ready();


u8 UC8179_READ_REVID();
//u8 *UC8179_READ_PRODUCT_REVISION();
//u8 *UC8179_READ_LUT_REVISION();
void UC8179_PANEL_SETTING(u8 REG_value, u8 KWR_value, u8 UD_value, u8 SHL_value, u8 SHD_N_value, u8 REST_N_value);
void UC8179_PANEL_SETTING_DEFAULT(void);
void UC8179_POWER_SETTING(u8 BD_EN, u8 VSR_EN, u8 VS_EN, u8 VG_EN, u8 VCOM_SLEW, u8 VG_LVL, u8 VDH_LVL, u8 VDL_LVL, u8 VDHR_LVL);
void UC8179_POWER_SETTING_DEFAULT(void);
void UC8179_POWER_OFF(void);
void UC8179_POWER_OFF_SEQUENCE_SETTING(u8 T_VDS_OFF);
void UC8179_POWER_ON(void);
void UC8179_POWER_ON_MEASURE(void);
void UC8179_BOOSTER_SOFT_START(u8 BT_PHA_PERIODE_VALUE, u8 BT_PHA_STRENGTH_VALUE, u8 BT_PHA_MIN_OFF_VALUE, u8 BT_PHB_PERIODE_VALUE, u8 BT_PHB_STRENGTH_VALUE, u8 BT_PHB_MIN_OFF_VALUE, u8 BT_PHC1_STRENGTH_VALUE, u8 BT_PHC1_MIN_OFF_VALUE, u8 BT_PHC2_ENABLE_VALUE, u8 BT_PHC2_STRENGTH_VALUE, u8 BT_PHC2_MIN_OFF_VALUE);
void UC8179_BOOSTER_SOFT_START_DEFAULT(void);
void UC8179_DEEP_SLEEP(void);
void UC8179_DATA_TRANSMISSION1(u8 *data, int data_length);
void UC8179_DATA_TRANSMISSION2(u8 *data, int data_length);
u8 UC8179_DATA_FLAG();
void UC8179_DISPLAY_REFRESH(void);
void UC8179_DUAL_SPI_MODE(u8 MM_EN, u8 DUSPI_EN);
void UC8179_AUTO_SEQUENCE(u8 auto_code);
void UC8179_LUT_OPTION(u8 STATE10_XON, u8 STATE9_XON, u8 STATE8_XON, u8 STATE7_XON, u8 STATE6_XON, u8 STATE5_XON, u8 STATE4_XON, u8 STATE3_XON, u8 STATE2_XON, u8 STATE1_XON);
void UC8179_KW_LUT_OPTION(u8 LUT_SELECT, u8 STATE10_XON, u8 STATE9_XON, u8 STATE8_XON, u8 STATE7_XON, u8 STATE6_XON, u8 STATE5_XON, u8 STATE4_XON, u8 STATE3_XON, u8 STATE2_XON, u8 STATE1_XON);
void UC8179_PLL_CONTROL(u8 FRAME_RATE_SETTING);
void UC8179_TEMPERATURE_SENSOR_ENABLE();
bool UC8179_LOW_POWER_DETECTION();
void UC8179_END_VOLTAGE_SETTING(u8 VCEND, u8 BDEND);
void UC8179_TCON_SETTING(u8 S2G, u8 G2S);
u8 UC8179_PANEL_GLASS_CHECK();
void UC8179_RESOLUTION_SETTING(int HORIZON_RESOLUTION, int VERTICAL_RESOLUTION);
void UC8179_GATE_SOURCE_START_SETTING(int SOURCE_START, int GATE_START);
bool UC8179_GET_STATUS(u8 STATUS_FLAG);
void UC8179_AUTO_MEASURE_VCOM(u8 value);
u8 UC8179_VCOM_VALUE_READ();
void UC8179_VCOM_DC_SETTING(int VCOM_DC);
void UC8179_PARTIAL_IN();
void UC8179_PARTIAL_OUT();
void UC8179_PROGRAMM_MODE();
void UC8179_ACTIVE_PROGRAMM();
void UC8179_LVD_VOLTAGE_SELECT(u8 LVD_SEL);
void UC8179_VCOM_AND_DATA_INTERVAL_SETTING(u8 BDZ, u8 BDV, u8 N2OCP, u8 DDX, u8 CDI);
void UC8179_VCOM_AND_DATA_SETTING_KWR_KW(u8 BDV, u8 DDX);
void UC8179_PARTIAL_WINDOW(int HRST, int HRND, int VRST, int VRED, u8 PT_SCAN);
void UC8179_POWER_SAVING();
void UC8179_PTYPE();
void UC8179_NTYPE();
void UC8179_TEMPERATUR_BOUNDRY_C2();
void UC8179_LUT_SETTING(u8 LUT_TYPE, u8 LELVEL_SELECT, u8 FRAME0, u8 FRAME1, u8 FRAME2, u8 FRAME3, u8 TIME_REPEAT );
u8 UC8179_OTP_READ(int ADDRESS);
u8 UC8179_TEMPERATUR_READ_INTER();

void UC8179_KWR_LUTC_SETTING(u8 *LUTC_ARRAY);
void UC8179_KW_LUTWW_SETTING(u8 *LUTC_ARRAY);
void UC8179_KWR_LUTR_SETTING(u8 *LUTR_ARRAY);
void UC8179_KWR_LUTW_SETTING(u8 *LUTW_ARRAY);
void UC8179_KWR_LUTK_SETTING(u8 *LUTK_ARRAY);
void UC8179_KWR_LUTBD_SETTING(u8 *LUTBD_ARRAY);



// UC8171 Test
u8 UC8171_READ_REVID();

#endif /* SOLOMON_H_ */
