#include <stdio.h>
#include <stdlib.h>

#include "FatFs/ff.h"
#include "types.h"
#include "pnm-utils.h"
#include "utils.h"
#include "UC8156.h"
#include "config.h"

#define LOG(msg) fprintf(stderr,"%s\n", msg);

#define BUFFER_LENGTH 1024

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

// load waveform data from SD-card
int sdcard_load_waveform(u8 *waveform_data, UINT length)
{
	FIL file;
	UINT count=0;
	int res;
	char path[64];

	sprintf(path, "/%s/%s", PATH, "display/waveform.bin");

	if (f_open(&file, path, FA_READ))
		return res;

	if (f_read(&file, waveform_data, length, &count))
		return res;

	if (f_close(&file))
		return res;

	if (count!=length)
		abort_now("Fatal error in: read-sd.c/sdcard_load_waveform: count!=length");

	return 0;
}

// reads Vcom value from text-file on SD-card
int sdcard_load_vcom(int *value)
{
	FIL fVcom;
	char buff[4];
	char path[64];

	sprintf(path, "/%s/%s", PATH, "display/vcom.txt");

	if (f_open(&fVcom, path, FA_READ) != FR_OK)
		return -1;

	parser_read_file_line(&fVcom, buff, 4);
	*value = parsevalue(buff, 0, 4);

	return 0;
}

// reads image data from PMG image file - part of load_image function
static int read_image_data(FIL *f, u8 *image)
{
	u8 data[BUFFER_LENGTH];
	size_t count;

	do {
		if (f_read(f, data, BUFFER_LENGTH, &count) != FR_OK)
			return -1;

		pack_4bpp(data, image, count);

		image+=count/4;

	} while (count);

	return 0;
}

// reads image data from PMG image file - part of load_image function
static int read_image_data_line_sharing(FIL *f, u8 *image)
{
	u8 data[SOURCE_LINES];
	u8 data_scrambled[SOURCE_LINES];
	size_t count;
	int i;

	do {
		if (f_read(f, data, SOURCE_LINES, &count) != FR_OK)
			return -1;

		for(i=0; i<SOURCE_LINES/2; i++)
		{
			data_scrambled[i*2]   = data[SOURCE_LINES/2+i];
			data_scrambled[i*2+1] = data[i];
		}

		pack_4bpp(data_scrambled, image, count);

		image+=count/4;
	} while (count);


	return 0;
}

// reads image data from PMG image file - part of load_image function
static int read_image_data_SOO_0(FIL *f, u8 *image)
{
	u8 data[SOURCE_LINES];
	u8 data_scrambled[SOURCE_LINES];
	size_t count;
	int i,j;

	for(j=0; j<GATE_LINES; j++)
	{
		if (f_read(f, data, SOURCE_LINES, &count) != FR_OK)
			return -1;

		for(i=0; i<SOURCE_LINES/2; i++)
		{
			data_scrambled[i] = data[i*2];
			data_scrambled[SOURCE_LINES/2+i]=data[i*2+1];
		}

		pack_4bpp(data_scrambled, image, count);
		image+=count/4;
	}

	return 0;
}

/* Reads an image from SD Card */
int sdcard_load_image(const char *image_name, u8 *image_data)
{
	FIL image_file;
	struct pnm_header hdr;
	int ret;

	if (f_open(&image_file, image_name, FA_READ) != FR_OK) {
		//LOG("Failed to open image file");
		return -1;
	}

	ret = pnm_read_header(&image_file, &hdr);

	if (ret < 0) {
		//LOG("Failed to parse PGM header");
		goto err_close_file;
	}

#ifdef WORKAROUND_FOR_SOO=1_BUG
	ret = read_image_data_SOO_0(&image_file, image_data);
#else
	#ifdef LINE_SHARING
	ret = read_image_data_line_sharing(&image_file, image_data);
	#else
	ret = read_image_data(&image_file, image_data);
	#endif
#endif

err_close_file:
	f_close(&image_file);

	return ret;
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

		fprintf(stderr,"%s\n", fno.fname);
		fprintf(stderr,"%s\n", fno.lfname);
	}
}

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

/*	fprintf(stderr,"%s\n", line);
	sscanf(line,"%[^,] %d", string, &value);
	fprintf(stderr,"%s - %d\n", string, value);
*/
	int counter=0;
	do
	{
		stat = parser_read_config_file_line(&fp, line, sizeof(line));
		fprintf(stderr,"%s\n", line);

		if (stat < 0) {
			LOG("Failed to read line");
			break;
		}

		opt = line;
		len = parser_read_str(opt, SEP, param_name, sizeof(param_name));
//		fprintf(stderr,"%d - %s\n", len, param_name);

		opt += len;
		len = parser_read_int(opt, SEP, &param_value);
//		fprintf(stderr,"%d - %d\n", len, param_value);

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
	fprintf(stderr,"param_source_lines - %d\n", value);
}

void param_gate_lines(int value)
{
	fprintf(stderr,"param_gate_lines - %d\n", value);
}

extern int DO_PROGRAM_MTP;
void param_mtp_program(int value)
{
	fprintf(stderr,"param_mtp_program - %d\n", value);
	DO_PROGRAM_MTP = value;
}

extern int MTP_ALREADY_PROGRAMMED;
void param_mtp_already_programmed(int value)
{
	fprintf(stderr,"param_mtp_alread_programmed - %d\n", value);
	MTP_ALREADY_PROGRAMMED = value;
}
