/*
 * solomon.h
 *
 *  Created on: 15.01.2014
 *      Author: andreas.meier
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

#define WAVEFORM_LENGTH 120
#define TS_LENGTH 10

void UC8156_reset();
void UC8156_hardware_reset();
unsigned int UC8156_wait_for_BUSY_inactive();
void UC8156_init_registers();
void UC8156_HVs_on();
void UC8156_HVs_off();

u8 UC8156_read_RevID();
void print_current_VCOM();

void UC8156_send_waveform(u8 *waveform);
void UC8156_set_Vcom(int VCOM_mv_value);
void UC8156_send_image_data(u8 *image_data);
void UC8156_send_image_data_area(u8 *image_data, int col_start, int col_size, int row_start, int row_size);
void UC8156_send_repeated_image_data(u8 image_data);
void UC8156_update_display(u8 update_mode, u8 waveform_mode);
void UC8156_update_display_with_power_on_off(u8 update_mode, u8 waveform_mode);
void UC8156_show_image(u8 *image_data, u8 update_mode, u8 waveform_mode);
void UC8156_show_image_area(u8 *image_data, int col_start, int col_size, int row_start, int row_size, u8 update_mode, u8 waveform_mode);
bool UC8156_check_status_register(u8 expected_value);
bool UC8156_check_RevID();

void UC8156_send_data_to_image_RAM_for_MTP_program(u8 *waveform_data, size_t size);

#endif /* SOLOMON_H_ */
