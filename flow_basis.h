/*
 * flow_basis.h
 *
 *  Created on: 16.02.2021
 *      Author: zhongke.dai
 */
#include <stdlib.h>
#include <stdio.h>
#include <UC8179.h>
#include <UC8179_MTP.h>

#include "types.h"
#include "FatFs/ff.h"
#include "display_functions.h"
#include "read-sd.h"
#include "slideshow.h"
#include "config_display_type.h"
#include "slideshow_tricolor.h"

#ifndef FLOW_BASIS_H_
#define FLOW_BASIS_H_

void basic_flow(int display_color);
void uc_8179_ini(void);
void path_find(void);

#endif /* FLOW_BASIS_H_ */
