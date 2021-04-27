/*
  Plastic Logic EPD project on MSP430

  Copyright (C) 2013 - 2017 Plastic Logic

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
 * UC8179_MTP.c
 *
 *  Created on: 08.04.2021
 *      Author: Zhongke.Dai & Matti.Haugwitz
 */
//#define DEBUG_PRINTOUTS_ON

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <UC8179.h>
#include <UC8179_MTP.h>

#include "msp430/msp430-spi.h"
#include "read-sd.h"
#include "utils.h"
#include "config_display_type.h"

#define WF_LIBRARY_LENGTH 2790
#define WF_1TYPEonly_LENGTH 0x4b9 //=1209d
#define MTP_TYPE2_OFFSET 0x500
#define MTP_VCOM_OFFSET 0x04b9

#define WF_TYPE1 0 // R40h.bit1 (MARS) =0
#define WF_TYPE2 2 // R40h.bit1 (MARS) =1

