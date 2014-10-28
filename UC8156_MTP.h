/*
 * UC8156_MTP.h
 *
 *  Created on: 15.09.2014
 *      Author: andreas.meier
 */

#ifndef UC8156_MTP_H_
#define UC8156_MTP_H_

void write_Vcom_to_MTP(u16 value);
u8 read_MTP_address(const u16 address);

void one_Byte_MTP_program(u16 address, u8 data);
void complex_MTP_program();

#endif /* UC8156_MTP_H_ */
