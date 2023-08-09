/*
 * UC8177C_debug.h
 *
 *  Created on: 01.11.2021
 *      Author: zhongke.dai
 */

#ifndef UC8177C_FLOW_DEBUG_H_
#define UC8177C_FLOW_DEBUG_H_

#include <stdlib.h>
#include <stdio.h>
#include <UC8177C.h>
#include <UC8177C_MTP.h>

#include "types.h"
#include "FatFs/ff.h"
#include "display_functions.h"
#include "read-sd.h"


void UC8177_basic_flow(void);
void external_flash_test(void);
int external_flash_waveform_save();
int waveform_sd_to_externalFlash();

int waveform_sd_to_externalFlash_part(char *wf_path, size_t address_start,uint32_t buffer_to_read_length);
int waveform_sd_to_externalFlash_part_no_check(char *wf_path, size_t address_start,uint32_t buffer_to_read_length);
int waveform_check(char *wf_path , uint32_t register_address_start );

int external_flash_setting(bool write_WF);
int WF_read_from_external_flash(uint32_t register_address_start);
int UC8177_VCOM_Test();
void UC8177_GP_test_flow(void);
void UC8177_slideshow_flow(u16 delay_ms);
#endif /* UC8177C_FLOW_DEBUG_H_ */
