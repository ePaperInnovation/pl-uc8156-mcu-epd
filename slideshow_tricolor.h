/*
 * slideshow_tricolor.h
 *
 *  Created on: 15.02.2021
 *      Author: zhongke.dai
 */

#include <stdio.h>
#include <string.h>

#include "types.h"
#include "FatFs/ff.h"
#include "UC8156.h"
#include "utils.h"
#include "display_functions.h"
#include "config_display_type.h"
void slideshow_tricolor_run(int mode, u16 loop_count, char *file_name, bool inv_bool);
void slideshow_tricolor_run_GL(int mode, u16 loop_count, char *file_name, int GL_name );
void slideshow_tricolor_run_path(int mode, int count, char *file_name, char *file_path, bool inv_bool );
void slideshow_tricolor_run_all(int mode, int count, char *file_name, char *file_path,   u8 transparency_key_value, u8 transparency_display_enable, u8 display_mode_select  ,bool inv_bool );

#ifndef SLIDESHOW_TRICOLOR_H_
#define SLIDESHOW_TRICOLOR_H_









#endif /* SLIDESHOW_TRICOLOR_H_ */
