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
#endif /* UC8177C_FLOW_DEBUG_H_ */
