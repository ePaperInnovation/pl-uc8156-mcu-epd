/*
 * config.h
 *
 * Author: andreas.meier
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define SOURCE_LINES 180
#define GATE_LINES 100
#define PIXEL_COUNT (u16)GATE_LINES*SOURCE_LINES

#define PATH "180x100"

#define MTP_WAVEFORM_PROGRAMMED YES
#define MTP_VCOM_PROGRAMMED YES

#if MTP_WAVEFORM_PROGRAMMED == YES
	#define UPDATE_WAVEFORMSOURCESELECT WAVEFORM_FROM_MTP
#else
	#define UPDATE_WAVEFORMSOURCESELECT WAVEFORM_FROM_LUT
#endif

#define WORKAROUND_FOR_VCOM2VBORDER_BUG

#endif /* CONFIG_H_ */
