/*
 * solomon.h
 *
 *  Created on: 15.01.2014
 *      Author: andreas.meier
 */

#ifndef SOLOMON_H_
#define SOLOMON_H_

#include "types.h"

void UC8156_reset();
void UC8156_wait_for_BUSY_inactive();
void UC8156_init_registers();
void UC8156_power_on();
void UC8156_power_off();

void UC8156_send_waveform(u8 *waveform);
void UC8156_set_Vcom(int VCOM_mv_value);
void UC8156_send_repeated_image_data(u8);
void UC8156_send_image_data(u8 *image_data);
void UC8156_update_display();

#endif /* SOLOMON_H_ */
