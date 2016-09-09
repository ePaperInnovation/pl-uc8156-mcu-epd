/*
 * config_display_type.h
 *
 *  Created on: 9 Sep 2016
 *      Author: andreas.meier
 */

#ifndef CONFIG_DISPLAY_TYPE_H_
#define CONFIG_DISPLAY_TYPE_H_

#include "types.h"

enum DISPLAY_TYPE {S014_T1_1, S031_T1_1, S011_T1_1};

struct display_config_struct
{
	u16 source_lines;
	u16 gate_lines;
	regSetting_t reg_settings;
}; // global variable

void set_display_type(int display_type);

#endif /* CONFIG_DISPLAY_TYPE_H_ */
