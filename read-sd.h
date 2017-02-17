#ifndef READ_SD_H_
#define READ_SD_H_

#include "FatFs/ff.h"

int sdcard_init(void);
void sdcard_load_image(char *image_name, u8 *image_data);
bool sdcard_load_vcom(int *value);
bool sdcard_load_waveform(char *waveform_file_name, u8 *waveform_data, UINT length);
enum DISPLAY_TYPE sdcard_read_display_type(const char *config_file_name);

int read_image_data_from_file_default(FIL *f, u8 *image_buffer);
int read_image_data_from_file_S031_T1(FIL *f, u8 *image_buffer);
int read_image_data_from_file_S021_T1(FIL *f, u8 *image_buffer);
extern int (* read_image_data_from_file) (FIL *f, u8 *image_buffer);

#endif /*READ_SD_H_*/
