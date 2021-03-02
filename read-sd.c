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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FatFs/ff.h"
#include "types.h"
#include "pnm-utils.h"
#include "utils.h"
#include "UC8156.h"
#include "config_display_type.h"
#include "read-sd.h"

#define LOG(msg) fprintf(stderr,"%s\n", msg);

 //global variables
extern u16 SOURCE_LINES, GATE_LINES;
extern char PATH[64];
extern bool LINE_SHARING;

int (* read_image_data_from_file) (FIL *f, u8 *image_buffer); //global definition

// global file system information used by FatFs
static FATFS Sd_Card;

// init SD-card
int sdcard_init(void)
{
	f_chdrive(0);
	return (f_mount(0,&Sd_Card) == FR_OK ? 0 : -EACCES);
}

// helper function
u8 charToHex(char upper, char lower)
{
	int uneg=48, lneg=48;

	if(upper >= 'A' && upper <='Z')
		uneg = 'A'-10;
	else if(upper >= 'a' && upper <='z')
		uneg = 'a'-10;

	if(lower >= 'A' && lower <='Z')
			lneg = 'A'-10;
	else if(lower >= 'a' && lower <='z')
		lneg = 'a'-10;

	u8 fu = (upper -uneg)<<4 | (lower-lneg);
	return fu;
}

/*Parses char values to integer*/
int parsevalue(char* str, int start, int length)
{
	int i;
	int pot=1;
	int ret=0;

	for (i = start+length-1; i >= start; i--)
	{
		if(str[i] >'9' | str[i] < '0')
			return -1;

		ret += (str[i]-'0')*pot;
		switch(pot)
		{
		case 1:
			pot=10;
			break;
		case 10:
			pot = 100;
			break;
		case 100:
			pot = 1000;
			break;
		}
	}
	return ret;
}

int parser_read_config_file_line(FIL *f, char *buffer, int max_length)
{
	size_t count;
	char *out;
	int i;

	for (i = 0, out = buffer; i < max_length; ++i, ++out) {
		if (f_read(f, out, 1, &count) != FR_OK)
			return -1;

		if ((*out == '\n') || !count)
			break;

		if (*out == '\r')
			--out;
	}

	if (i == max_length)
		return -1;

	*out = '\0';

	return !!count;
}

void param_source_lines(int value)
{
	printf("param_source_lines - %d\n", value);
}

void param_gate_lines(int value)
{
	printf("param_gate_lines - %d\n", value);
}


// load waveform data from SD-card
bool sdcard_load_waveform(char *path, u8 *waveform_data, UINT length)
{
	FIL file;
	UINT count=0;




	if (f_open(&file, path, FA_READ))
		return false;

	if (f_read(&file, waveform_data, length, &count))
		return false;

	f_close(&file);

	if (count!=length)
		return false;

	return(true);
}

// reads Vcom value from text-file on SD-card
bool sdcard_load_vcom(int *value)
{
	FIL fVcom;
	char buff[4];
	char path[64];

	sprintf(path, "/%s/%s", PATH, "display/vcom.txt");

	if (f_open(&fVcom, path, FA_READ) != FR_OK)
		return false;

	parser_read_file_line(&fVcom, buff, 4);
	*value = parsevalue(buff, 0, 4);

	return true;
}

#define BUFFER_LENGTH 256 //1024

// reads image data from PMG image file - part of load_image function
int read_image_data_from_file_default(FIL *f, u8 *image_buffer)
{
	static u8 data[BUFFER_LENGTH];
	size_t count;

	do {
		if (f_read(f, data, BUFFER_LENGTH, &count) != FR_OK)
			return -1;

		pack_2bpp(data, image_buffer, count);

		image_buffer+=count/4;

	} while (count);

	return 0;
}



