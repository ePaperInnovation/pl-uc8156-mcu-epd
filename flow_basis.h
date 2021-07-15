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

enum display_color{color_lectum =0x00, color_yellow= 0x01, color_red= 0x02 };


void waveform_type1_function(void);
void waveform_type2_function(void);
void register_vcom_set(void);
void image_eval_flow_SD(int display_color);
void path_find(void);
void tricolor_yellow(u8 waveform_place);
void tricolor_red(int mode, u8 waveform_place);
void tricolor_yellow_path(void);
void tricolor_red_path(void);
void tricolor_binary(void);
void slideshow_run_all(int mode, u16 delay_ms, int count);
void tricolor_red_partial(int mode);
void tricolor_red_all(int mode);
void tricolor_red_opti(int mode);

void image_ini_test(void);
void image_acep(void);
void image_acep_WhiteErase();
void image_acep_update(u8 *image_buffer, u8 image_type);
void Data_Entry_Mode_Setting(bool previous);
void Write_Previous_Buffer(u8 *image_buffer, u8 EDP_Clear_TYPE);
void red_color_test(u8 *image_buffer);
void waveform_type1_flash_function(void);
void waveform_type2_flash_function(void);
void HTS_test(void);
void AO_Test(void);
void tricolor_display_with_4GL_waveform(void);
void waveformType1_choose(u8 waveform_place);
void waveformType2_choose(u8 waveform_place);
void image_eval_flow_flash(int display_color);
void tricolor_yellow_flash(u8 waveform_place);
void tricolor_red_flash(int mode, u8 waveform_place);

#endif /* FLOW_BASIS_H_ */
