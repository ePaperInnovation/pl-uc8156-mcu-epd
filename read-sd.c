#include <stdio.h>

//#include "image-data.h"
#include "FatFs/ff.h"
#include "types.h"
#include "pnm-utils.h"
#include "utils.h"
#include "UC8156.h"
#include "config.h"

#define BUFFER_LENGTH SOURCE_LINES

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

// loads waveform from SD-card
int sdcard_load_waveform(const char *path, u8 *waveform_data)
{
	FIL file;
	UINT count=0;

	if(f_open(&file,path,FA_READ) == FR_OK)
	{
		f_read(&file, waveform_data, WAVEFORM_LENGTH, &count);
		f_close(&file);
	}
	else
		return -1;

	if (count==WAVEFORM_LENGTH)
		return 0;
	else
		return -2;
}

// reads Vcom value from text-file on SD-card
int sdcard_load_vcom(const char *filename, int *vcom_mv_value)
{
	FIL fVcom;
	char buff[4];
	int val;
	if (f_open(&fVcom, filename, FA_READ) != FR_OK) {
				//LOG("Failed to open slideshow text file [%s]", SLIDES_PATH);
				return -1;
			}

	while(parser_read_file_line(&fVcom,buff,4))
	{
		val = parsevalue(buff,0,4);

		*vcom_mv_value = val;
	}
	return 1;
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
static int read_image_data_test(FIL *f, u8 *image)
{
	u8 data[SOURCE_LINES];
	u8 data_scrambled[SOURCE_LINES];
	size_t count;
	int i;

		if (f_read(f, data, SOURCE_LINES, &count) != FR_OK)
			return -1;

		for(i=0; i<SOURCE_LINES/2; i++)
		{
			data_scrambled[i] = data[i*2];
			data_scrambled[SOURCE_LINES/2+i]=data[i*2+1];
		}

		pack_4bpp(data_scrambled, image, count);

		image+=count/4;


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

	//ret = read_image_data(&image_file, image_data);
	ret = read_image_data_SOO_0(&image_file, image_data);
	//ret = read_image_data_test(&image_file, image_data);

err_close_file:
	f_close(&image_file);

	return ret;
}