// reads image data from PMG image file - part of load_image function
#define BUFFER_LENGTH_S031_T1 148
int read_image_data_from_file_S031_T1(FIL *f, u8 *image_buffer)
{
	u8 data[BUFFER_LENGTH_S031_T1];
	u8 data_scrambled[BUFFER_LENGTH_S031_T1];
	size_t count;
	int i;

	do {
		if (f_read(f, data, BUFFER_LENGTH_S031_T1, &count) != FR_OK)
			return -1;

		for(i=0; i<BUFFER_LENGTH_S031_T1/2; i++)
		{
			data_scrambled[i*2]   = data[BUFFER_LENGTH_S031_T1/2+i];
			data_scrambled[i*2+1] = data[i];
		}

		pack_2bpp(data_scrambled, image_buffer, count);

		image_buffer+=count/4;
	} while (count);

	return 0;
}

#define BUFFER_LENGTH_D011_T1 232
int read_image_data_from_file_D011_T1(FIL *f, u8 *image_buffer)
{
	u8 image_file_data[BUFFER_LENGTH_D011_T1];
	u8 data_scrambled[240];
	size_t count;
	int i,j;

	for (j=0; j<GATE_LINES; j++)
	{
		if (f_read(f, image_file_data, BUFFER_LENGTH_D011_T1, &count) != FR_OK)
			return -1;

		for(i=0; i<BUFFER_LENGTH_D011_T1; i++)
			if (image_file_data[i] > 128)
				(image_file_data[i] = 255);
		for(i=0; i<BUFFER_LENGTH_D011_T1/2; i++)
		{
			data_scrambled[115-i] = image_file_data[i];
			data_scrambled[i+120] = image_file_data[i+BUFFER_LENGTH_D011_T1/2];
		}
		pack_2bpp(data_scrambled, image_buffer+j*SOURCE_LINES/4, SOURCE_LINES);
	}
	UC8156_send_image_data(image_buffer);
	for (j=GATE_LINES-1; j>=0; j--)
	{
		if (f_read(f, image_file_data, BUFFER_LENGTH_D011_T1, &count) != FR_OK)
			return -1;
		for(i=0; i<BUFFER_LENGTH_D011_T1; i++)
			if (image_file_data[i] > 128)
				(image_file_data[i] = 255);
		for(i=0; i<BUFFER_LENGTH_D011_T1/2; i++)
		{
			data_scrambled[235-i] = image_file_data[i];
			data_scrambled[i] = image_file_data[i+BUFFER_LENGTH_D011_T1/2];
		}
		pack_2bpp(data_scrambled, image_buffer+j*SOURCE_LINES/4, SOURCE_LINES);
	}
	UC8156_send_image_data_slave(image_buffer);
	return 0;
}

// reads image data from PMG image file - part of load_image function
#define BUFFER_LENGTH_S021_T1 240
int read_image_data_from_file_S021_T1(FIL *f, u8 *image_buffer)
{
    u8 image_file_data[BUFFER_LENGTH_S021_T1];
    u8 data_scrambled[240];

    size_t count;
    int i,j;








    for (j=0; j<GATE_LINES; j++)
    {
        if (f_read(f, image_file_data, BUFFER_LENGTH_S021_T1, &count) != FR_OK)
            return -1;

        for(i=0; i<BUFFER_LENGTH_S021_T1/2; i++)
        {
            data_scrambled[239-i] = image_file_data[i];
            data_scrambled[i] = image_file_data[i+BUFFER_LENGTH_S021_T1/2];
        }

        pack_2bpp(data_scrambled, image_buffer+j*SOURCE_LINES/4, SOURCE_LINES);
    }

//    UC8156_send_image_data(image_buffer);

    return 0;
}

/* Reads an image from SD Card */
void sdcard_load_image(char *image_name, u8 *image_data)
{
	FIL image_file;
	struct pnm_header hdr;

	if (f_open(&image_file, image_name, FA_READ) != FR_OK)
		abort_now("Fatal error in: read-sd.c -> sdcard_load_image -> f_open", ABORT_SD_CARD);

	if (pnm_read_header(&image_file, &hdr) < 0)
		abort_now("Fatal error in: read-sd.c -> sdcard_load_image -> pnm_read_header", ABORT_SD_CARD);

	if (read_image_data_from_file(&image_file, image_data) != 0)
		abort_now("Fatal error in: read-sd.c -> sdcard_load_image -> read_image_data", ABORT_SD_CARD);

	f_close(&image_file);
}

