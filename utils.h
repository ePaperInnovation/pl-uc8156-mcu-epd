/*
  Plastic Logic EPD project on MSP430

  Copyright (C) 2013 - 2017 Plastic Logic

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef INCLUDE_UTIL_H
#define INCLUDE_UTIL_H 1

#include "types.h"
#include "FatFs/ff.h"
#include <stdint.h>

/* Defines how error will be signaled on status LED */
#if 0
enum abort_error {
	ABORT_UNDEFINED = 0,     /* Undefined error */
	ABORT_MSP430_GPIO_INIT,	 /* General error initialising GPIO */
	ABORT_MSP430_COMMS_INIT, /* Error initialising MSP430 comms */
	ABORT_HWINFO,            /* Error reading HWINFO EEPROM. Could be
	                          * comms error or content error */
	ABORT_I2C_INIT,          /* Error initialising I2C (Epson) */
	ABORT_DISP_INFO,         /* Error reading display information. Could
	                          * be many errors (comms error, content error
	                          * missing or invalid file, etc).
	                          * Also depends on preprocessor settings */
	ABORT_HVPSU_INIT,        /* Error initialising HVPSU. Most likely to
	                          * be a comms error, but could indicate a
	                          * failed PMIC */
	ABORT_EPDC_INIT,         /* Error initialising EPDC. Could be many
	                          * errors (comms error, EPDC failure, failed
	                          * to load init code, failed on one of several
	                          * commands needed to initialise the EPDC,
	                          * failed to load waveform, etc) */
	ABORT_APPLICATION,       /* Failed while running application. Multiple
	                          * causes for this, depending on application
	                          * that is running. Most likely failures are
	                          * due to missing/invalid files or hardware
	                          * problems such as POK or comms failure */
	ABORT_ASSERT,            /* Failed assert statement (debug use only) */
	ABORT_CONFIG,			 /* Failed to read config file */
};
#endif
enum led_error_code {
	ABORT_UNDEFINED = 0,     /* Undefined error */
	ABORT_MSP430_INIT,	 	 /* General initialising MSP430 */
	ABORT_MSP430_COMMS_INIT,
	ABORT_UC8156_MTP,         /* Error reading or writing the MTP memory */
	ABORT_UNUSED,
	ABORT_DISP_INFO,         /* Error reading display information. Could
	                          * be many errors (comms error, content error
	                          * missing or invalid file, etc).
	                          * Also depends on preprocessor settings */
	ABORT_HV_INIT,	        /* Error initialising HVs */
	ABORT_UC8156_INIT, 		 /* Error initialising UC8156 (e.g. Read_RevID or Read_StatReg) */
	ABORT_APPLICATION,       /* Failed while running application. Multiple
	                          * causes for this, depending on application
	                          * that is running. Most likely failures are
	                          * due to missing/invalid files or hardware
	                          * problems such as POK or comms failure */
	ABORT_SD_CARD,			/* Failed to read SD card */
	ABORT_CONFIG
};

#ifdef LOG_TAG
#define LOG(msg, ...) \
	do { printf("%-8s: "msg"\n", LOG_TAG, ##__VA_ARGS__); } while (0)
#else
//#define LOG(msg, ...)
#endif

/** Check for the presence of a file in FatFs */
#define is_file_present(_path) ({ FILINFO i; f_stat(_path, &i) == FR_OK; })

/* -- Lightweight string parser -- */

/** Return the offset of the first occurence of any sep characters in str if
 * skip=0, or the first occurence of any character not in sep if skip=1 */
extern int parser_find_str(const char *str, const char *sep, int skip);

/** Copy the current string until the next separator in out and return the
 * offset to the next string, 0 if last and -1 if error */
extern int parser_read_str(const char *str, const char *sep, char *out,
			   int out_len);

/** Same as parser_read_str but convert the string to an integer */
extern int parser_read_int(const char *str, const char *sep, int *out);

/** Read a series of integers at the given addresses */
extern int parser_read_int_list(const char *str, const char *sep, int **list);

/** Read area coordinates (left, top, width, height) */
extern int parser_read_area(const char *str, const char *sep, struct area *a);

/** Read one line worth of data from a text file
 *  Return 1 if some data was read, 0 if end of file reached and no data was
 *  read but no error occured and -1 if an error occured.  */
extern int parser_read_file_line(FIL *f, char *buffer, int max_length);

/* -- Debug utilities */

/** Print the contents of a buffer with offsets on stdout */
extern void dump_hex(const void *data, uint16_t len);

/* --  Other display related utilities */

/* Converts image data to 4 bit per pixel data */
void pack_2bpp(u8 *in, u8 *out, int in_count);

/* aborts the program in case of a fatal error and prints an error message */
void abort_now(const char *error_string, enum led_error_code error_code);

#endif /* INCLUDE_UTIL_H */
