#ifndef SLIDESHOW_H_
#define SLIDESHOW_H_

#include "types.h"

int show_image(const char *image, int mode);
//int slideshow_run(const char *path, slideshow_cb_t callback);
int slideshow_run(const char *path, int mode, u16 delay_ms);

#endif
