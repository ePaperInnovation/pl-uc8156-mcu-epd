/*
 * flow_basis.h
 *
 *  Created on: 16.02.2021
 *      Author: zhongke.dai
 */
#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "UC8156.h"
#include "FatFs/ff.h"
#include "display_functions.h"
#include "read-sd.h"
#include "slideshow.h"
#include "config_display_type.h"
#include "UC8156_MTP.h"
#include "slideshow_tricolor.h"

#ifndef FLOW_BASIS_H_
#define FLOW_BASIS_H_
void waveform_type1_function(void);
void waveform_type2_function(void);
void register_vcom_set(void);
void basic_flow(int display_color);
void path_find(void);
void tricolor_yellow(void);
void tricolor_red(int mode);
void tricolor_yellow_path(void);
void tricolor_red_path(void);
void tricolor_binary(void);
void slideshow_run_all(int mode, u16 delay_ms, int count);
void tricolor_red_partial(int mode);
void tricolor_red_all(int mode);
void tricolor_red_opti(int mode);
void tricolor_yellow_test(void);
#endif /* FLOW_BASIS_H_ */
