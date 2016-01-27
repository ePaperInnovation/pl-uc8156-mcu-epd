/*
 * config.h
 *
 * Author: andreas.meier
 */

#ifndef CONFIG_148x70_H_
#define CONFIG_148x70_H_

#define SOURCE_LINES 70
#define GATE_LINES 148

#define PATH "70083x"

#define VCOM 3600

static const regSetting_t reg_settings[] =
{
		{0x01, 1, {0x12}},
		{0x02, 2, {0x25, 0xff}},
		{0x06, 2, {0x67, 0x55}},
		{0x0c, 4, {0, 240-1, 0, 160-1}},
		{0x0d, 4, {0, SOURCE_LINES-1, 0, GATE_LINES-1}},
		{0x0e, 2, {0, GATE_LINES-1}},
		{0x0f, 1, {0x02}},
		{0x18, 4, {0x00, 0x00, 0x24, 0x07}},
		{0x1d, 1, {0x04}},
		{0x1f, 3, {0x00, 0x00, 0x00}}
};

#endif /* CONFIG_148x70_H_ */
