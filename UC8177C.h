/*
 * UC8177C.h
 *
 *  Created on: 01.11.2021
 *      Author: zhongke.dai
 */

#ifndef UC8177C_H_
#define UC8177C_H_

#include "types.h"
#include <stddef.h>


unsigned int UC8177_wait_for_BUSY_inactive();
unsigned long UC8177_wait_for_PowerON_ready();


void UC8177_hardware_reset(void);
void UC8177_wait_for_BUSY_inactive_debug(void);

void UC8177_check_RevID(void);
void UC8177_PSR(u8 param1, u8 param2);
void UC8177_wait_for_PowerON_ready_debug(void);
void UC8177_PWR(u8 param1, u8 param2, u8 param3, u8 param4);
void UC8177_POF(void);
void UC8177_PFS(u8 param1);
void UC8177_PON(void);
void UC8177_DPC(u8 param1);
void UC8177_DSLP(u8 param1);
void UC8177_BTST(u8 param1, u8 param2, u8 param3);
void UC8177_DTM1(u8 param1, u8 *image_data, size_t size);
void UC8177_DRF(u8 param1, u8 param2, u8 param3, u8 param4, u8 param5, u8 param6, u8 param7, u8 param8, u8 param9);
void UC8177_DTM2(u8 param1, u8 *image_data, size_t size);
void UC8177_AWM1(u8 param1);
void UC8177_AWM2(u8 param1, u8 param2, u8 param3, u8 param4);
void UC8177_LUTC(u8 *waveform_lutc);
void UC8177_LUTD(u8 param1, u8 *lutd, size_t size);
bool UC8177_Send_WaveformFile_to_LUTD(char *wf_path);
void UC8177_LUTR(u8 *lutr);
void UC8177_PS(u8 param1);
void UC8177_MISCS1(u8 param1);
void UC8177_PLL(u8 param1);
void UC8177_TSC(u8 *data);
void UC8177_TSE(u8 param1, u8 param2);
void UC8177_CDI(u8 param1, u8 param2);
u8 UC8177_LPD(void);
void UC8177_S8S9SPI(void);
void UC8177_TCON(u8 param1, u8 param2, u8 param3);
void UC8177_TRES(u8 param1, u8 param2, u8 param3, u8 param4);
void UC8177_DAM(u8 param1);
void UC8177_FLG(u8 * data);
//u8* UC8177_PIPE_COL_FLG(void);
//u8* UC8177_PIPE_BUSY_FLG(void);
void UC8177_AMV(u8 param1);
u8 UC8177_VV(void);
void UC8177_VDCS(u8 param1);
void UC8177_DTMW(u8 param1, u8 param2, u8 param3, u8 param4, u8 param5, u8 param6, u8 param7, u8 param8);
void UC8177_EDS(u8 param1);
void UC8177_XONS(u8 param1, u8 param2, u8 param3, u8 param4, u8 param5, u8 param6);
void UC8177_LEDDS(u8 param1, u8 param2);
void UC8177_PBC(u8 param1);
u8 UC8177_PBCS(void);
void UC8177_GDOS(u8 param1, u8 param2);
void UC8177_set_Vcom(int Vcom_mv_value);
bool UC8177_image_read_from_sd(char *image_path);
//u8 UC8177_BUSY_N_check(void);




#endif /* UC8177C_H_ */
