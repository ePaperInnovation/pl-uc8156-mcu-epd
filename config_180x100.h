/*
 * config.h
 *
 * Author: andreas.meier
 */

#ifndef CONFIG_180x100_H_
#define CONFIG_180x100_H_

#define SOURCE_LINES 180
#define GATE_LINES 100

#define PATH "70081x"

static regSetting_t reg_settings[] =
{
		{0x01, 1, {0x12}},
		{0x02, 2, {0x25, 0xff}},
		{0x06, 2, {0x67, 0x55}},
		{0x0c, 4, {0, 0xef, 0, 0x9f}},         // {0, 240-1, 0, 160-1}
//		{0x0d, 4, {0, 180-1, 160-100, 160-1}}, // old setup using Reg[0fh]=0x02
		{0x0d, 4, {0, 0xb3, 0x3c, 0x9f}},      // {0, 180-1, 60, 160-1}
//		{0x0e, 2, {0, 160-1}},                 // old setup using Reg[0fh]=0x02
		{0x0e, 2, {0, 0x3c}},                  // {0, 60}
//		{0x0f, 1, {0x02}},                     // old setup using Reg[0fh]=0x02
		{0x0f, 1, {0x00}},
		{0x18, 4, {0x00, 0x00, 0x24, 0x07}},
		{0x1d, 1, {0x04}},
		{0x1f, 3, {0x00, 0x00, 0x00}},
		{0x44, 1, {0x60}}
};

#endif /* CONFIG_180x100_H_ */
