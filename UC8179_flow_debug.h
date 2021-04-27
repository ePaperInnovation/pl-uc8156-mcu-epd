/*
 * UC8179_flow_debug.h
 *
 *  Created on: 21.04.2021
 *      Author: zhongke.dai
 */


#include <stdlib.h>
#include <stdio.h>
#include <UC8179.h>
#include <UC8179_MTP.h>

#include "types.h"
#include "FatFs/ff.h"
#include "display_functions.h"
#include "read-sd.h"
#include "slideshow.h"
#include "config_display_type.h"
#include "slideshow_tricolor.h"








#ifndef UC8179_FLOW_DEBUG_H_
#define UC8179_FLOW_DEBUG_H_

void UC8179_basic_flow(int display_color);
void UC8179_ini(void);
void UC8179_path_find(void);
void UC8179_KW_MODE(void);
void UC8179_KWR_MODE(void);

#endif /* UC8179_FLOW_DEBUG_H_ */