void read_directory_list(const char *path)
{
	DIR dir;
	FILINFO fno;
	fno.lfsize = _MAX_LFN - 1;
	static TCHAR lfname[_MAX_LFN];
	fno.lfname = lfname;

	if (f_opendir(&dir, path) != FR_OK)
		exit(EXIT_FAILURE);

	while(1)
	{
		if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0)
			break;

		printf("%s\n", fno.fname);
		printf("%s\n", fno.lfname);
	}
}

enum DISPLAY_TYPE sdcard_read_display_type(const char *config_file_name)
{
	FIL fp;
	char display_type_string[81];

	if (f_open(&fp, config_file_name, FA_READ) != FR_OK)
		return UNKNOWN;
//		abort_now("Could not open config-file", ABORT_DISP_INFO);

	parser_read_config_file_line(&fp, display_type_string, sizeof(display_type_string));
	printf("%s\n", display_type_string);

	f_close(&fp);

	if (strcmp(display_type_string, "S011_T1.1") == 0)
		return S011_T1_1;
	else if (strcmp(display_type_string, "S011_T1.2") == 0)
		return S011_T1_2;
	else if (strcmp(display_type_string, "S014_T1.1") == 0)
		return S014_T1_1;
	else if (strcmp(display_type_string, "S014_T1.2") == 0)
		return S014_T1_2;
	else if (strcmp(display_type_string, "S021_T1.1") == 0)
		return S021_T1_1;
	else if (strcmp(display_type_string, "S031_T1.1") == 0)
		return S031_T1_1;
	else if (strcmp(display_type_string, "D011_T1.1") == 0)
		return D011_T1_1;

	return UNKNOWN;
}

/*
static const char SEP[] = ", ";

static void param_source_lines(int value);
static void param_gate_lines(int value);
static void param_mtp_program(int value);
static void param_mtp_already_programmed(int value);

void read_config_file(const char *config_file_name)
{
	FIL fp;
	int stat=0, len;
	int param_value;
	char line[81];
	char param_name[16];
	char *opt;

	struct param {
		const char *name;
		void (*func)(int value);
	};
	static const struct param param_table[] = {
		{ "source_lines", param_source_lines },
		{ "gate_lines", param_gate_lines },
		{ "do_mtp_program", param_mtp_program },
		{ "mtp_already_programmed", param_mtp_already_programmed },
		{ NULL, NULL }
	};
	const struct param *param;


	stat = f_open(&fp, config_file_name, FA_READ);
	if (stat != FR_OK)
	{
		LOG("Could not open config-file");
		exit(EXIT_FAILURE);
	}

	printf("%s\n", line);
	sscanf(line,"%[^,] %d", string, &value);
	printf("%s - %d\n", string, value);

	do
	{
		stat = parser_read_config_file_line(&fp, line, sizeof(line));
		printf("%s\n", line);

		if (stat < 0) {
			LOG("Failed to read line");
			break;
		}

		opt = line;
		len = parser_read_str(opt, SEP, param_name, sizeof(param_name));
//		printf("%d - %s\n", len, param_name);

		opt += len;
		len = parser_read_int(opt, SEP, &param_value);
//		printf("%d - %d\n", len, param_value);

		for (param = param_table; param->name != NULL; ++param) {
			if (!strcmp(param->name, param_name)) {
				param->func(param_value);
				break;
			}
		}

		if (param->name == NULL) {
			LOG("Invalid parameter");
			stat = -1;
			break;
		}

	}
	while (stat>0);

}

int DO_PROGRAM_MTP;
void param_mtp_program(int value)
{
	printf("param_mtp_program - %d\n", value);
	DO_PROGRAM_MTP = value;
}

int MTP_ALREADY_PROGRAMMED;
void param_mtp_already_programmed(int value)
{
	printf("param_mtp_alread_programmed - %d\n", value);
	MTP_ALREADY_PROGRAMMED = value;
}
*/
