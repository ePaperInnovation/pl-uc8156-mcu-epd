#ifndef READ_SD_H_
#define READ_SD_H_

#define DEFAULT_PATH Type25

int sdcard_init(void);
int sdcard_load_image(const char *image_name, u8 *image_data);
int sdcard_load_vcom(int *value);
int sdcard_load_waveform(u8 *waveform_data, UINT length);

#endif /*READ_SD_H_*/
