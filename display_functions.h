/*
 * display_functions.h
 *
 *  Created on: 24 Aug 2016
 *      Author: andreas.meier
 */

#ifndef DISPLAY_FUNCTIONS_H_
#define DISPLAY_FUNCTIONS_H_

void clear_display();
int show_image_from_SDcard(const char *image, int mode);
int show_image_from_SDcard_V2(const char *image, int mode, int waveform_table);


#endif /* DISPLAY_FUNCTIONS_H_ */
