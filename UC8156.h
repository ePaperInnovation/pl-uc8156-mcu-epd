/*
 * solomon.h
 *
 *  Created on: 15.01.2014
 *      Author: andreas.meier
 */

#ifndef SOLOMON_H_
#define SOLOMON_H_

#include "types.h"
extern u8 Waveform_Delta[90];
extern u8 Waveform_Refresh[90];


void UC8156_reset();
void UC8156_wait_for_BUSY_inactive();
void solomon_init_registers();
void solomon_send_waveform(u8 *waveform);
void solomon_send_vcom(int VCOM_mv_value);
void solomon_send_repeated_image_data(u8);
void solomon_send_image_data(u8 *image_data);
void solomon_update_display();
void solomon_reset_ram_address();
// for debug
void solomon_switch_HV_on();
void solomon_switch_HV_off();


#endif /* SOLOMON_H_ */
