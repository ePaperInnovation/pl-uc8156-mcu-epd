#ifndef READ_SD_H_
#define READ_SD_H_

#include "FatFs/ff.h"

int sdcard_init(void);
void sdcard_load_image(char *image_name, u8 *image_data);
bool sdcard_load_vcom(int *value);
bool sdcard_load_waveform(char *waveform_file_name, u8 *waveform_data, UINT length);
int sdcard_read_display_type(const char *config_file_name);

#endif /*READ_SD_H_*/
