/*
 * config.h
 *
 * Author: andreas.meier
 */

#ifndef CONFIG_74x312_SHARED_H_
#define CONFIG_74x312_SHARED_H_

#define SOURCE_LINES 148
#define GATE_LINES 156

#define PATH "700521"

#define LINE_SHARING

static regSetting_t reg_settings[] =
{
		{0x01, 1, {0x12}},
		{0x02, 2, {0x25, 0x44}},
		{0x06, 2, {0x67, 0x55}},
		{0x0c, 4, {0x00, 0xef, 0x00, 0x9b}}, //{0, 240-1, 0, 156-1}
		{0x0d, 4, {0x00, 0x93, 0x00, 0x9b}}, //{0, 148-1, 0, 156-1}
		{0x0e, 2, {0x00, 0x9b}},             //{0, 156-1}
		{0x0f, 1, {0x02}},
		{0x18, 4, {0x50,0x01,0x24,0x07}},
		{0x1d, 1, {0x04}},
		{0x1f, 3, {0x00, 0x00, 0x00}},
		{0x44, 1, {0x60}}
};

#endif /* CONFIG_74x312_SHARED_H_ */
