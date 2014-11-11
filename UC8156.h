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

enum UPDATE_MODES {FULL_UPDATE=0x01, PARTIAL_UPDATE=0x05};

#define WAVEFORM_LENGTH 120

void UC8156_reset();
void UC8156_wait_for_BUSY_inactive();
void UC8156_init_registers();
void UC8156_HVs_on();
void UC8156_HVs_off();

u8 UC8156_read_RevID();


void UC8156_send_waveform(u8 *waveform);
void UC8156_set_Vcom(int VCOM_mv_value);
void UC8156_send_image_data(u8 *image_data);
void UC8156_send_repeated_image_data(u8 image_data);
void UC8156_update_display(u8 mode);

void UC8156_send_data_to_image_RAM_for_MTP_program(u8 *waveform_data, size_t size);

#endif /* SOLOMON_H_ */
