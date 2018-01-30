/*
 * UC8156_MTP.h
 *
 *  Created on: 15.09.2014
 *      Author: andreas.meier
 */

#ifndef UC8156_MTP_H_
#define UC8156_MTP_H_

#include "types.h"

void write_Vcom_to_MTP(u16 value);
u8 read_MTP_address(const u16 address);

void one_Byte_MTP_program(u16 address, u8 data);
void complex_MTP_program();
void write_waveform_to_MTP(u8 *waveform_data, int waveform_data_length, int mtp_offset_pgrs, int wf_type);
void write_complete_waveform_library_to_MTP_from_file(char *filename);

void print_SerialNo_read_from_MTP();
void print_WfVersion_read_from_MTP();
void print_Display_Type_read_from_MTP();
void print_MagicWord_read_from_MTP();

enum DISPLAY_TYPE convert_string_to_DISPLAY_TYPE(const char *display_type_string);

#endif /* UC8156_MTP_H_ */
