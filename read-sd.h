#ifndef READ_SD_H_
#define READ_SD_H_

#define DEFAULT_PATH Type25

int sdcard_init(void);
int sdcard_load_image(const char *image_name, u8 *image_data);
int sdcard_load_vcom(const char *filename, int *regValue);
int sdcard_load_waveform(const char *path, u8 *formdata);

int parsevalue(char* str, int start, int length);

#endif /*READ_SD_H_*/
