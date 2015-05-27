/*
 * config.h
 *
 * Author: andreas.meier
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define SOURCE_LINES 240
#define GATE_LINES 160
#define PIXEL_COUNT (u16)GATE_LINES*SOURCE_LINES

#define PATH "240x160"

#define WORKAROUND_FOR_SOO=1_BUG //work-around for UC8156a/b having a bug using SOO=1

#endif /* CONFIG_H_ */
