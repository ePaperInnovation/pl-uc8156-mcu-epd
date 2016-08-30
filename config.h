/*
 * config.h
 *
 * Author: andreas.meier
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define S014_T1_1 //
//#define S031_T1.1
//#define S011_T1.1

#ifdef S014_T1_1

	#define SOURCE_LINES 180
	#define GATE_LINES 100

	#define PATH "S014_T1.1"

#elif defined S031_T1_1

	#define SOURCE_LINES 148
	#define GATE_LINES 156

	#define PATH "S031_T1.1"

	#define LINE_SHARING

#elif defined S011_T1.1

	#define SOURCE_LINES 70
	#define GATE_LINES 148

//	#define PATH "70083x"
	#define PATH "S011_T1.1"

#endif

#define PIXEL_COUNT (u16)GATE_LINES*SOURCE_LINES

#endif /* CONFIG_H_ */
