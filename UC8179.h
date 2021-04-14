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

#ifndef SOLOMON_H_
#define SOLOMON_H_

#include "types.h"
#include <stddef.h>

#define GATE_LINES_MAX 160
#define SOURCE_LINES_MAX 240

enum UPDATE_MODES {FULL_UPDATE=0x00, PARTIAL_UPDATE=0x04, INIT_UPDATE=0x10}; // Reg[14h]
enum WAVEFORM_TYPES {NORMAL_4GL=0x00, FAST_2GL=0x02}; // Reg[40h]
enum WAVEFORM_MODES {WAVEFORM_FROM_LUT=0x00, WAVEFORM_FROM_MTP=0x02}; // Reg[14h]
enum TRANSPARENCY_KEY_VALUE { TRANSPARENCY_GS0 = 0x00,  TRANSPARENCY_GS1 = 0x40, TRANSPARENCY_GS2 = 0x80,TRANSPARENCY_GS3 = 0xc0 }; // Reg[14h]
enum TRANSPARENCY_DISPLAY_ENABLE {  TRANSPARENCY_DISPLAY_ENABLE_OFF = 0x00,  TRANSPARENCY_DISPLAY_ENABLE_ON = 0x08 };  // Reg[14h]
enum DISPLAY_MODE_SELECT {FULL_DISPLAY_UPDATE = 0x00, INITIAL_UPDATE = 0x10, AREA_DISPLAY_UPDATE = 0x20, AREA_DISPLAY_UPDATE_DISABLE_NON_SELECT_GATE_LINE = 0x30};
enum DATA_STOP_FLAG {NOT_ALL_DATA=0x00, ONEFRAME_DATA=0x80}; // Reg[14h]

#define WAVEFORM_LENGTH 120
#define TS_LENGTH 10


void UC8179_panel_Setting(u8 reg_value);
void UC8179_power_Setting(u8 reg_value1, u8 reg_value2, u8 reg_value3,  u8 reg_value4,  u8 reg_value5);
void UC8179_poweroff_sequenceSetting(u8 reg_value);
void UC8179_boost_Softstart(u8 reg_value1, u8 reg_value2, u8 reg_value3,  u8 reg_value4);
void UC8179_data_Transmission1(u8 *image_data, size_t size);
void UC8179_display_Refresh();
void UC8179_data_Transmission1(u8 *image_data, size_t size);
u8 UC8179_read_PanelCheck();

void UC8179_reset();
void UC8179_hardware_reset();
unsigned int UC8179_wait_for_BUSY_inactive();
unsigned long UC8179_wait_for_PowerON_ready();
unsigned long UC8179_wait_for_PowerON_ready_timeout();
void UC8179_init_registers();
void UC8179_HVs_on();
void UC8179_HVs_off();

u8 UC8179_read_RevID();
u8 print_current_VCOM();

void UC8179_send_waveform(u8 *waveform);
void UC8179_set_Vcom(int VCOM_mv_value);
void UC8179_send_image_data(u8 *image_data);
void UC8179_send_image_data_inv(u8 *image_data);
void UC8179_send_image_data_area(u8 *image_data, int col_start, int col_size, int row_start, int row_size);
void UC8179_send_repeated_image_data(u8 image_data);
void UC8179_update_display(u8 update_mode, u8 waveform_mode);
void UC8179_update_display_full();
void UC8179_update_display_with_power_on_off(u8 update_mode, u8 waveform_mode);
void UC8179_show_image(u8 *image_data, u8 update_mode, u8 waveform_mode);
void UC8179_show_image_inv(u8 *image_data, u8 update_mode, u8 waveform_mode);

void UC8179_show_image_area(u8 *image_data, int col_start, int col_size, int row_start, int row_size, u8 update_mode, u8 waveform_mode);
void UC8179_check_status_register(u8 expected_value);
void UC8179_check_RevID();

void UC8179_send_data_to_image_RAM_for_MTP_program(u8 *waveform_data, size_t size);

// Function prototypes for dual controller displays - second display
unsigned int UC8179_wait_for_BUSY_inactive_slave();
void UC8179_init_registers_slave();
void UC8179_HVs_on_dual();
void UC8179_HVs_off_dual();
void UC8179_send_waveform_slave(u8 *waveform);
void UC8179_print_waveform();
void UC8179_print_waveform_slave();
void UC8179_set_Vcom_slave(int Vcom_mv_value);
void UC8179_send_image_data_slave(u8 *image_data);
void UC8179_send_repeated_image_data_slave(u8 image_data);
void UC8179_update_display_dual(u8 update_mode, u8 waveform_mode);
void UC8179_show_image_dual(u8 *image_data, u8 update_mode, u8 waveform_mode);
void UC8179_check_RevID_slave();

float UC8179_measure_VCOM();
void print_measured_VCOM();
void UC8179_measure_Vcom_curve();
void tcom_timing_setting(u8 vg_lv, u8 vs_lv);
void drive_voltage_setting(u8 gap, u8 s2g);


void UC8179_send_image_data_GL0(u8 *image_data);
void UC8179_send_image_data_GL4(u8 *image_data);
void UC8179_send_image_data_GL11(u8 *image_data);
void UC8179_send_image_data_GL15(u8 *image_data);

void UC8179_show_image_GL(u8 *image_data, u8 update_mode, u8 waveform_mode, int GL_name);
void UC8179_update_display_all_set(u8 update_mode, u8 waveform_mode, u8 transparency_key_value, u8 transparency_display_enable, u8 display_mode_select);
void UC8179_show_image_all_set(u8 *image_data, u8 update_mode, u8 waveform_mode, u8 transparency_key_value, u8 transparency_display_enable, u8 display_mode_select, bool inv_enable);
void UC8179_set_Vcom_Acep(int Vcom_mv_value);
void drive_voltage_setting_Acep(int vs_lv);
u8 UC8179_read_PanelCheck();
#endif /* SOLOMON_H_ */
