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

void UC8179_basic_flow();
void UC8179_ini(void);
void UC8179_path_find(void);
void UC8179_KW_MODE(void);
void UC8179_KWR_MODE(void);
void UC8179_TIME(void);
void UC8171_basic_flow();
void UC8171_ini(void);
char *UC8171_image_path(char *file_name, char *file_path);
void UC8171_image_update(char *file_name, char *file_path);
void UC8171_image_clear(void);
void UC8171_image_update_S041_flash();
void UC8171_image_black(void);

void UC8179_image_BW_clear(void);
void UC8179_image_BW_black(void);
void UC8179_BW_TEST(void);
void UC8171_image_update_S041_flash_partial();
void UC8179_image_update_S0357_flash(void);
void UC8179_basic_flow_from_SD(void);
void UC8179_image_update_S0357_SD(void);
void UC8179_VCOM_Test(void);
#endif /* UC8179_FLOW_DEBUG_H_ */
