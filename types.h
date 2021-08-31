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
/*
 * types.h -- General type and constant definitions
 *
 * Authors: Nick Terry <nick.terry@plasticlogic.com>
 *
 */

#ifndef TYPES_H_
#define TYPES_H_

#define	CONFIG_NO_PRINTK	1

// We have to be aware of endianess when communicating with the Epson controllers.
// MSP430 is little endian, EPSON is big endian
//
#define CPU_LITTLE_ENDIAN	1

/* borrow some type definitions from the Linux kernel
 * Note: C99 compilers support uint8_t etc so these might be more portable
 * also defined in stdint.h
 */
typedef unsigned char u8; 	/* unsigned byte (8 bits) */
typedef unsigned short u16; /* unsigned word (16 bits) */
typedef unsigned long u32; 	/* unsigned 32-bit value */
// u64; /* unsigned 64-bit value */
typedef signed char s8; 	/* signed byte (8 bits) */
typedef signed short s16; 	/* signed word (16 bits) */
typedef signed long s32; 	/* signed 32-bit value */
// s64; /* signed 64-bit value */
typedef int (*slideshow_cb_t)(const char *path);

typedef u8 bool;
enum bool_opts {
	false = 0,
	true = 1
};

typedef  s16 screen_t;		/* screen selector - a handle */

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

#ifndef min
#define min(x,y)	( (x) < (y) ? (x) : (y) )
#endif
#ifndef max
#define max(x,y)	( (x) > (y) ? (x) : (y) )
#endif

#define DIV_ROUND_CLOSEST(x, divisor)(			\
{												\
	(((x) + ((divisor) / 2)) / (divisor));		\
}												\
)

typedef union {
	short command;
	short data;
	u8 bytes[2];
} endianess; /* Typo: endianness */

typedef union {
	u32 data32;
	struct {
		u16 msw;
		u16 lsw;
	} dummy;
} param32;



typedef struct charLoc
{
	char ident;
	int	 left;
	int  top;
	int  width;
	int  height;
} charLocation;

extern u16 __bswap_16(u16 x);
extern u32 __bswap_32(u32 x);

struct area {
	int left;
	int top;
	int width;
	int height;
};

#if CPU_LITTLE_ENDIAN
#define swap_bytes(_x_)	_swap_bytes((_x_))

#define htobe16(_x) __bswap_16(_x)
#define htole16(_x) (_x)
#define be16toh(_x) __bswap_16(_x)
#define le16toh(_x) (_x)

#define htobe32(_x) __bswap_32(_x)
#define htole32(_x) (_x)
#define be32toh(_x) __bswap_32(_x)
#define le32toh(_x) (_x)

#else
#define	swap_bytes(_x_)	(_x_)
#endif


/* Linux error definitions */
#ifndef _ASM_GENERIC_ERRNO_BASE_H
#define _ASM_GENERIC_ERRNO_BASE_H

#define	EPERM		 1	/* Operation not permitted */
#define	ENOENT		 2	/* No such file or directory */
#define	ESRCH		 3	/* No such process */
#define	EINTR		 4	/* Interrupted system call */
#define	EIO		 	 5	/* I/O error */
#define	ENXIO		 6	/* No such device or address */
#define	E2BIG		 7	/* Argument list too long */
#define	ENOEXEC		 8	/* Exec format error */
#define	EBADF		 9	/* Bad file number */
#define	ECHILD		10	/* No child processes */
#define	EAGAIN		11	/* Try again */
#define	ENOMEM		12	/* Out of memory */
#define	EACCES		13	/* Permission denied */
#define	EFAULT		14	/* Bad address */
#define	ENOTBLK		15	/* Block device required */
#define	EBUSY		16	/* Device or resource busy */
#define	EEXIST		17	/* File exists */
#define	EXDEV		18	/* Cross-device link */
#define	ENODEV		19	/* No such device */
#define	ENOTDIR		20	/* Not a directory */
#define	EISDIR		21	/* Is a directory */
#define	EINVAL		22	/* Invalid argument */
#define	ENFILE		23	/* File table overflow */
#define	EMFILE		24	/* Too many open files */
#define	ENOTTY		25	/* Not a typewriter */
#define	ETXTBSY		26	/* Text file busy */
#define	EFBIG		27	/* File too large */
#define	ENOSPC		28	/* No space left on device */
#define	ESPIPE		29	/* Illegal seek */
#define	EROFS		30	/* Read-only file system */
#define	EMLINK		31	/* Too many links */
#define	EPIPE		32	/* Broken pipe */
#define	EDOM		33	/* Math argument out of domain of func */
#define	ERANGE		34	/* Math result not representable */

// manually adding missing error codes
#define	ETIME		100 /* Timeout */
#define	EDEFAULT	101 /* Operation failed - default applied */
#define	EPARAM		102	/* Invalid parameter */
#endif

// packed pixel value for GL0..3
#define GL0 0x00
#define GL1 0x55
#define GL2 0xAA
#define GL3 0xFF

#if CONFIG_NO_PRINTK
//#define	printk(fmt, ...)	(void)0
#else
#define	KERN_INFO 	"\001"
#define	KERN_ERR 	"\002"
#define NO_PRINTK	0
int printf(const char *_format, ...);
#define printk(...)	printf( __VA_ARGS__ )
#endif

#define CPU_CLOCK_SPEED_IN_HZ	20000000L
#if CPU_CLOCK_SPEED_IN_HZ < 1000000L
#error CPU_CLOCK_SPEED_IN_HZ assumed to be more than 1MHz in delay timer calculations
#endif

/* -- Sleep & delay -- */

extern void udelay(u16 us);
extern void mdelay(u16 ms);
extern void msleep(u16 ms);


extern unsigned long millis(void);
extern void timerbStart(unsigned int countVal);
extern unsigned long int timerbRead(void);
extern void timerbWait(void);
extern bool timerbFinished(void);
extern unsigned long int timerbInit(void);
extern unsigned long int timerbStop(void);


/* -- Other display related utilities -- */

extern int util_read_vcom(void);

typedef struct {
	u8 addr;
	int valCount;
	u8 val[5];
} regSetting_t;

#endif /* TYPES_H_ */
