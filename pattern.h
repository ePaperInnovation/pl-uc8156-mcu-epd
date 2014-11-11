/*
 * pattern.h
 *
 *  Created on: 11.11.2014
 *      Author: andreas.meier
 */

#ifndef PATTERN_H_
#define PATTERN_H_

#include "types.h"

void white_update();
void walking_source_line();
void diagonale();
void checkerboard_debug(u8 data, u8 s_start, u8 s_end);
void checkerboard();
void inv_checkerboard();
void alt_source_debug(u8 data);
void alt_source();
void inv_alt_source();
void alt_source_4er();

#endif /* PATTERN_H_ */
