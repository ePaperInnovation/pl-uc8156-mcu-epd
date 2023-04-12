/*
 * UC8177C_MTP.h
 *
 *  Created on: 01.11.2021
 *      Author: zhongke.dai
 */


#ifndef UC8177C_MTP_H_
#define UC8177C_MTP_H_

#include "types.h"
#include "config_display_type.h"
#include <stddef.h>

#define UC8177C_DISPLAY_SOURCE 480
#define UC8177C_DISPLAY_GATE 600


void UC8177_Eink_ini(void);
void UC8177_black_update(void);
void UC8177_black_partial_update(u16 p_x, u16 p_y, u16 area_w, u16 area_h);
void UC8177_white_partial_update(u16 p_x, u16 p_y, u16 area_w, u16 area_h);
void UC8177_white_update(void);
void UC8177_test_update(void);
void UC8177_image_update(char *image_path, u8 DN_EN);
void UC8177_image_update2(char *image_path, u8 DN_EN);
void UC8177_image_update_partial(char *image_path);
u8 flag_check(u8 flag_byte);
void buffer_check(void);
void temp_check(void);
void UC8177_black_partial_pipeline_update(u16 p_x, u16 p_y, u16 area_w, u16 area_h);
void UC8177_white_partial_all_update(void);
void UC8177_black_partial_edge_update_test(void);




#endif /* UC8179_MTP_H_ */
